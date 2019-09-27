// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "./Types.hpp"
#include "./Math.hpp"

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework utilites.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::utl
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Multidimensional object's shape.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	constexpr auto SHAPE_MAX_DIMENSIONS = u64(4); // Changing this number will not magicly generate fuctions for higher dimensions.
	
	class Shape
	{
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Data.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		u64 Count;
		u64 Dimensions[SHAPE_MAX_DIMENSIONS];
		public:

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Default constructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr Shape ( void ) : Count(0), Dimensions{1, 1, 1, 1} {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: 1D.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr Shape ( const u64 _X ) : Count(1), Dimensions{_X, 1, 1, 1} {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: 2D.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr Shape ( const u64 _X, const u64 _Y ) : Count(2), Dimensions{_X, _Y, 1, 1} {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: 3D.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr Shape ( const u64 _X, const u64 _Y, const u64 _Z ) : Count(3), Dimensions{_X, _Y, _Z, 1} {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor: 4D.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr Shape ( const u64 _X, const u64 _Y, const u64 _Z, const u64 _W ) : Count(4), Dimensions{_X, _Y, _Z, _W} {}
		
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Compare shapes.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto operator== ( const Shape& _Right ) const -> bool
		{
			if(this->Count != _Right.Count) return false;
			for(auto u = u64(0); u < SHAPE_MAX_DIMENSIONS; ++u) if(this->Dimensions[u] != _Right.Dimensions[u]) return false;
			return true;
		}

		constexpr inline auto operator!= ( const Shape& _Right ) const -> bool
		{
			return !(*this == _Right);
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get dimension lenght.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto operator[] ( const u64 _Index ) const -> u64 { return Dimensions[_Index]; }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get dimensions count.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto count ( void ) const -> u64 { return this->Count; }
		
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get object's space size.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto size ( void ) const -> u64
		{
			auto UnitsCount = u64(1);
			
			for(auto u = u64(0); u < SHAPE_MAX_DIMENSIONS; ++u) UnitsCount *= this->Dimensions[u];

			return UnitsCount;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get index into 1D array from 2D position.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto idx ( const u64 _X, const u64 _Y ) const -> u64 { return math::index(_X, _Y, this->Dimensions[1]); }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get index into 1D array from 3D position.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto idx ( const u64 _X, const u64 _Y, const u64 _Z ) const -> u64 { return math::index(_X, _Y, _Z, this->Dimensions[1], this->Dimensions[2]); }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get index into 1D array from 4D position.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto idx ( const u64 _X, const u64 _Y, const u64 _Z, const u64 _W ) const -> u64
		{
			return math::index(_X, _Y, _Z, _W, this->Dimensions[1], this->Dimensions[2], this->Dimensions[3]);
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Check if 2D position is inside object's space.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto isInside ( const i64 _X, const i64 _Y ) const -> bool
		{ 
			if(_X >= i64(this->Dimensions[0])) return false;
			if(_X < i64(0)) return false;
			
			if(_Y >= i64(this->Dimensions[1])) return false;
			if(_Y < i64(0)) return false;
			
			return true;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Check if 3D position is inside object's space.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto isInside ( const i64 _X, const i64 _Y, const i64 _Z ) const -> bool
		{ 
			if(_X >= i64(this->Dimensions[0])) return false;
			if(_X < i64(0)) return false;
			
			if(_Y >= i64(this->Dimensions[1])) return false;
			if(_Y < i64(0)) return false;

			if(_Z >= i64(this->Dimensions[2])) return false;
			if(_Z < i64(0)) return false;
			
			return true;
		}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Check if 4D position is inside object's space.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto isInside ( const i64 _X, const i64 _Y, const i64 _Z, const i64 _W ) const -> bool
		{ 
			if(_X >= i64(this->Dimensions[0])) return false;
			if(_X < i64(0)) return false;
			
			if(_Y >= i64(this->Dimensions[1])) return false;
			if(_Y < i64(0)) return false;

			if(_Z >= i64(this->Dimensions[2])) return false;
			if(_Z < i64(0)) return false;

			if(_W >= i64(this->Dimensions[3])) return false;
			if(_W < i64(0)) return false;
			
			return true;
		}
	};

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Multidimensional iterator.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> class ShapeIter
	{
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Data.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		bool IsDone;
		Shape Shp;
		T Step[SHAPE_MAX_DIMENSIONS];
		T Pos[SHAPE_MAX_DIMENSIONS];
		public:

		//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructors.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr ShapeIter ( const Shape _Shp ) : IsDone(false), Shp(_Shp), Step{T(1), T(1), T(1), T(1)}, Pos{T(0), T(0), T(0), T(0)} {}
		constexpr ShapeIter ( const Shape _Shp, const T _SX ) : IsDone(false), Shp(_Shp), Step{_SX, T(1), T(1), T(1)}, Pos{T(0), T(0), T(0), T(0)} {}
		constexpr ShapeIter ( const Shape _Shp, const T _SX, const T _SY ) : IsDone(false), Shp(_Shp), Step{_SX, _SY, T(1), T(1)}, Pos{T(0), T(0), T(0), T(0)} {}
		constexpr ShapeIter ( const Shape _Shp, const T _SX, const T _SY, const T _SZ ) : IsDone(false), Shp(_Shp), Step{_SX, _SY, _SZ, T(1)}, Pos{T(0), T(0), T(0), T(0)} {}
		constexpr ShapeIter ( const Shape _Shp, const T _SX, const T _SY, const T _SZ, const T _SW ) : IsDone(false), Shp(_Shp), Step{_SX, _SY, _SZ, _SW}, Pos{T(0), T(0), T(0), T(0)} {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Trivial Set/Get functions.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto operator[] ( const u64 _Index ) const -> u64 { return u64(Pos[_Index]); }
		constexpr inline auto isDone ( void ) const -> bool { return this->IsDone; }
		
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Get index into shape at current position.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto idx ( void ) const -> u64
		{
			if(this->Shp.count() == 1) return u64(Pos[0]);
			if(this->Shp.count() == 2) return this->Shp.idx(u64(Pos[0]), u64(Pos[1]));
			if(this->Shp.count() == 3) return this->Shp.idx(u64(Pos[0]), u64(Pos[1]), u64(Pos[2]));
			if(this->Shp.count() == 4) return this->Shp.idx(u64(Pos[0]), u64(Pos[1]), u64(Pos[2]), u64(Pos[3]));
		}
		
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Next step in space.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		constexpr inline auto next ( void ) -> void
		{
			if((this->Shp.count() > 0) && !this->IsDone)
			{
				this->Pos[0] += this->Step[0];
			
				if(this->Pos[0] >= this->Shp[0])
				{
					if(this->Shp.count() == 1) this->IsDone = true;

					else
					{
						this->Pos[0] = T(0);
						this->Pos[1] += this->Step[1];

						if(this->Pos[1] >= this->Shp[1])
						{
							if(this->Shp.count() == 2) this->IsDone = true;

							else
							{
								this->Pos[0] = T(0);
								this->Pos[1] = T(0);
								this->Pos[2] += this->Step[2];

								if(this->Pos[2] >= this->Shp[2])
								{
									if(this->Shp.count() == 3) this->IsDone = true;

									else
									{
										this->Pos[0] = T(0);
										this->Pos[1] = T(0);
										this->Pos[2] = T(0);
										this->Pos[3] += this->Step[3];

										if(this->Pos[3] >= this->Shp[3])
										{
											this->IsDone = true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	};
}