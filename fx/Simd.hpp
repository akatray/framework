// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pragma.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Imports.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <fx/Types.hpp>
#include <fx/Allocator.hpp>
#include <immintrin.h>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Definitions.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Select one.
//#define FX_SIMD_NOCASH
//#define FX_SIMD_SSE
#define FX_SIMD_AVX

#define FX_SIMD_FUSEDOPS

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Framework - Python can go fuck itself.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Performance measurements proves that turning optimizer on, render this pointless.
// Runs somewhat fast in debug mode though.
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace fx::simd
{
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// No cash renaming.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	#ifdef FX_SIMD_NOCASH
		#define ALIGMENT_PS 8
		constexpr auto SIMD_ALIGNMENT = u64(8);
		#define UNIT_PS 1
	#endif

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// SSE renaming.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	#ifdef FX_SIMD_SSE
		#define ALIGMENT_PS 16
		constexpr auto SIMD_ALIGNMENT = u64(16);
		#define UNIT_PS 4
		
		#define LOAD_PS _mm_load_ps
		#define STORE_PS _mm_store_ps

		#define SETZERO_PS _mm_setzero_ps
		#define MUL_PS _mm_mul_ps
		#define DIV_PS _mm_div_ps
		#define ADD_PS _mm_add_ps
		#define SUB_PS _mm_sub_ps
	#endif

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// AVX renaming.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	#ifdef FX_SIMD_AVX
		#define ALIGMENT_PS 32
		constexpr auto SIMD_ALIGNMENT = u64(32);
		#define UNIT_PS 8
		
		#define LOAD_PS _mm256_load_ps
		#define STORE_PS _mm256_store_ps

		#define SETZERO_PS _mm256_setzero_ps
		#define MUL_PS _mm256_mul_ps
		#define DIV_PS _mm256_div_ps
		#define ADD_PS _mm256_add_ps
		#define SUB_PS _mm256_sub_ps
	#endif

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Fused operations.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	#ifdef FX_SIMD_FUSEDOPS
		#ifdef FX_SIMD_SSE
			#define FMADD_PS _mm_fmadd_ps
			#define FMSUB_PS _mm_fmsub_ps
		#endif
		
		#ifdef FX_SIMD_AVX
			#define FMADD_PS _mm256_fmadd_ps
			#define FMSUB_PS _mm256_fmsub_ps
		#endif
	#endif

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Aligned allocation/deallocation.
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class T> auto allocAligned ( const u64 _Size, const u64 _Alignment = ALIGMENT_PS ) -> T* { return reinterpret_cast<T*>(_mm_malloc(_Size * sizeof(T), _Alignment)); }
	template<class T> auto freeAligned ( T* _Ptr ) -> void { _mm_free(_Ptr); }




	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Aligned allocator.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template <class T> struct AllocAlign
	{
		u64 Alignment;
		using value_type = T;

		AllocAlign ( void ) noexcept { this->Alignment = SIMD_ALIGNMENT; }
		AllocAlign ( u64 _Alignment ) noexcept { this->Alignment = _Alignment; }
		template <class U> AllocAlign ( AllocAlign<U> const& ) noexcept {}

		auto allocate ( std::size_t _Count ) -> value_type* { static_cast<value_type*>(_mm_malloc(_Count * sizeof(value_type), this->Aligment)); }
		auto deallocate ( value_type* _Pointer, std::size_t ) noexcept -> void  { _mm_free(_Pointer); }
	};

	template <class T, class U> bool operator==(AllocAlign<T> const&, AllocAlign<U> const&) noexcept{return true;}
	template <class T, class U> bool operator!=(AllocAlign<T> const& x, AllocAlign<U> const& y) noexcept {return !(x == y);}




	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// That rare case.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	class AllocatorAligned : public Allocator
	{
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Members.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		u64 Alignment;
		public:
		
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Default constructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		AllocatorAligned ( void ) : Alignment(32) {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Explicit constructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		AllocatorAligned ( const u64 _Aligment ) : Alignment(_Aligment) {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Destructor.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		~AllocatorAligned ( void ) final {}

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Allocate memory. Size in bytes.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto alloc ( const u64 _Size ) -> ptr final { return _mm_malloc(_Size, this->Alignment); }

		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Free memory.
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		auto free ( void* _Pointer ) -> void final { _mm_free(_Pointer); }
	};

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Default aligned memory provider.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	auto AllocSimd = AllocatorAligned(ALIGMENT_PS);

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Increase size to fit whole number of simd units.
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline auto adjustSize ( const u64 _Size ) -> u64 { return u64(UNIT_PS * std::ceil(r32(_Size) / UNIT_PS)); }

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  Out += Vec0[] * Vec1[]
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline auto mulVecByVecSum ( const u64 _VecSize, const r32* _Vec0, const r32* _Vec1 ) -> r32
	{
		// Reference version.
		#ifdef FX_SIMD_NOCASH
			auto Sum = r32(0.0f);
			for(auto f = u64(0); f < _VecSize; ++f) Sum += _Vec0[f] * _Vec1[f];
			return Sum;

		// Vectorized version.
		#else
			auto Fragments = u64(_VecSize / UNIT_PS);

			auto Vec0 = SETZERO_PS();
			auto Vec1 = SETZERO_PS();
			auto Sum = SETZERO_PS();

			for(auto f = u64(0); f < Fragments; ++f)
			{
				Vec0 = LOAD_PS(_Vec0);
				Vec1 = LOAD_PS(_Vec1);

				#ifdef FX_SIMD_FUSEDOPS
					Sum = FMADD_PS(Vec0, Vec1, Sum);
				#else
					Vec0 = MUL_PS(Vec0, Vec1);
					Sum = ADD_PS(Sum, Vec0);
				#endif

				_Vec0 += UNIT_PS;
				_Vec1 += UNIT_PS;
			}

			auto Unpack = reinterpret_cast<r32*>(&Sum);

			#ifdef FX_SIMD_SSE
				return (Unpack[0] + Unpack[1] + Unpack[2] + Unpack[3]);
			#endif

			#ifdef FX_SIMD_AVX
				return (Unpack[0] + Unpack[1] + Unpack[2] + Unpack[3] + Unpack[4] + Unpack[5] + Unpack[6] + Unpack[7]);
			#endif
		#endif
	}

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  Out += Vec0[] * Const
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline auto mulVecByConstSum ( u64 _VecSize,  r32* _Vec0, r32 _Const ) -> r32
	{
		// Reference version.
		#ifdef FX_SIMD_NOCASH
			auto Sum = r32(0.0f);
			for(auto f = u64(0); f < _VecSize; ++f) Sum += _Vec0[f] * _Const;
			return Sum;

		// Vectorized version.
		#else
			auto Fragments = u64(_VecSize / UNIT_PS);

			auto Vec0 = SETZERO_PS();
			auto Sum = SETZERO_PS();

			#ifdef FX_SIMD_SSE
				auto Const = _mm_set_ps(_Const, _Const, _Const, _Const);
			#endif

			#ifdef FX_SIMD_AVX
				auto Const = _mm256_set_ps(_Const, _Const, _Const, _Const, _Const, _Const, _Const, _Const);
			#endif

			for(auto f = u64(0); f < Fragments; ++f)
			{
				Vec0 = LOAD_PS(_Vec0);
				
				#ifdef FX_SIMD_FUSEDOPS
					Sum = FMADD_PS(Vec0, Const, Sum);
				#else
					
					Vec0 = MUL_PS(Vec0, Const);
					Sum = ADD_PS(Sum, Vec0);
				#endif

				_Vec0 += UNIT_PS;
			}

			auto Unpack = reinterpret_cast<r32*>(&Sum);

			#ifdef FX_SIMD_SSE
				return (Unpack[0] + Unpack[1] + Unpack[2] + Unpack[3]);
			#endif

			#ifdef FX_SIMD_AVX
				return (Unpack[0] + Unpack[1] + Unpack[2] + Unpack[3] + Unpack[4] + Unpack[5] + Unpack[6] + Unpack[7]);
			#endif
		#endif
	}

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  OutVec[] += Vec0[] * Const
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline auto mulVecByConstAddToOut ( const u64 _VecSize, r32* _VecOut, const r32* _Vec0, const r32 _Const ) -> void
	{
		// Reference version.
		#ifdef FX_SIMD_NOCASH
			for(auto f = u64(0); f < _VecSize; ++f) _VecOut[f] += _Vec0[f] * _Const;
		
		// Vectorized version.
		#else
			auto Fragments = u64(std::floor(r32(_VecSize) / UNIT_PS));
			auto Partial = _VecSize - (Fragments * UNIT_PS);

			auto VecOut = SETZERO_PS();
			auto Vec0 = SETZERO_PS();

			#ifdef FX_SIMD_SSE
				auto Const = _mm_set_ps(_Const, _Const, _Const, _Const);
			#endif

			#ifdef FX_SIMD_AVX
				auto Const = _mm256_set_ps(_Const, _Const, _Const, _Const, _Const, _Const, _Const, _Const);
			#endif

			for(auto f = u64(0); f < Fragments; ++f)
			{
				VecOut = LOAD_PS(_VecOut);
				Vec0 = LOAD_PS(_Vec0);

				#ifdef FX_SIMD_FUSEDOPS
					VecOut = FMADD_PS(Vec0, Const, VecOut);
				#else
					Vec0 = MUL_PS(Vec0, Const);
					VecOut = ADD_PS(VecOut, Vec0);
				#endif

				STORE_PS(_VecOut, VecOut);

				_VecOut += UNIT_PS;
				_Vec0 += UNIT_PS;
			}

			if(Partial != 0) for(auto f = u64(0); f < Partial; ++f) _VecOut[f] += _Vec0[f] * _Const;

		#endif
	}

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//  OutVec[] -= Vec0[] * Const
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	inline auto mulVecByConstSubFromOut ( const u64 _VecSize, r32* _VecOut, const r32* _Vec0, const r32 _Const) -> void
	{
		// Reference version.
		#ifdef FX_SIMD_NOCASH
			for(auto f = u64(0); f < _VecSize; ++f) _VecOut[f] -= _Vec0[f] * _Const;
		
		// Vectorized version.
		#else
			auto Fragments = u64(std::floor(r32(_VecSize) / UNIT_PS));
			auto Partial = _VecSize - (Fragments * UNIT_PS);

			auto VecOut = SETZERO_PS();
			auto Vec0 = SETZERO_PS();

			#ifdef FX_SIMD_SSE
				auto Const = _mm_set_ps(_Const, _Const, _Const, _Const);
			#endif

			#ifdef FX_SIMD_AVX
				auto Const = _mm256_set_ps(_Const, _Const, _Const, _Const, _Const, _Const, _Const, _Const);
			#endif

			for(auto f = u64(0); f < Fragments; ++f)
			{
				VecOut = LOAD_PS(_VecOut);
				Vec0 = LOAD_PS(_Vec0);

				Vec0 = MUL_PS(Vec0, Const);
				VecOut = SUB_PS(VecOut, Vec0);

				STORE_PS(_VecOut, VecOut);

				_VecOut += UNIT_PS;
				_Vec0 += UNIT_PS;
			}

			if(Partial != 0) for(auto f = u64(0); f < Partial; ++f) _VecOut[f] -= _Vec0[f] * _Const;

		#endif
	}
}