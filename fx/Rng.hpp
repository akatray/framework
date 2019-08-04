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
// Framework.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get random number.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename NumberType> auto rng (const NumberType _Lower, const NumberType _Upper) -> NumberType
	{
		auto Seed = std::random_device();
		auto Engine = std::minstd_rand(Seed());

		if constexpr(std::is_floating_point<NumberType>())
		{
			auto Distro = std::uniform_real_distribution<NumberType>(_Lower, _Upper);
			return Distro(Engine);
		}

		else if constexpr(std::is_integral<NumberType>())
		{
			auto Distro = std::uniform_int_distribution<NumberType>(_Lower, _Upper);
			return Distro(Engine);
		}

		else
		{
			static_assert(false, "Can't use type for random number generation.");
		}
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Fill buffer with random numbers.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <typename NumberType, typename BufferType> auto rng (BufferType* _Buffer, const u64 _BufferSize, const NumberType _Lower, const NumberType _Upper) -> void
	{
		auto Seed = std::random_device();
		auto Engine = std::minstd_rand(Seed());

		if constexpr(std::is_floating_point<NumberType>())
		{
			auto Distro = std::uniform_real_distribution<NumberType>(_Lower, _Upper);
			for(auto i = u64(0); i < _BufferSize; ++i) _Buffer[i] = Distro(Engine);
		}

		else if constexpr(std::is_integral<NumberType>())
		{
			auto Distro = std::uniform_int_distribution<NumberType>(_Lower, _Upper);
			for(auto i = u64(0); i < _BufferSize; ++i) _Buffer[i] = Distro(Engine);
		}

		else
		{
			static_assert(false, "Can't use type for random number generation.");
		}
	}
}

