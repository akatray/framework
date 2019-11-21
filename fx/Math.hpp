// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Types.hpp"
#include <algorithm>
#include <type_traits>
#include <cmath>
#include <numeric>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework - Blackmagic.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::math
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Constants.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	constexpr auto E = 2.71828;
	constexpr auto PI = 3.14159;

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Normalize value to 0..1 range.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto normalize ( const T _Val, const T _Min, const T _Max ) -> T
	{
		return (_Val - _Min) / (_Max - _Min);
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// X*X.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto sqr ( const T _X ) -> T { return _X * _X; }


	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Sums _X[] values.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto sum ( const u64 _Size, const T* _X ) -> T
	{
		auto Sum = T(0);
		for(auto n = u64(0); n < _Size; ++n) Sum += _X[n];

		return Sum;
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Gets magnitude of vector _X[].
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto mag ( const u64 _Size, const T* _X ) -> T
	{
		auto Mag = T(0);
		for(auto n = u64(0); n < _Size; ++n) Mag += sqr(_X[n]);
		return std::sqrt(Mag);
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Normalize vector _X[].
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto normalize ( const u64 _Size, T* _X, T _Mag ) -> void
	{
		for(auto n = u64(0); n < _Size; ++n) _X[n] /= _Mag;
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get average value in _X[].
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto mean ( const u64 _Size, const T* _X ) -> T { return sum(_Size, _X) / _Size; }
	
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Linear interpolation.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto lerp ( const T _A, const T _B, const T _T ) -> T
	{
		if constexpr(std::is_floating_point_v<T>) return _A + _T * (_B - _A);
		else static_assert(false, "T does not satisfy requirements!");
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Linear interpolation. Precise version.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto lerp2 ( const T _A, const T _B, const T _T ) -> T
	{
		if constexpr(std::is_floating_point_v<T>) return (T(1) - _T) * _A + _T * _B;
		else static_assert(false, "T does not satisfy requirements!");
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Check if number is even.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto isEven ( const T _Val ) -> bool
	{
		if constexpr(std::is_integral_v<T>)
		{
			if(_Val % 2 == 0) return true;
			else return false;
		}

		else static_assert(false, "T does not satisfy requirements!");
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get index into 1D array from 2D coordinates.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	constexpr inline auto index_r ( const u64 _X, const u64 _Y, const u64 _MaxY) -> u64 { return _Y + _MaxY * _X; }
	constexpr inline auto index_c ( const u64 _X, const u64 _Y, const u64 _MaxX) -> u64 { return _X + _MaxX * _Y; }

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get index into 1D array from 3D coordinates.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	constexpr inline auto index_r ( const u64 _X, const u64 _Y, const u64 _Z, const u64 _MaxY, const u64 _MaxZ ) -> u64
	{
		return _Z + _MaxZ * (_Y + _MaxY * _X);
	}

	constexpr inline auto index_c ( const u64 _X, const u64 _Y, const u64 _Z, const u64 _MaxX, const u64 _MaxY ) -> u64
	{
		return _X + _MaxX * (_Y + _MaxY * _Z);
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get index into 1D array from 4D coordinates.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	constexpr inline auto index_r ( const u64 _X, const u64 _Y, const u64 _Z, const u64 _W, const u64 _MaxY, const u64 _MaxZ, const u64 _MaxW ) -> u64
	{
		return _W + _MaxW * (_Z + _MaxZ * (_Y + _MaxY * _X));
	}

	constexpr inline auto index_c ( const u64 _X, const u64 _Y, const u64 _Z, const u64 _W, const u64 _MaxX, const u64 _MaxY, const u64 _MaxZ ) -> u64
	{
		return _X + _MaxX * (_Y + _MaxY * (_Z + _MaxZ * _W));
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Mean Squared Error.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> inline auto mse ( const T _Real, const T _Ideal ) -> T { return std::pow(_Real - _Ideal, 2); }

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Mean Squared Error of an array.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> inline auto mse ( const u64 _Size, const T* _Real, const T* _Ideal ) -> T
	{
		auto Sum = T(0);

		for(auto n = u64(0); n < _Size; ++n) Sum += std::pow(_Real[n] - _Ideal[n], 2);

		return Sum / _Size;
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Logistic.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> inline auto sigmoid ( const T _X ) -> T { return (T(1.0) / (T(1.0) + std::pow(E, -_X))); }
	template<class T> inline auto sigmoidDer ( const T _X ) -> T { return (sigmoid(_X) * (T(1.0) - sigmoid(_X))); }
	template<class T> constexpr inline auto sigmoidDer2 ( const T _FX ) -> T { return (_FX * (T(1.0) - _FX)); }

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// TanH.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> inline auto tanh ( const T _X ) -> T { return (T(2.0) / (T(1.0) + std::pow(E, -_X * T(2.0)))) - T(1.0); }
	template<class T> inline auto tanhDer2 ( const T _FX ) -> T { return T(1.0) - std::pow(_FX, T(2.0)); }

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// REctified Linear Unit.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto relu ( const T _X ) -> T { return std::max(T(0.0), _X); }
	template<class T> constexpr inline auto reluDer ( const T _X ) -> T { if(_X >= T(0.0)) return T(1.0); else return T(0.0); }

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Parametric REctified Linear Unit.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> constexpr inline auto prelu (const T _X, const T _A = T(0.2) ) -> T { if(_X >= T(0.0)) return _X; else return _X * _A; }
	template<class T> constexpr inline auto preluDer (const T _X, const T _A = T(0.2)) -> T { if(_X >= T(0.0)) return T(1.0); else return _A; }
}
