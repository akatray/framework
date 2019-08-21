// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Types.hpp"
#include <cmath>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework - Blackmagic.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::math
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Constants.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	const auto E = r32(2.71828f);
	const auto PI = r32(3.14159f);

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get index into 1D array from 2D coordinates.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline constexpr auto index ( const u64 _X, const u64 _Y, const u64 MaxY) -> u64
	{
		return _Y + MaxY * _X;
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get index into 1D array from 3D coordinates.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline constexpr auto index ( const u64 _X, const u64 _Y, const u64 _Z, const u64 MaxY, const u64 _MaxZ ) -> u64
	{
		return _Z + _MaxZ * (_Y + MaxY * _X);
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Get index into 1D array from 4D coordinates.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline constexpr auto index ( const u64 _X, const u64 _Y, const u64 _Z, const u64 _W, const u64 _MaxY, const u64 _MaxZ, const u64 _MaxW ) -> u64
	{
		return _W + _MaxW * (_Z + _MaxZ * (_Y + _MaxY * _X));
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Object to store multidimensional object's shape.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	class Shape
	{
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Data.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		static const u64 MAX_DIMENSIONS = 4;
		u64 Count;
		u64 Dimensions[MAX_DIMENSIONS];
		public:

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Default constructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Shape ( void ) : Count(0)
		{
			for(auto Unit = u64(0); Unit < MAX_DIMENSIONS; ++Unit)
			{
				this->Dimensions[Unit] = 1;
			}
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: 1D.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Shape ( const u64 _X ) : Shape()
		{
			this->Count = 1;
			this->Dimensions[0] = _X;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: 2D.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Shape ( const u64 _X, const u64 _Y ) : Shape()
		{
			this->Count = 2;
			this->Dimensions[0] = _X;
			this->Dimensions[1] = _Y;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: 3D.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Shape ( const u64 _X, const u64 _Y, const u64 _Z ) : Shape()
		{
			this->Count = 3;
			this->Dimensions[0] = _X;
			this->Dimensions[1] = _Y;
			this->Dimensions[2] = _Z;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: 4D.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		Shape ( const u64 _X, const u64 _Y, const u64 _Z, const u64 _W ) : Shape()
		{
			this->Count = 4;
			this->Dimensions[0] = _X;
			this->Dimensions[1] = _Y;
			this->Dimensions[2] = _Z;
			this->Dimensions[2] = _W;
		}
		
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get dimension lenght.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto operator[] ( const u64 _Index ) const -> u64
		{
			return Dimensions[_Index];
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get object's space size.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto size ( void ) const -> u64
		{
			auto UnitsCount = u64(1);
			
			for(auto Unit = u64(0); Unit < MAX_DIMENSIONS; ++Unit)
			{
				UnitsCount *= this->Dimensions[Unit];
			}

			return UnitsCount;
		}

		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get index into 1D array from 2D coordinates.
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto idx ( const u64 _X, const u64 _Y ) const -> u64
		{
			return index(_X, _Y, this->Dimensions[1]);
		}

		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get index into 1D array from 3D coordinates.
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto idx ( const u64 _X, const u64 _Y, const u64 _Z ) const -> u64
		{
			return index(_X, _Y, _Z, this->Dimensions[1], this->Dimensions[2]);
		}

		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get index into 1D array from 4D coordinates.
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr auto idx ( const u64 _X, const u64 _Y, const u64 _Z, const u64 _W ) const -> u64
		{
			return index(_X, _Y, _Z, _W, this->Dimensions[1], this->Dimensions[2], this->Dimensions[3]);
		}
	};

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Logistic function.
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline auto sigmoid ( const r32 _X ) -> r32
	{
		return (1.0f / (1.0f + std::powf(E, -_X)));
	}

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Logistic function's derivative. Proper version.
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline auto sigmoidDer ( const r32 _X ) -> r32
	{
		return (sigmoid(_X) * (1.0f - sigmoid(_X)));
	}

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Logistic function's derivative. Version when logistic function was already applied.
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline auto sigmoidDerPre ( const r32 _FX ) -> r32
	{
		return (_FX * (1.0f - _FX));
	}

}