// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Types.hpp"
#include <random>
#include <type_traits>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework: All things random.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::rng
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get random number.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto rnum (const T _Lower, const T _Upper, const u32 _Seed = 0) -> T
	{
		auto Seed = u32(0);
		if(_Seed == 0) Seed = std::random_device()();
		else Seed = _Seed;

		auto Engine = std::minstd_rand(Seed);

		if constexpr(std::is_floating_point_v<T>)
		{
			auto Distro = std::uniform_real_distribution<T>(_Lower, _Upper);
			return Distro(Engine);
		}

		else if constexpr(std::is_integral_v<T>)
		{
			auto Distro = std::uniform_int_distribution<T>(_Lower, _Upper);
			return Distro(Engine);
		}

		else
		{
			static_assert(false, "T does not satisfy requirements!");
		}
		
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Fill buffer with random numbers.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto rbuf ( T* _Buffer, const u64 _Size, const T _Lower, const T _Upper, const u32 _Seed = 0 ) -> void
	{
		auto Seed = u32(0);
		if(_Seed == 0) Seed = std::random_device()();
		else Seed = _Seed;

		auto Engine = std::minstd_rand(Seed);

		if constexpr(std::is_floating_point_v<T>)
		{
			auto Distro = std::uniform_real_distribution<T>(_Lower, _Upper);
			for(auto i = u64(0); i < _Size; ++i) _Buffer[i] = Distro(Engine);
		}

		else if constexpr(std::is_integral_v<T>)
		{
			auto Distro = std::uniform_int_distribution<T>(_Lower, _Upper);
			for(auto i = u64(0); i < _Size; ++i) _Buffer[i] = Distro(Engine);
		}

		else
		{
			static_assert(false, "T does not satisfy requirements!");
		}
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Introduce noise on buffer's data.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T> auto rbufNoise ( T* _Buffer, const u64 _Size, const T _Lower, const T _Upper, const u32 _Seed = 0 ) -> void
	{
		auto Seed = u64(0);
		if(_Seed == 0) Seed = std::random_device()();
		else Seed = _Seed;

		auto Engine = std::minstd_rand(Seed);

		if constexpr(std::is_floating_point<T>())
		{
			auto Distro = std::uniform_real_distribution<T>(_Lower, _Upper);
			for(auto i = u64(0); i < _Size; ++i) _Buffer[i] += Distro(Engine);
		}

		else if constexpr(std::is_integral<T>())
		{
			auto Distro = std::uniform_int_distribution<T>(_Lower, _Upper);
			for(auto i = u64(0); i < _Size; ++i) _Buffer[i] += Distro(Engine);
		}

		else
		{
			static_assert(false, "T does not satisfy requirements!");
		}
	}
}
