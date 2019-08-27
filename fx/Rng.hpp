// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Types.hpp"
#include <random>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework: Random numbers and noise.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get random number.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T> auto rng (const T _Lower, const T _Upper, const u64 _Seed = 0) -> T
	{
		auto Seed = u64(0);

		if(_Seed == 0) Seed = std::random_device()();
		else Seed = _Seed;

		auto Engine = std::minstd_rand(u32(Seed));

		if constexpr(std::is_floating_point<T>())
		{
			auto Distro = std::uniform_real_distribution<T>(_Lower, _Upper);
			return Distro(Engine);
		}

		else if constexpr(std::is_integral<T>())
		{
			auto Distro = std::uniform_int_distribution<T>(_Lower, _Upper);
			return Distro(Engine);
		}

		else
		{
			static_assert(false, "Error: fx->rng()");
		}
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Fill buffer with random numbers.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename T> auto rngBuffer (T* _Buffer, const u64 _Size, const T _Lower, const T _Upper, const u64 _Seed = 0) -> void
	{
		auto Seed = u64(0);

		if(_Seed == 0) Seed = std::random_device()();
		else Seed = _Seed;

		auto Engine = std::minstd_rand(u32(Seed));

		if constexpr(std::is_floating_point<T>())
		{
			auto Distro = std::uniform_real_distribution<T>(_Lower, _Upper);
			for(auto i = u64(0); i < _Size; ++i) _Buffer[i] = Distro(Engine);
		}

		else if constexpr(std::is_integral<T>())
		{
			auto Distro = std::uniform_int_distribution<T>(_Lower, _Upper);
			for(auto i = u64(0); i < _Size; ++i) _Buffer[i] = Distro(Engine);
		}

		else
		{
			static_assert(false, "Error: fx->rngBuffer()");
		}
	}

	/*
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Add some noise to buffer values.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class T> auto addNoise (T* _Buffer, const u64 _Size, const T _ValMin, const T _ValMax, const r32 _RngMag) -> void
	{
		auto Seed = std::random_device();
		auto Engine = std::minstd_rand(Seed());
		auto DisVal = std::uniform_real_distribution<r32>(_ValMin, _ValMax);
		auto DisDir = std::uniform_int_distribution<i32>(0, 1);
		
		for(auto i = u64(0); i < _Size; ++i)
		{
			auto Noise = DisVal(Engine) * _RngMag;

			if(DisDir(Engine) == 1) Noise *= -1;

			if((_Buffer[i] + Noise) < _ValMin) _Buffer[i] = _ValMin;
			else if((_Buffer[i] + Noise) > _ValMax) _Buffer[i] = _ValMax;
			else _Buffer[i] += Noise;
		}
	}
	*/
}

