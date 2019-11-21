// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "./Types.hpp"
#include "./Error.hpp"
#include "./Math.hpp"

#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable:4996)
#pragma warning(disable:26451)
#pragma warning(disable:26812)
#pragma warning(disable:6385)
#pragma warning(disable:6262)
#pragma warning(disable:6001)
#pragma warning(disable:28182)
#pragma warning(disable:6308)
#pragma warning(disable:6387)
#pragma warning(disable:6386)
#include "./dep/stb_image.h"
#include "./dep/stb_image_write.h"
#include "./dep/stb_image_resize.h"
#pragma warning(pop)
#undef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#undef STB_IMAGE_RESIZE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <vector>
#include <type_traits>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework: Image utilities.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::img
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Error codes for image.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	constexpr auto ERR_EMPTY = u64(1);
	constexpr auto ERR_BAD_ARGS = u64(2);
	constexpr auto ERR_NOT_FLAT = u64(3);
	constexpr auto ERR_INCONSISTENT_DIM = u64(4);
	constexpr auto ERR_UNKNOWN_FORMAT = u64(5);
	constexpr auto ERR_LOAD_FAILED = u64(6);
	constexpr auto ERR_FAILED_TO_OPEN = u64(7);

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Peek image format by magic numbers.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	enum struct FileFormat { AUTO, NO_FILE, UNDEFINED, JPG, PNG, BMP, GIF, PSD };

	auto peekFormat ( const str& _Filename ) -> FileFormat
	{
		auto File = std::ifstream(_Filename, std::ios::binary);
		auto Sample = std::vector<u8>(4);

		if(File.is_open())
		{
			File.read(reinterpret_cast<char*>(Sample.data()), 4);

			if((Sample[0] == 0xFF) && (Sample[1] == 0xD8)) return FileFormat::JPG;
			if((Sample[0] == 0x89) && (Sample[1] == 0x50) && (Sample[2] == 0x4E) && (Sample[3] == 0x47)) return FileFormat::PNG;
			if((Sample[0] == 0x42) && (Sample[1] == 0x4D)) return FileFormat::BMP;
			if((Sample[0] == 0x47) && (Sample[1] == 0x49) && (Sample[2] == 0x46) && (Sample[3] == 0x38)) return FileFormat::GIF;
			if((Sample[0] == 0x38) && (Sample[1] == 0x42) && (Sample[2] == 0x50) && (Sample[3] == 0x53)) return FileFormat::PSD;

			return FileFormat::UNDEFINED;
		}

		return FileFormat::NO_FILE;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Image container.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> class Image 
	{
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Members.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		std::vector<T> Data;
		u64 Width;
		u64 Height;
		u64 Depth;
		template <typename C> friend class Image; 
		public:

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Constructors.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( void ) : Width(0), Height(0), Depth(0), Data(0) {}
		Image ( const u64 _Width, const u64 _Height, const u64 _Depth ) : Width(_Width), Height(_Height), Depth(_Depth), Data(_Width*_Height*_Depth) {}
		Image ( const str _Filename ) { this->load(_Filename); }
		Image ( const Image<T>& _Original ) { if(this != &_Original) *this = _Original; }
		template<class C> Image ( const Image<C>& _Original ) { *this = _Original; }
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
		// Converting copy assignment.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		template<class C> auto operator= ( const Image<C>& _Right ) -> Image<T>&
		{
			this->Width = _Right.Width;
			this->Height = _Right.Height;
			this->Depth = _Right.Depth;
			this->Data.resize(this->size());


			// Integer to real converter.
			if constexpr (std::is_integral_v<C> && std::is_floating_point_v<T>)
			{
				for(auto IdxCom = u64(0); IdxCom < this->size(); ++IdxCom) this->Data[IdxCom] = (T(1.0) / maxVal<C>()) * _Right.Data[IdxCom];
			}


			// Real to integer converter.
			if constexpr(std::is_floating_point_v<C> && std::is_integral_v<T>)
			{
				const auto [ValMin, ValMax] = std::minmax_element(_Right.Data.begin(), _Right.Data.end());

				for(auto IdxCom = u64(0); IdxCom < this->size(); ++IdxCom)
				{
					this->Data[IdxCom] = maxVal<T>() * math::normalize(_Right[IdxCom], *ValMin, *ValMax);
				}
			}
			

			return *this;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Move assignment.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto operator= ( Image<T>&& _Right ) noexcept -> Image<T>&
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
		// Trivial.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline auto operator[] ( const u64 _Index ) -> T& { return this->Data[_Index]; }
		inline auto operator[] ( const u64 _Index ) const -> const T& { return this->Data[_Index]; }
		inline auto data ( void ) -> T*  { return this->Data.data(); }
		inline auto data ( void ) const -> const T*  { return this->Data.data(); }
		inline auto isEmpty ( void ) const -> bool { if(this->Data.size() == 0) return true; else return false; }
		inline auto width ( void ) const -> u64 { return this->Width; }
		inline auto height ( void ) const -> u64 { return this->Height; }
		inline auto depth ( void ) const -> u64 { return this->Depth; }
		inline auto size ( void ) const -> u64 { return (this->Width*this->Height*this->Depth); }
		inline auto sizeInBytes ( void ) const -> u64 { return (this->size()*sizeof(T)); }
		inline auto read ( const u64 _X, const u64 _Y, const u64 _D ) -> T { return this->Data[math::index_c(_X, _Y, _D, this->Width, this->Height)]; }
		inline auto write ( const u64 _X, const u64 _Y, const u64 _D, const T _Val ) -> void { this->Data[math::index_c(_X, _Y, _D, this->Width, this->Height)] = _Val; }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Copy in sizeInBytes() amount of bytes from _Src.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto copyIn ( const T* _Src ) -> void { std::memcpy(this->Data.data(), _Src, this->sizeInBytes()); }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Load image from file.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto load ( const str& _Filename ) -> void
		{
			static_assert(std::is_same_v<T, u8>, "fx::Image<T>::load | Type not implemented.");

			auto Format = img::peekFormat(_Filename);
			if(Format == img::FileFormat::NO_FILE) throw Error("fx"s, "Image<T>"s, "load"s, img::ERR_FAILED_TO_OPEN, "Failed to open file: "s + _Filename);
			if(Format == img::FileFormat::UNDEFINED) throw Error("fx"s, "Image<T>"s, "load"s, img::ERR_UNKNOWN_FORMAT, "Unknown file format: "s + _Filename);
			

			auto Width = i32(0);
			auto Height = i32(0);
			auto Depth = i32(0);

			auto ImageData = stbi_load(_Filename.c_str(), &Width, &Height, &Depth, 0);
			if(ImageData == NULL) throw Error("fx"s, "Image<T>"s, "load"s, img::ERR_LOAD_FAILED, "stbi_load() returned NULL: "s + _Filename);


			this->Width = Width;
			this->Height = Height;
			this->Depth = Depth;

			this->Data.resize(this->size());
					
			std::memcpy(this->Data.data(), ImageData, this->sizeInBytes());
			stbi_image_free(ImageData);
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Save image to file.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto save ( const str& _Filename, const img::FileFormat _Format = img::FileFormat::AUTO ) const -> void
		{
			static_assert(std::is_same_v<T, u8>, "fx::Image<T>::save | Type not implemented.");
			if(this->isEmpty()) throw Error("fx"s, "Image<T>"s, "save"s, img::ERR_EMPTY, "Image is empty: "s + _Filename);
			

			if(_Format == img::FileFormat::AUTO) stbi_write_jpg(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data.data(), 90);
			else if(_Format == img::FileFormat::JPG) stbi_write_jpg(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data.data(), 90);
			else if(_Format == img::FileFormat::BMP) stbi_write_bmp(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data.data());
			else if(_Format == img::FileFormat::PNG) stbi_write_png(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data.data(), i32(this->Width) * i32(this->Depth));
		}
	};
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework: Image operations.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::img
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Remap channels in new order.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto remap ( const Image<T>& _Src, const std::vector<int>& _Map ) -> Image<T>
	{
		if(_Src.isEmpty()) throw Error("fx::img", "", "remap", img::ERR_EMPTY, "Image is empty.");
		if(_Src.depth() != _Map.size()) throw Error("fx::img", "", "remap", img::ERR_BAD_ARGS, " Map size != depth.");


		auto NewImage = Image<T>(_Src.width(), _Src.height(), _Src.depth());

		for(auto IdxCol = u64(0); IdxCol < _Src.size(); IdxCol += _Src.depth())
		{
			for(auto IdxCh = u64(0); IdxCh < _Src.depth(); ++IdxCh) NewImage[IdxCol+IdxCh] = _Src[IdxCol+_Map[IdxCh]];
		}


		return NewImage;
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Flatten image to single channel.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	enum struct OpFlatten { KEEP_RED, KEEP_GREEN, KEEP_BLUE, KEEP_ALPHA, MEAN };
	
	template<class T> auto flatten ( const Image<T>& _Src, const OpFlatten _Op ) -> Image<T>
	{
		if(_Src.isEmpty()) throw Error("fx::img", "", "flatten", img::ERR_EMPTY, "Image is empty.");
		

		auto NewImage = Image<T>(_Src.width(), _Src.height(), 1);
		auto IdxColDst = u64(0);

		if((_Op == OpFlatten::KEEP_RED) || (_Op == OpFlatten::KEEP_GREEN) || (_Op == OpFlatten::KEEP_BLUE) || (_Op == OpFlatten::KEEP_ALPHA))
		{
			auto Offset = u64(0);
			if(_Op == OpFlatten::KEEP_RED) Offset = 0;
			else if(_Op == OpFlatten::KEEP_GREEN) Offset = 1;
			else if(_Op == OpFlatten::KEEP_BLUE) Offset = 2;
			else if(_Op == OpFlatten::KEEP_ALPHA) Offset = 3;

			for(auto IdxColSrc = u64(0); IdxColSrc < _Src.size(); IdxColSrc += _Src.depth()) { NewImage[IdxColDst] = _Src[IdxColSrc+Offset]; ++IdxColDst; }
		}

		else if(_Op == OpFlatten::MEAN)
		{
			for(auto IdxColSrc = u64(0); IdxColSrc < _Src.size(); IdxColSrc += _Src.depth())
			{
				auto Sum = initTypeMax<T>();
				for(auto IdxCh = u64(0); IdxCh < _Src.depth(); ++IdxCh) Sum += _Src[IdxColSrc+IdxCh];
				NewImage[IdxColDst] = T(Sum / _Src.depth());

				++IdxColDst;
			}
		}


		return NewImage;
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Fatten image to new depth.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto fatten ( const Image<T>& _Src, const u64 _Depth ) -> Image<T>
	{
		if(_Src.isEmpty()) throw Error("fx::img", "", "fatten", img::ERR_EMPTY, "Image is empty.");


		auto NewImage = Image<T>(_Src.width(), _Src.height(), _Depth);
		auto IdxColDst = u64(0);

		for(auto IdxColSrc = u64(0); IdxColSrc < _Src.size(); ++IdxColSrc)
		{
			for(auto IdxCh = u64(0); IdxCh < _Depth; ++IdxCh) NewImage[IdxColDst+IdxCh] = _Src[IdxColSrc];
			IdxColDst += _Depth;
		}


		return NewImage;
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Splits channels into separate images.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto split ( const Image<T>& _Src ) -> std::vector<Image<T>>
	{
		if(_Src.isEmpty()) throw Error("fx::img", "", "split", img::ERR_EMPTY, "Image is empty.");


		auto Channels = std::vector<Image<T>>();
		for(auto IdxCh = u64(0); IdxCh < _Src.depth(); ++IdxCh) Channels.push_back(Image<T>(_Src.width(), _Src.height(), 1));


		auto IdxColDst = u64(0);

		for(auto IdxColSrc = u64(0); IdxColSrc < _Src.size(); IdxColSrc += _Src.depth())
		{
			for(auto IdxCh = u64(0); IdxCh < _Src.depth(); ++IdxCh) Channels[IdxCh][IdxColDst] = _Src[IdxColSrc+IdxCh];
			++IdxColDst;
		}


		return Channels;
	}
	
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Merge single channel images into one multi channel image.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto merge ( const std::vector<Image<T>>& _Channels ) -> Image<T>
	{
		auto Width = _Channels[0].width();
		auto Height = _Channels[0].height();
		
		for(auto& Channel : _Channels)
		{
			if(Channel.isEmpty()) throw Error("fx::img", "", "merge", img::ERR_EMPTY, "Image is empty.");
			if(Channel.depth() != 1) throw Error("fx::img", "", "merge", img::ERR_NOT_FLAT, "Image is not flat.");
			if(Channel.width() != Width) throw Error("fx::img", "", "merge", img::ERR_INCONSISTENT_DIM, "Inconsistent dimensions.");
			if(Channel.height() != Height) throw Error("fx::img", "", "merge", img::ERR_INCONSISTENT_DIM, "Inconsistent dimensions.");
		}
		

		auto NewImage = Image<T>(Width, Height, _Channels.size());
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

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Resize image.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto resize ( const Image<T>& _Src, const u64 _Width, const u64 _Height ) -> Image<T>
	{
		if(_Src.isEmpty()) throw Error("fx::img", "", "resize", img::ERR_EMPTY, "Image is empty.");
		static_assert(((std::is_same_v<T, u8>) || (std::is_same_v<T, r32>)), "fx::img::resize | Type not implemented.");
		

		auto NewImage = Image<T>(_Width, _Height, _Src.depth());

		if constexpr(std::is_same_v<T, u8>) stbir_resize_uint8(_Src.data(), i32(_Src.width()), i32(_Src.height()), 0, NewImage.data(), i32(_Width), i32(_Height), 0, i32(_Src.depth()));
		if constexpr(std::is_same_v<T, r32>) stbir_resize_float(_Src.data(), i32(_Src.width()), i32(_Src.height()), 0, NewImage.data(), i32(_Width), i32(_Height), 0, i32(_Src.depth()));


		return NewImage;
	}
}
