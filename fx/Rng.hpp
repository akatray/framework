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
// Framework: All things random.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::rng
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get random number.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto rnum ( const T _Lower, const T _Upper, const u32 _Seed = 0 )
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
	// Get random number from normal distribution.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto rnum_nrm ( const rMAX _Mean = rMAX(0), const rMAX _Sigma = rMAX(1), const u32 _Seed = 0 )
	{
		auto Seed = u32(0);
		if(_Seed == 0) Seed = std::random_device()();
		else Seed = _Seed;

		auto Engine = std::minstd_rand(Seed);
		auto Distro = std::normal_distribution<T>(_Mean, _Sigma);
		
		return Distro(Engine);
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Fill buffer with random numbers.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto rbuf ( const uMAX _Size, T* _Buffer, const T _Lower, const T _Upper, const u32 _Seed = 0 )
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
	// Fill buffer with random numbers from normal distribution.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto rbuf_nrm ( const uMAX _Size, T* _Buffer, const rMAX _Mean = rMAX(0), const rMAX _Sigma = rMAX(1), const u32 _Seed = 0 )
	{
		auto Seed = u32(0);
		if(_Seed == 0) Seed = std::random_device()();
		else Seed = _Seed;

		auto Engine = std::minstd_rand(Seed);
		auto Distro = std::normal_distribution<T>(_Mean, _Sigma);

		for(auto i = u64(0); i < _Size; ++i) _Buffer[i] = Distro(Engine);
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get randomly generated string.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	auto getString ( const uMAX _Size, const u32 _Seed = 0 )
	{
		auto Seed = u32(0);
		if(_Seed == 0) Seed = std::random_device()();
		else Seed = _Seed;

		auto Engine = std::minstd_rand(Seed);
		auto Distro = std::uniform_int_distribution<u32>(97, 122);

		auto Buffer = str(_Size, 'x');
		for(auto i = uMAX(0); i < _Size; ++i) Buffer[i] = Distro(Engine);

		return Buffer;
	}
}
