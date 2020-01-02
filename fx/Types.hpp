// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Type renaming.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	using ptr = void*;
	
	using u8 = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;
	using uMAX = std::uint64_t; // Max unsigned integer.

	using i8 = std::int8_t;
	using i16 = std::int16_t;
	using i32 = std::int32_t;
	using i64 = std::int64_t;
	using iMAX = std::int64_t;  // Max unsigned integer.

	using r32 = float;
	using r64 = double;
	using rMAX = double; // Max real integer.

	using str = std::string;
	using namespace std::string_literals;

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Check if types match their expected size.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	static_assert(sizeof(ptr) == 8, "Size of fx::ptr is not 8!");
	
	static_assert(sizeof(u8) == 1, "Size of fx::u8 is not 1!");
	static_assert(sizeof(u16) == 2, "Size of fx::u16 is not 2!");
	static_assert(sizeof(u32) == 4, "Size of fx::u32 is not 4!");
	static_assert(sizeof(u64) == 8, "Size of fx::u64 is not 8!");
	
	static_assert(sizeof(i8) == 1, "Size of fx::i8 is not 1!");
	static_assert(sizeof(i16) == 2, "Size of fx::i16 is not 2!");
	static_assert(sizeof(i32) == 4, "Size of fx::i32 is not 4!");
	static_assert(sizeof(i64) == 8, "Size of fx::i64 is not 8!");
	
	static_assert(sizeof(r32) == 4, "Size of fx::r32 is not 4!");
	static_assert(sizeof(r64) == 8, "Size of fx::r64 is not 8!");


	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get minimum value type can hold.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr auto minVal ( void )
	{
		if constexpr(std::is_integral_v<T>)
		{
			if constexpr(std::is_signed_v<T>)
			{
				if constexpr(std::is_same_v<T, i8>) return T(0x00);
				if constexpr(std::is_same_v<T, i16>) return T(0x0000);
				if constexpr(std::is_same_v<T, i32>) return T(0x00000000);
				if constexpr(std::is_same_v<T, i64>) return T(0x0000000000000000);
			}
			
			else return T(0);
		}

		else static_assert(false, "Minimum value unknown!");
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get maximum value type can hold.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr auto maxVal ( void )
	{
		if constexpr(std::is_integral_v<T>)
		{
			if constexpr(std::is_signed_v<T>)
			{
				if constexpr(std::is_same_v<T, i8>) return i8(0xFF);
				if constexpr(std::is_same_v<T, i16>) return i16(0xFFFF);
				if constexpr(std::is_same_v<T, i32>) return i32(0xFFFFFFFF);
				if constexpr(std::is_same_v<T, i64>) return i64(0xFFFFFFFFFFFFFFFF);
			}
			
			else
			{
				if constexpr(std::is_same_v<T, u8>) return u8(0xFF);
				if constexpr(std::is_same_v<T, u16>) return u16(0xFFFF);
				if constexpr(std::is_same_v<T, u32>) return u32(0xFFFFFFFF);
				if constexpr(std::is_same_v<T, u64>) return u64(0xFFFFFFFFFFFFFFFF);
			}
		}

		else static_assert(false, "Maximum value unknown!");
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr auto initTypeMax ( void )
	{
		if constexpr(std::is_integral_v<T>)
		{
			if constexpr(std::is_signed_v<T>) return i64(0);
			else return u64(0);
		}

		else if constexpr(std::is_floating_point_v<T>) return r64(0.0);

		else static_assert(false, "fx::initMaxType | Type not implemented.");
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto memZero ( const u64 _Size, T* _Dst ) { std::memset(_Dst, 0, _Size * sizeof(T)); }
	template<class T> auto memCopy ( const u64 _Size, T* _Dst, const T* _Src ) { std::memcpy(_Dst, _Src, _Size * sizeof(T)); }
}
