#ifndef AYA_CORE_CONFIG_H
#define AYA_CORE_CONFIG_H

#define AYA_DEBUG
#include <iostream>

#ifdef _WIN32
#if  defined(__GNUC__)	// it should handle both MINGW and CYGWIN
#define SIMD_FORCE_INLINE        __inline__ __attribute__((always_inline))
#define ATTRIBUTE_ALIGNED16(a)   a __attribute__((aligned(16)))
#define ATTRIBUTE_ALIGNED64(a)   a __attribute__((aligned(64)))
#define ATTRIBUTE_ALIGNED128(a)  a __attribute__((aligned(128)))
#elif ( defined(_MSC_VER) && _MSC_VER < 1300 )
#define SIMD_FORCE_INLINE inline
#define ATTRIBUTE_ALIGNED16(a) a
#define ATTRIBUTE_ALIGNED64(a) a
#define ATTRIBUTE_ALIGNED128(a) a
#elif defined(_M_ARM)
#define SIMD_FORCE_INLINE inline
#define ATTRIBUTE_ALIGNED16(a) __declspec() a
#define ATTRIBUTE_ALIGNED64(a) __declspec() a
#define ATTRIBUTE_ALIGNED128(a) __declspec () a
#else//__MINGW32__
//#define BT_HAS_ALIGNED_ALLOCATOR
#pragma warning(disable : 4324) // disable padding warning
//			#pragma warning(disable:4530) // Disable the exception disable but used in MSCV Stl warning.
#pragma warning(disable:4996) //Turn off warnings about deprecated C routines
//			#pragma warning(disable:4786) // Disable the "debug name too long" warning

#define SIMD_FORCE_INLINE inline
#define ATTRIBUTE_ALIGNED16(a) __declspec(align(16)) a
#define ATTRIBUTE_ALIGNED64(a) __declspec(align(64)) a
#define ATTRIBUTE_ALIGNED128(a) __declspec (align(128)) a
#endif
#endif

#if (defined(_WIN32) && (_MSC_VER) && _MSC_VER >= 1400)
#if _MSC_VER > 1400
/**@brief use simd acceleration */
#define AYA_USE_SIMD
#endif
#endif

// math library only
/**@brief use sqrt approximation*/
#define AYA_USE_SQRT_APPROXIMATION
/**@brief use debug output scalar approximation*/
#define AYA_SCALAR_OUTPUT_APPROXIMATION

// core/spectrum
#define AYA_SAMPLED_SPECTRUM

// core/memory
#define AYA_L1_CACHE_LINE_SIZE 64
#endif