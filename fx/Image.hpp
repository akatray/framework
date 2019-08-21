// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "./Types.hpp"
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


// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework - Image's friends.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::img
{
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
	// Options for fatten operation.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	enum struct FattenOp
	{
		MONO_TO_RG,
		MONO_TO_RGB,
		MONO_TO_RGBA
	};

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Options for remap operation.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	enum struct RemapOp
	{
		BGR
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
	// It just works.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	class Image 
	{
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Data.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Buffer<u8> Data;
		TypeToken Type; // Not templated to be able change value type on runtime.
		u64 Width;
		u64 Height;
		u64 Depth;
		public:

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Default constructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( void ) : Width(0), Height(0), Depth(0), Type(TypeToken::U8), Data() {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: Custom allocator.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( Allocator& _Alloc ) : Width(0), Height(0), Depth(0), Type(TypeToken::U8), Data(_Alloc) {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: Create empty image.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( const u64 _Width, const u64 _Height, const u64 _Depth, const TypeToken _Type = TypeToken::U8, Allocator& _Alloc = AllocDef ) : Width(_Width), Height(_Height), Depth(_Depth), Type(_Type), Data(_Alloc)
		{
			this->Data.resize(this->sizeInBytes());
			this->Data.clear();
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: Load from file.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( const str _Filename, Allocator& _Alloc = AllocDef ) : Data(_Alloc)
		{
			this->load(_Filename);
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Copy constructor. To give python feel.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( const Image& _Original )
		{
			if(this != &_Original)
			{
				*this = _Original;
			}
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Move constructor. To give python feel.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Image ( Image&& _Original ) 
		{
			if(this != &_Original)
			{
				*this = std::move(_Original);
			}
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Copy assignment. To give python feel.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto operator= ( const Image& _Right ) -> Image&
		{
			if(this != &_Right)
			{
				this->Width = _Right.Width;
				this->Height = _Right.Height;
				this->Depth = _Right.Depth;
				this->Type = _Right.Type;
				this->Data = _Right.Data; // It just works.
			}

			return *this;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Move assignment. To give python feel.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto operator= ( Image&& _Right ) -> Image&
		{
			if(this != &_Right)
			{
				this->Width = _Right.Width;
				this->Height = _Right.Height;
				this->Depth = _Right.Depth;
				this->Type = _Right.Type;
				this->Data = std::move(_Right.Data); // It just works.
			}

			return *this;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Destructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~Image ( void )
		{
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// 
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto operator() ( void ) -> Buffer<u8>& // Pray for optimizer to inline this instead of calling.
		{
			return this->Data;
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get width.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto width ( void ) const -> u64 // Pray for optimizer to inline this instead of calling.
		{
			return this->Width;
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get height.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto height ( void ) const -> u64 // Pray for optimizer to inline this instead of calling.
		{
			return this->Height;
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get depth. Also known as channels.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto depth ( void ) const -> u64 // Pray for optimizer to inline this instead of calling.
		{
			return this->Depth;
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get pixel count.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto size ( void ) const -> u64 // Pray for optimizer to inline this instead of calling.
		{
			return (this->Width * this->Height * this->Depth);
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get data size in bytes.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto sizeInBytes ( void ) const -> u64 // Pray for optimizer to inline this instead of calling.
		{
			return (this->Width * this->Height * this->Depth * getTypeSize(this->Type));
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Flatten image to single channel.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto flatten ( const img::FlattenOp _Op ) -> void
		{
			if(this->Data())
			{
				if((_Op == img::FlattenOp::KEEP_RED) || (_Op == img::FlattenOp::KEEP_GREEN) || (_Op == img::FlattenOp::KEEP_BLUE) || (_Op == img::FlattenOp::KEEP_ALPHA))
				{
					auto ChannelOffset = u64(0);

					if(_Op == img::FlattenOp::KEEP_RED) ChannelOffset = 0;
					else if(_Op == img::FlattenOp::KEEP_GREEN) ChannelOffset = 1;
					else if(_Op == img::FlattenOp::KEEP_BLUE) ChannelOffset = 2;
					else if(_Op == img::FlattenOp::KEEP_ALPHA) ChannelOffset = 3;
						
					auto NewImage = Image(this->Width, this->Height, 1, this->Type, this->Data.allocator());
						
					auto NewPixelIdx = u64(0);

					for(auto PixelIdx = u64(0); PixelIdx < this->size(); PixelIdx += this->Depth)
					{
						if(this->Type == TypeToken::U8) NewImage.Data.cast<u8>()[NewPixelIdx] = this->Data.cast<u8>()[PixelIdx + ChannelOffset];
						else if(this->Type == TypeToken::U16) NewImage.Data.cast<u16>()[NewPixelIdx] = this->Data.cast<u16>()[PixelIdx + ChannelOffset];
						else if(this->Type == TypeToken::U32) NewImage.Data.cast<u32>()[NewPixelIdx] = this->Data.cast<u32>()[PixelIdx + ChannelOffset];
						else if(this->Type == TypeToken::U64) NewImage.Data.cast<u64>()[NewPixelIdx] = this->Data.cast<u64>()[PixelIdx + ChannelOffset];
							
						else if(this->Type == TypeToken::I8) NewImage.Data.cast<i8>()[NewPixelIdx] = this->Data.cast<i8>()[PixelIdx + ChannelOffset];
						else if(this->Type == TypeToken::I16) NewImage.Data.cast<i16>()[NewPixelIdx] = this->Data.cast<i16>()[PixelIdx + ChannelOffset];
						else if(this->Type == TypeToken::I32) NewImage.Data.cast<i32>()[NewPixelIdx] = this->Data.cast<i32>()[PixelIdx + ChannelOffset];
						else if(this->Type == TypeToken::I64) NewImage.Data.cast<i64>()[NewPixelIdx] = this->Data.cast<i64>()[PixelIdx + ChannelOffset];

						else if(this->Type == TypeToken::R32) NewImage.Data.cast<r32>()[NewPixelIdx] = this->Data.cast<r32>()[PixelIdx + ChannelOffset];
						else if(this->Type == TypeToken::R64) NewImage.Data.cast<r64>()[NewPixelIdx] = this->Data.cast<r64>()[PixelIdx + ChannelOffset];

						++NewPixelIdx;
					}

					*this = std::move(NewImage);
				}

				if(_Op == img::FlattenOp::AVG)
				{
					auto NewImage = Image(this->Width, this->Height, 1, this->Type, this->Data.allocator());
						
					auto NewPixelIdx = u64(0);

					for(auto PixelIdx = u64(0); PixelIdx < this->size(); PixelIdx += this->Depth)
					{
						auto IntegerSum = u64(0);
						auto RealSum = r64(0.0);
							
						for(auto ChannelIdx = u64(0); ChannelIdx < this->Depth; ++ChannelIdx)
						{
							if(this->Type == TypeToken::U8) IntegerSum += this->Data.cast<u8>()[PixelIdx + ChannelIdx];
							else if(this->Type == TypeToken::U16) IntegerSum += this->Data.cast<u16>()[PixelIdx + ChannelIdx];
							else if(this->Type == TypeToken::U32) IntegerSum += this->Data.cast<u32>()[PixelIdx + ChannelIdx];
							else if(this->Type == TypeToken::U64) IntegerSum += this->Data.cast<u64>()[PixelIdx + ChannelIdx];

							else if(this->Type == TypeToken::I8) IntegerSum += this->Data.cast<i8>()[PixelIdx + ChannelIdx];
							else if(this->Type == TypeToken::I16) IntegerSum += this->Data.cast<i16>()[PixelIdx + ChannelIdx];
							else if(this->Type == TypeToken::I32) IntegerSum += this->Data.cast<i32>()[PixelIdx + ChannelIdx];
							else if(this->Type == TypeToken::I64) IntegerSum += this->Data.cast<i64>()[PixelIdx + ChannelIdx];

							else if(this->Type == TypeToken::R32) RealSum += this->Data.cast<r32>()[PixelIdx + ChannelIdx];
							else if(this->Type == TypeToken::R64) RealSum += this->Data.cast<r64>()[PixelIdx + ChannelIdx];
						}

						if(this->Type == TypeToken::U8) NewImage.Data.cast<u8>()[NewPixelIdx] = u8(IntegerSum / this->Depth);
						else if(this->Type == TypeToken::U16) NewImage.Data.cast<u16>()[NewPixelIdx] = u16(IntegerSum / this->Depth);
						else if(this->Type == TypeToken::U32) NewImage.Data.cast<u32>()[NewPixelIdx] = u32(IntegerSum / this->Depth);
						else if(this->Type == TypeToken::U64) NewImage.Data.cast<u64>()[NewPixelIdx] = u64(IntegerSum / this->Depth);

						else if(this->Type == TypeToken::U8) NewImage.Data.cast<i8>()[NewPixelIdx] = i8(IntegerSum / this->Depth);
						else if(this->Type == TypeToken::U16) NewImage.Data.cast<i16>()[NewPixelIdx] = i16(IntegerSum / this->Depth);
						else if(this->Type == TypeToken::U32) NewImage.Data.cast<i32>()[NewPixelIdx] = i32(IntegerSum / this->Depth);
						else if(this->Type == TypeToken::U64) NewImage.Data.cast<i64>()[NewPixelIdx] = i64(IntegerSum / this->Depth);

						else if(this->Type == TypeToken::R32) NewImage.Data.cast<r32>()[NewPixelIdx] = r32(RealSum / this->Depth);
						else if(this->Type == TypeToken::R64) NewImage.Data.cast<r64>()[NewPixelIdx] = r64(RealSum / this->Depth);
							
						++NewPixelIdx;
					}

					*this = std::move(NewImage);
				}
			}

			else
			{
				std::cout << "Error[fx->Image->flatten]\n";
				std::cout << "  Data == nullptr!\n";
			}
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Fatten image to many channels.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto fatten ( img::FattenOp _Op ) -> void
		{
			if(this->Data())
			{
				if((_Op == img::FattenOp::MONO_TO_RG) || (_Op == img::FattenOp::MONO_TO_RGB) || (_Op == img::FattenOp::MONO_TO_RGBA))
				{
					auto ChannelCount = u64(0);

					if(_Op == img::FattenOp::MONO_TO_RG) ChannelCount = 2;
					if(_Op == img::FattenOp::MONO_TO_RGB) ChannelCount = 3;
					if(_Op == img::FattenOp::MONO_TO_RGBA) ChannelCount = 4;

					auto NewImage = Image(this->Width, this->Height, ChannelCount, this->Type, this->Data.allocator());

					auto NewPixelIdx = u64(0);

					for(auto PixelIdx = u64(0); PixelIdx < this->size(); ++PixelIdx)
					{
						for(auto ChannelIdx = u64(0); ChannelIdx < ChannelCount; ++ChannelIdx)
						{
							if(this->Type == TypeToken::U8) NewImage.Data.cast<u8>()[NewPixelIdx + ChannelIdx] = this->Data.cast<u8>()[PixelIdx];
							else if(this->Type == TypeToken::U16) NewImage.Data.cast<u16>()[NewPixelIdx + ChannelIdx] = this->Data.cast<u16>()[PixelIdx];
							else if(this->Type == TypeToken::U32) NewImage.Data.cast<u32>()[NewPixelIdx + ChannelIdx] = this->Data.cast<u32>()[PixelIdx];
							else if(this->Type == TypeToken::U64) NewImage.Data.cast<u64>()[NewPixelIdx + ChannelIdx] = this->Data.cast<u64>()[PixelIdx];
								
							else if(this->Type == TypeToken::I8) NewImage.Data.cast<i8>()[NewPixelIdx + ChannelIdx] = this->Data.cast<i8>()[PixelIdx];
							else if(this->Type == TypeToken::I16) NewImage.Data.cast<i16>()[NewPixelIdx + ChannelIdx] = this->Data.cast<i16>()[PixelIdx];
							else if(this->Type == TypeToken::I32) NewImage.Data.cast<i32>()[NewPixelIdx + ChannelIdx] = this->Data.cast<i32>()[PixelIdx];
							else if(this->Type == TypeToken::I64) NewImage.Data.cast<i64>()[NewPixelIdx + ChannelIdx] = this->Data.cast<i64>()[PixelIdx];

							else if(this->Type == TypeToken::R32) NewImage.Data.cast<r32>()[NewPixelIdx + ChannelIdx] = this->Data.cast<r32>()[PixelIdx];
							else if(this->Type == TypeToken::R64) NewImage.Data.cast<r64>()[NewPixelIdx + ChannelIdx] = this->Data.cast<r64>()[PixelIdx];
						}

						NewPixelIdx += ChannelCount;
					}

					*this = std::move(NewImage);
				}
			}

			else
			{
				std::cout << "Error[fx->Image->fatten]\n";
				std::cout << "  Data == nullptr!\n";
			}
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// 
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto remap ( const img::RemapOp _Op ) -> void
		{
			if(this->Data())
			{
				auto RedIdx = u64(0);
				auto GreenIdx = u64(1);
				auto BlueIdx = u64(2);
				auto AlphaIdx = u64(3);

				if(_Op == img::RemapOp::BGR)
				{
					RedIdx = 2;
					GreenIdx = 1;
					BlueIdx = 0;
					AlphaIdx = 3;
				}

				auto NewImage = Image(this->Width, this->Height, this->Depth, this->Type, this->Data.allocator());

				auto NewPixelIdx = u64(0);

				for(auto PixelIdx = u64(0); PixelIdx < this->size(); PixelIdx += this->Depth)
				{
					if(this->Type == TypeToken::U8)
					{
						if(this->Depth >= 1) NewImage.Data.cast<u8>()[NewPixelIdx + 0] = this->Data.cast<u8>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<u8>()[NewPixelIdx + 1] = this->Data.cast<u8>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<u8>()[NewPixelIdx + 2] = this->Data.cast<u8>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<u8>()[NewPixelIdx + 3] = this->Data.cast<u8>()[PixelIdx + AlphaIdx];
					}

					if(this->Type == TypeToken::U16)
					{
						if(this->Depth >= 1) NewImage.Data.cast<u16>()[NewPixelIdx + 0] = this->Data.cast<u16>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<u16>()[NewPixelIdx + 1] = this->Data.cast<u16>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<u16>()[NewPixelIdx + 2] = this->Data.cast<u16>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<u16>()[NewPixelIdx + 3] = this->Data.cast<u16>()[PixelIdx + AlphaIdx];
					}

					if(this->Type == TypeToken::U32)
					{
						if(this->Depth >= 1) NewImage.Data.cast<u32>()[NewPixelIdx + 0] = this->Data.cast<u32>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<u32>()[NewPixelIdx + 1] = this->Data.cast<u32>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<u32>()[NewPixelIdx + 2] = this->Data.cast<u32>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<u32>()[NewPixelIdx + 3] = this->Data.cast<u32>()[PixelIdx + AlphaIdx];
					}

					if(this->Type == TypeToken::U64)
					{
						if(this->Depth >= 1) NewImage.Data.cast<u64>()[NewPixelIdx + 0] = this->Data.cast<u64>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<u64>()[NewPixelIdx + 1] = this->Data.cast<u64>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<u64>()[NewPixelIdx + 2] = this->Data.cast<u64>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<u64>()[NewPixelIdx + 3] = this->Data.cast<u64>()[PixelIdx + AlphaIdx];
					}

					if(this->Type == TypeToken::I8)
					{
						if(this->Depth >= 1) NewImage.Data.cast<i8>()[NewPixelIdx + 0] = this->Data.cast<i8>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<i8>()[NewPixelIdx + 1] = this->Data.cast<i8>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<i8>()[NewPixelIdx + 2] = this->Data.cast<i8>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<i8>()[NewPixelIdx + 3] = this->Data.cast<i8>()[PixelIdx + AlphaIdx];
					}

					if(this->Type == TypeToken::I16)
					{
						if(this->Depth >= 1) NewImage.Data.cast<i16>()[NewPixelIdx + 0] = this->Data.cast<i16>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<i16>()[NewPixelIdx + 1] = this->Data.cast<i16>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<i16>()[NewPixelIdx + 2] = this->Data.cast<i16>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<i16>()[NewPixelIdx + 3] = this->Data.cast<i16>()[PixelIdx + AlphaIdx];
					}

					if(this->Type == TypeToken::I32)
					{
						if(this->Depth >= 1) NewImage.Data.cast<i32>()[NewPixelIdx + 0] = this->Data.cast<i32>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<i32>()[NewPixelIdx + 1] = this->Data.cast<i32>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<i32>()[NewPixelIdx + 2] = this->Data.cast<i32>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<i32>()[NewPixelIdx + 3] = this->Data.cast<i32>()[PixelIdx + AlphaIdx];
					}

					if(this->Type == TypeToken::I64)
					{
						if(this->Depth >= 1) NewImage.Data.cast<i64>()[NewPixelIdx + 0] = this->Data.cast<i64>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<i64>()[NewPixelIdx + 1] = this->Data.cast<i64>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<i64>()[NewPixelIdx + 2] = this->Data.cast<i64>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<i64>()[NewPixelIdx + 3] = this->Data.cast<i64>()[PixelIdx + AlphaIdx];
					}

					if(this->Type == TypeToken::R32)
					{
						if(this->Depth >= 1) NewImage.Data.cast<r32>()[NewPixelIdx + 0] = this->Data.cast<r32>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<r32>()[NewPixelIdx + 1] = this->Data.cast<r32>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<r32>()[NewPixelIdx + 2] = this->Data.cast<r32>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<r32>()[NewPixelIdx + 3] = this->Data.cast<r32>()[PixelIdx + AlphaIdx];
					}

					if(this->Type == TypeToken::R64)
					{
						if(this->Depth >= 1) NewImage.Data.cast<r64>()[NewPixelIdx + 0] = this->Data.cast<r64>()[PixelIdx + RedIdx];
						if(this->Depth >= 2) NewImage.Data.cast<r64>()[NewPixelIdx + 1] = this->Data.cast<r64>()[PixelIdx + GreenIdx];
						if(this->Depth >= 3) NewImage.Data.cast<r64>()[NewPixelIdx + 2] = this->Data.cast<r64>()[PixelIdx + BlueIdx];
						if(this->Depth >= 4) NewImage.Data.cast<r64>()[NewPixelIdx + 3] = this->Data.cast<r64>()[PixelIdx + AlphaIdx];
					}

					NewPixelIdx += this->Depth;
				}

				*this = std::move(NewImage);
			}

			else
			{
				std::cout << "Error[fx->Image->remap]\n";
				std::cout << "  Data == nullptr!\n";
			}
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Change type.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto changeType ( const TypeToken _Target ) -> void
		{
			if(this->Data())
			{
				if((_Target == TypeToken::R32) && (this->Type == TypeToken::U8))
				{
					auto NewImage = Image(this->Width, this->Height, this->Depth, TypeToken::R32, this->Data.allocator());

					for(auto PixelIdx = u64(0); PixelIdx < this->size(); ++PixelIdx)
					{
						NewImage.Data.cast<r32>()[PixelIdx] = (1.0f / 255) * this->Data.cast<u8>()[PixelIdx];
					}

					*this = std::move(NewImage);
				}

				else if((_Target == TypeToken::U8) && (this->Type == TypeToken::R32))
				{
					auto NewImage = Image(this->Width, this->Height, this->Depth, TypeToken::U8, this->Data.allocator());
					
					for(auto PixelIdx = u64(0); PixelIdx < this->size(); ++PixelIdx)
					{
						if(this->Data.cast<r32>()[PixelIdx] > 1.0f) this->Data.cast<r32>()[PixelIdx] = 1.0f;
						if(this->Data.cast<r32>()[PixelIdx] < 0.0f) this->Data.cast<r32>()[PixelIdx] = 0.0f;
							
						NewImage.Data.cast<u8>()[PixelIdx] = u8(u8(255) * this->Data.cast<r32>()[PixelIdx]);
					}

					*this = std::move(NewImage);
				}

				else
				{
					std::cout << "Error[fx->Image->changeType]\n";
					std::cout << "  Change not implemented!\n";
				}
			}

			else
			{
				std::cout << "Error[fx->Image->changeType]\n";
				std::cout << "  Data == nullptr!\n";
			}
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Apply kernel filter. Not finished.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
		auto resize ( const u64 _Width, const u64 _Height ) -> void
		{
			if(this->Data())
			{
				if((this->Type == TypeToken::U8) || (this->Type == TypeToken::R32))
				{
					auto NewImage = Image(_Width, _Height, this->Depth, this->Type, this->Data.allocator());

					if(this->Type == TypeToken::U8) stbir_resize_uint8(this->Data.cast<u8>(), i32(this->Width), i32(this->Height), 0, NewImage.Data.cast<u8>(), i32(_Width), i32(_Height), 0, i32(this->Depth));
					if(this->Type == TypeToken::R32) stbir_resize_float(this->Data.cast<r32>(), i32(this->Width), i32(this->Height), 0, NewImage.Data.cast<r32>(), i32(_Width), i32(_Height), 0, i32(this->Depth));

					*this = std::move(NewImage);
				}

				else
				{
					std::cout << "Error[fx->Image->resize]\n";
					std::cout << "  Format not implemented!\n";
				}
			}

			else
			{
				std::cout << "Error[fx->Image->resize]\n";
				std::cout << "  Data == nullptr!\n";
			}
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Load image from file.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto load ( const str& _Filename ) -> bool
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
					this->Type = TypeToken::U8;

					this->Data.resize(this->sizeInBytes());
					
					std::memcpy(this->Data(), ImageData, this->sizeInBytes());
					stbi_image_free(ImageData);

					return true;
				}

				else
				{
					std::cout << "Error[fx->Image->load]\n";
					std::cout << "  stbi_load() failed on file " << _Filename << "!\n";
					return false;
				}
			}

			else
			{
				std::cout << "Error[fx->Image->load]\n";
				std::cout << "  Can't identify file format " << _Filename << "!\n";
				return false;
			}
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Save image to file.
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto save ( const str& _Filename, const img::FileFormat _Format = img::FileFormat::AUTO ) -> void
		{
			if(this->Data())
			{
				if(this->Type == TypeToken::U8)
				{
					if(_Format == img::FileFormat::AUTO) stbi_write_jpg(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data(), 90);
					else if(_Format == img::FileFormat::JPG) stbi_write_jpg(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data(), 90);
					else if(_Format == img::FileFormat::BMP) stbi_write_bmp(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data());
					else if(_Format == img::FileFormat::PNG) stbi_write_png(_Filename.c_str(), i32(this->Width), i32(this->Height), i32(this->Depth), this->Data(), i32(this->Width) * i32(this->Depth));

					else
					{
						std::cout << "Error[fx->Image->save]\n";
						std::cout << "  Can't save image to file " << _Filename << "! File format not implemented!\n";
					}
				}

				else
				{
					std::cout << "Error[fx->Image->save]\n";
					std::cout << "  Can't save image to file " << _Filename << "! Type not implemented!\n";
				}
			}

			else
			{
				std::cout << "Error[fx->Image->save]\n";
				std::cout << "  Can't save image to file " << _Filename << "! Data == nullptr!\n";
			}
		}
	};
}