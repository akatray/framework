// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "./Types.hpp"
#include "./Error.hpp"
#include "./Rng.hpp"

#include "./Math.hpp"
#include "./Buffer.hpp"
//#include "./Array.hpp"
#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#pragma warning(disable:4996)
#include "./dep/stb_image.h"
#include "./dep/stb_image_write.h"
#include "./dep/stb_image_resize.h"
#include <iostream>
#include <fstream>
#include <type_traits>


// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework - Image's friends.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::img
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Error codes for image.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	constexpr auto ERR_DATA_NULL = u64(1);
	constexpr auto ERR_IMAGE_NOT_FLAT = u64(2); // Had depth != 1.
	constexpr auto ERR_IMAGE_INCONSISTENT_DIM = u64(3);

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Enum for image file formats.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	enum struct FileFormat
	{
		AUTO,
		UNDEFINED,
		JPG,
		PNG,
		BMP,
		GIF,
		PSD
	};

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Options for flatten operation.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	enum struct FlattenOp
	{
		KEEP_RED, // Keep channel 0.
		KEEP_GREEN, // Keep channel 1.
		KEEP_BLUE, // Keep channel 2.
		KEEP_ALPHA, // Keep channel 3.
		AVG // Average all channels.
	};

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Peek image format by magic numbers.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	auto peekFormat ( const str& _Filename ) -> FileFormat
	{
		auto File = std::ifstream(_Filename, std::ios::binary);
		auto Sample = Buffer<u8>(4);

		if(File.is_open())
		{
			File.read(Sample.cast<char>(), 4);

			if((Sample[0] == 0xFF) && (Sample[1] == 0xD8)) return FileFormat::JPG;
			if((Sample[0] == 0x89) && (Sample[1] == 0x50) && (Sample[2] == 0x4E) && (Sample[3] == 0x47)) return FileFormat::PNG;
			if((Sample[0] == 0x42) && (Sample[1] == 0x4D)) return FileFormat::BMP;
			if((Sample[0] == 0x47) && (Sample[1] == 0x49) && (Sample[2] == 0x46) && (Sample[3] == 0x38)) return FileFormat::GIF;
			if((Sample[0] == 0x38) && (Sample[1] == 0x42) && (Sample[2] == 0x50) && (Sample[3] == 0x53)) return FileFormat::PSD;
		}

		return FileFormat::UNDEFINED;
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Inbuilt filters.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	const r32 KernelBoxBlur3x3[] = {0.11111f, 0.11111f, 0.11111f, 0.11111f, 0.11111f, 0.11111f, 0.11111f, 0.11111f, 0.11111f};
	const r32 KernelBoxBlur5x5[] = {0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f, 0.04f};



}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Provides some high level image manipulation.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> class Image 
	{
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Data.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Buffer<T> Data;
		u64 Width;
		u64 Height;
		u64 Depth;
		template <typename C> friend class Image; 
		public:

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Default constructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( void ) : Width(0), Height(0), Depth(0), Data() {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: Custom allocator.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( Allocator& _Alloc ) : Width(0), Height(0), Depth(0), Data(_Alloc) {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: Create empty image.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( const u64 _Width, const u64 _Height, const u64 _Depth, Allocator& _Alloc = AllocDef ) : Width(_Width), Height(_Height), Depth(_Depth), Data(_Alloc)
		{
			this->Data.resize(this->size());
			this->Data.clear();
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: Load from file.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( const str _Filename, Allocator& _Alloc = AllocDef ) : Data(_Alloc) { this->load(_Filename); }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Copy constructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( const Image<T>& _Original ) { if(this != &_Original) *this = _Original; }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Casting copy constructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		template<class C> Image ( const Image<C>& _Original ) { *this = _Original; }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Move constructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( Image<T>&& _Original ) noexcept { if(this != &_Original) *this = std::move(_Original); }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Copy assignment.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto operator= ( const Image<T>& _Right ) -> Image<T>&
		{
			if(this != &_Right)
			{
				this->Width = _Right.Width;
				this->Height = _Right.Height;
				this->Depth = _Right.Depth;
				this->Data = _Right.Data;
			}

			return *this;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Casting copy assignment.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		template<class C> auto operator= ( const Image<C>& _Right ) -> Image<T>&
		{
			if constexpr (((std::is_same<T, u8>::value) || (std::is_same<T, r32>::value)) && ((std::is_same<C, u8>::value) || (std::is_same<C, r32>::value)))
			{
				this->Width = _Right.Width;
				this->Height = _Right.Height;
				this->Depth = _Right.Depth;
				this->Data.resize(this->size(), _Right.Data.allocator());

				if constexpr ((std::is_same<T, r32>::value) && (std::is_same<C, u8>::value))
				{
					for(auto c = u64(0); c < this->size(); ++c) this->Data[c] = (1.0f / 255) * _Right.Data[c];
				}

				if constexpr ((std::is_same<T, u8>::value) && (std::is_same<C, r32>::value))
				{
					for(auto c = u64(0); c < this->size(); ++c)
					{
						if(_Right.Data[c] > 1.0f) this->Data[c] = T(1.0);
						else if(_Right.Data[c] < 0.0f) this->Data[c] = T(0.0);
						else this->Data[c] = T(T(255) * _Right.Data[c]);
					}
				}
			}

			else static_assert(false, "fx->Image->casting_copy: Given combination not implemented!");

			return *this;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Move assignment.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto operator= ( Image<T>&& _Right ) -> Image<T>&
		{
			if(this != &_Right)
			{
				this->Width = _Right.Width;
				this->Height = _Right.Height;
				this->Depth = _Right.Depth;
				this->Data = std::move(_Right.Data);
			}

			return *this;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Trivial functions.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline auto operator() ( void ) -> Buffer<T>&  { return this->Data; }
		inline auto operator[] ( const u64 _Index ) const -> T& { return this->Data[_Index]; }
		inline auto isNull ( void ) const -> bool { if(this->Data()) return false; else return true; }
		inline auto width ( void ) const -> u64 { return this->Width; }
		inline auto height ( void ) const -> u64 { return this->Height; }
		inline auto depth ( void ) const -> u64 { return this->Depth; }
		inline auto size ( void ) const -> u64 { return (this->Width * this->Height * this->Depth); }
		inline auto sizeInBytes ( void ) const -> u64 { return (this->Width * this->Height * this->Depth * sizeof(T)); }
		inline auto read ( const u64 _X, const u64 _Y, const u64 _D ) -> T { return this->Data()[math::index_c(_X, _Y, _D, this->Height, this->Depth)]; }
		inline auto write ( const u64 _X, const u64 _Y, const u64 _D, const T _Val ) -> void { this->Data()[math::index_c(_X, _Y, _D, this->Height, this->Depth)] = _Val; }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Flatten image to single channel.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto flatten ( const img::FlattenOp _Op ) -> void
		{
			if(this->Data())
			{
				if((_Op == img::FlattenOp::KEEP_RED) || (_Op == img::FlattenOp::KEEP_GREEN) || (_Op == img::FlattenOp::KEEP_BLUE) || (_Op == img::FlattenOp::KEEP_ALPHA))
				{
					auto Offset = u64(0);
					if(_Op == img::FlattenOp::KEEP_RED) Offset = 0;
					else if(_Op == img::FlattenOp::KEEP_GREEN) Offset = 1;
					else if(_Op == img::FlattenOp::KEEP_BLUE) Offset = 2;
					else if(_Op == img::FlattenOp::KEEP_ALPHA) Offset = 3;
						
					auto NewImage = Image<T>(this->Width, this->Height, 1, this->Data.allocator());
					auto nc = u64(0);

					for(auto c = u64(0); c < this->size(); c += this->Depth) { NewImage.Data[nc] = this->Data[c + Offset]; ++nc; }

					*this = std::move(NewImage);
				}

				if(_Op == img::FlattenOp::AVG)
				{
					auto NewImage = Image<T>(this->Width, this->Height, 1, this->Data.allocator());
					auto nc = u64(0);

					for(auto c = u64(0); c < this->size(); c += this->Depth)
					{
						if constexpr(std::is_floating_point<T>())
						{
							auto Sum = r64(0.0);
							for(auto d = u64(0); d < this->Depth; ++d) Sum += this->Data[c + d];
							NewImage()[nc] = T(Sum / this->Depth);
						}

						if constexpr(std::is_integral<T>())
						{
							auto Sum = u64(0.0);
							for(auto d = u64(0); d < this->Depth; ++d) Sum += this->Data[c + d];
							NewImage()[nc] = T(Sum / this->Depth);
						}

						++nc;
					}

					*this = std::move(NewImage);
				}
			}

			else throw Error("fx", "Image<T>", "flatten", img::ERR_DATA_NULL, "Data was nullptr.");

		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Fatten image to new depth.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto fatten ( const u64 _Depth ) -> void
		{
			if(this->Data())
			{
				auto NewImage = Image<T>(this->Width, this->Height, _Depth, this->Data.allocator());
				auto nc = u64(0);

				for(auto c = u64(0); c < this->size(); ++c)
				{
					for(auto d = u64(0); d < _Depth; ++d) NewImage()[nc + d] = this->Data[c];
					nc += _Depth;
				}

				*this = std::move(NewImage);

			}

			else throw Error("fx", "Image<T>", "fatten", img::ERR_DATA_NULL, "Data was nullptr.");
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Remap channels in new order.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto remap ( const std::vector<u64>& _Map ) -> void
		{
			if(this->Data())
			{
				if(_Map.size() == this->Depth)
				{
					auto NewImage = Image<T>(this->Width, this->Height, this->Depth, this->Data.allocator());

					for(auto c = u64(0); c < this->size(); c += this->Depth)
					{
						for(auto d = u64(0); d < this->Depth; ++d) NewImage()[c + d] = this->Data[c + _Map[d]];
					}

					*this = std::move(NewImage);
				}

				else throw str("fx->Image->remap(): Map size != depth!");
			}

			else throw Error("fx", "Image<T>", "remap", img::ERR_DATA_NULL, "Data was nullptr.");
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Resize image.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto resize ( const u64 _Width, const u64 _Height ) -> void
		{
			if(this->Data())
			{
				if constexpr ((std::is_same<T, u8>::value) || (std::is_same<T, r32>::value))
				{
					auto NewImage = Image<T>(_Width, _Height, this->Depth, this->Data.allocator());

					if constexpr (std::is_same<T, u8>::value) stbir_resize_uint8(this->Data(), i32(this->Width), i32(this->Height), 0, NewImage()(), i32(_Width), i32(_Height), 0, i32(this->Depth));
					if constexpr (std::is_same<T, r32>::value) stbir_resize_float(this->Data(), i32(this->Width), i32(this->Height), 0, NewImage()(), i32(_Width), i32(_Height), 0, i32(this->Depth));

					*this = std::move(NewImage);
				}

				else static_assert(false, "fx->Image->resize(): Component type not implemented!");
			}

			else throw Error("fx", "Image<T>", "resize", img::ERR_DATA_NULL, "Data was nullptr.");
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Copy in sizeInBytes() amount of bytes from _Src.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto copyIn ( const T* _Src ) -> void
		{
			std::memcpy(this->Data(), _Src, this->sizeInBytes());
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// 
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto shift ( const T _Value ) -> void
		{
			if(this->Data())
			{
				for(auto c = u64(0); c < this->size(); ++c) this->Data[c] += _Value;
			}

			else throw Error("fx", "Image<T>", "shift", img::ERR_DATA_NULL, "Data was nullptr.");
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// 
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto lift ( const T _Value ) -> void
		{
			if(this->Data())
			{
				for(auto c = u64(0); c < this->size(); ++c) this->Data[c] *= _Value;
			}

			else throw Error("fx", "Image<T>", "lift", img::ERR_DATA_NULL, "Data was nullptr.");
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Apply kernel filter. Not finished.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		/*
		auto filter ( const r32* _Filter, const u64 _Radius ) -> void
		{
			if(this->Data())
			{
				auto NewImage = Image(this->Width, this->Height, this->Depth, this->Type, this->Data.allocator());
					
				const auto RadiusMin = -i64(_Radius);
				const auto RadiusMax = i64(_Radius+1);
					
				for(auto d = u64(0); d < this->Depth; ++d) { for(auto y = i64(0); y < i64(this->Height); ++y) { for(auto x = i64(0); x < i64(this->Width); ++x)
				{
					auto Result = r32(0.0f);
					auto FilterOff = u64(0);

					for(auto yf = RadiusMin; yf != RadiusMax; ++yf) { for(auto xf = RadiusMin; xf != RadiusMax; ++xf)
					{
						if(((x + xf) < 0) || ((y + yf) < 0) || ((x + xf) >= i64(this->Width)) || ((y + yf) >= i64(this->Height)))
						{
							++FilterOff;
						}

						else
						{
							Result += this->Data.cast<r32>()[math::index(x + xf, y + yf, d, this->Height, this->Depth)] * _Filter[FilterOff];
							++FilterOff;
						}
					}}

					NewImage.Data.cast<r32>()[math::index(x, y, d, this->Height, this->Depth)] = Result;

				}}}

				*this = std::move(NewImage);
			}

			else
			{
				std::cout << "Error[fx->Image->filter]\n";
				std::cout << "  Data == nullptr!\n";
			}
		}
		


		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Resize image.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto cut ( const u64 _V, const u64 _H ) -> std::vector<Image>
		{
			if(this->Data())
			{
				if((this->Type == TypeToken::U8) || (this->Type == TypeToken::R32))
				{
					auto W = this->Width / _V;
					auto H = this->Height / _H;
					auto S = math::Shape(this->Width, this->Height, this->Depth);

					auto Cuts = std::vector<Image>(_V * _H);

					auto ox = u64(0);
					auto oy = u64(0);
					
					for(auto c = u64(0); c < Cuts.size(); ++c)
					{
						auto x = u64(0);
						auto y = u64(0);
						auto d = u64(0);
						
						Cuts[c] = Image(this->Width, this->Height, this->Depth, this->Type, this->Data.allocator());

						for(auto p = u64(0); p < this->size(); ++p)
						{
							Cuts[c].Data.cast<r32>()[p] = this->Data.cast<r32>()[S.idx(ox + x, oy + y, d)];

							++d; if(d == this->Depth) {d = 0; ++x;}
							if(x == W) {x = 0; ++y;}
						}

						ox += W; if(ox >= this->Width) {ox = 0; oy += H;}
					}

					return std::move(Cuts);
				}

			}
		}
		*/
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Load image from file.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto load ( const str& _Filename ) -> bool
		{
			if constexpr (std::is_same<T, u8>::value)
			{
				auto Format = img::peekFormat(_Filename);

				if(Format != img::FileFormat::UNDEFINED)
				{
					auto Width = i32(0);
					auto Height = i32(0);
					auto Depth = i32(0);

					auto ImageData = stbi_load(_Filename.c_str(), &Width, &Height, &Depth, 0);

					if(ImageData != NULL)
					{
						this->Width = Width;
						this->Height = Height;
						this->Depth = Depth;

						this->Data.resize(this->size());
					
						std::memcpy(this->Data(), ImageData, this->sizeInBytes());
						stbi_image_free(ImageData);

						return true;
					}

					else
					{
						std::cout << "fx->Image->load(" << _Filename << "): stbi_load() failed on file!";
						return false;
					}
				}

				else
				{
					std::cout << "fx->Image->load(" << _Filename << "): Can't identify file format!";
					return false;
				}
			}

			else static_assert(false, "fx->Image->load(): Component type not implemented!");
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Save image to file.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto save ( const str& _Filename, const img::FileFormat _Format = img::FileFormat::AUTO ) -> void
		{
			if(this->Data())
			{
				if constexpr (std::is_same<T, u8>::value)
				{
					if(_Format == img::FileFormat::AUTO) stbi_write_jpg(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data(), 90);
					else if(_Format == img::FileFormat::JPG) stbi_write_jpg(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data(), 90);
					else if(_Format == img::FileFormat::BMP) stbi_write_bmp(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data());
					else if(_Format == img::FileFormat::PNG) stbi_write_png(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data(), i32(this->Width) * i32(this->Depth));

					else throw str("fx->Image->save(") + _Filename + str("): File format not implemented!");
				}

				else static_assert(false, "fx->Image->save(): Component type not implemented!");
			}

			else throw str("fx->Image->save(") + _Filename + str("): Data was nullptr!");
		}
	};
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework: Image operations.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::img
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Splits channels into separate images.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto split ( const Image<T>& _Src, Allocator& _Alloc = AllocDef ) -> std::vector<Image<T>>
	{
		if(_Src.isNull()) throw Error("fx::img", "", "split", img::ERR_DATA_NULL, "Image is empty.");


		auto Channels = std::vector<Image<T>>();
		for(auto IdxCh = u64(0); IdxCh < _Src.depth(); ++IdxCh) Channels.push_back(Image<T>(_Src.width(), _Src.height(), 1, _Alloc));


		auto IdxColDst = u64(0);

		for(auto IdxColSrc = u64(0); IdxColSrc < _Src.size(); IdxColSrc += _Src.depth())
		{
			for(auto IdxCh = u64(0); IdxCh < _Src.depth(); ++IdxCh) Channels[IdxCh][IdxColDst] = _Src[IdxColSrc + IdxCh];
			++IdxColDst;
		}


		return Channels;
	}
	
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Merge single channel images into one multi channel image.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto merge ( const std::vector<Image<T>>& _Channels, Allocator& _Alloc = AllocDef ) -> Image<T>
	{
		auto Width = _Channels[0].width();
		auto Height = _Channels[0].height();
		

		for(auto& Channel : _Channels)
		{
			if(Channel.isNull()) throw Error("fx::img", "", "merge", img::ERR_DATA_NULL, "Image is empty.");
			if(Channel.depth() != 1) throw Error("fx::img", "", "merge", img::ERR_IMAGE_NOT_FLAT, "Image is not flat.");
			if(Channel.width() != Width) throw Error("fx::img", "", "merge", img::ERR_IMAGE_INCONSISTENT_DIM, "Inconsistent dimensions!");
			if(Channel.height() != Height) throw Error("fx::img", "", "merge", img::ERR_IMAGE_INCONSISTENT_DIM, "Inconsistent dimensions!");
		}
		

		auto NewImage = Image<T>(Width, Height, _Channels.size(), _Alloc);
		auto IdxColSrc = u64(0);

		for(auto IdxColDst = u64(0); IdxColDst < NewImage.size(); IdxColDst += NewImage.depth())
		{
			for(auto IdxCh = u64(0); IdxCh < NewImage.depth(); ++IdxCh)
			{
				NewImage[IdxColDst + IdxCh] = _Channels[IdxCh][IdxColSrc];
			}
					
			++IdxColSrc;
		}

		return NewImage;
	}
}