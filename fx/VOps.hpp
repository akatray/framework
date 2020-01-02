// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <fx/Types.hpp>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework: Vectorized operations.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::vops
{
	template<class T> constexpr inline auto mulOutByConst ( const uMAX _Size, T* _Out, const T _Const )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] *= _Const;
	}

	template<class T> constexpr inline auto divOutByConst ( const uMAX _Size, T* _Out, const T _Const )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] /= _Const;
	}
		
	template<class T> constexpr inline auto mulVecByConst ( const uMAX _Size, T* _Out, const T* _Vec, const T _Const )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] = _Vec[i] * _Const;
	}

	template<class T> constexpr inline auto mulVecByConstAddToOut ( const uMAX _Size, T* _Out, const T* _Vec, const T _Const )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] += (_Vec[i] * _Const);
	}

	template<class T> constexpr inline auto mulVecByConstSubFromOut ( const uMAX _Size, T* _Out, const T* _Vec, const T _Const )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] -= (_Vec[i] * _Const);
	}

	template<class T> constexpr inline auto mulOutByVec ( const uMAX _Size, T* _Out, const T* _Vec )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] *= _Vec[i];
	}

	template<class T> constexpr inline auto mulVecByVec ( const uMAX _Size, T* _Out, const T* _Vec0, const T* _Vec1 )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] = _Vec0[i] * _Vec1[i];
	}

	template<class T> constexpr inline auto addVecToOut ( const uMAX _Size, T* _Out, const T* _Vec )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] += _Vec[i];
	}
		
	template<class T> constexpr inline auto addVecToVec ( const uMAX _Size, T* _Out, const T* _Vec0, const T* _Vec1 )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] = _Vec0[i] + _Vec1[i];
	}

	template<class T> constexpr inline auto subConstFromOut ( const uMAX _Size, T* _Out, const T _Const )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] -= _Const;
	}

	template<class T> constexpr inline auto subVecFromOut ( const uMAX _Size, T* _Out, const T* _Vec )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] -= _Vec[i];
	}
		
	template<class T> constexpr inline auto subVecFromVec ( const uMAX _Size, T* _Out, const T* _Vec0, const T* _Vec1 )
	{
		for(auto i = uMAX(0); i < _Size; ++i) _Out[i] = _Vec0[i] - _Vec1[i];
	}

	template<class T> constexpr inline auto mulVecByVecSum ( const uMAX _Size, const T* _Vec0, const T* _Vec1 ) -> T
	{
		auto Sum = T(0.0);
		for(auto i = uMAX(0); i < _Size; ++i) Sum += _Vec0[i] * _Vec1[i];
		return Sum;
	}
	
}
