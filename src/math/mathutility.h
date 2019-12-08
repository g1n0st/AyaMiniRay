#ifndef AYA_MATHUTILITY_H
#define AYA_MATHUTILITY_H

#include "..\core\config.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

#if defined(AYA_DEBUG)
#include <assert.h>
#else
#define assert
#endif

#ifdef _WIN32
#include <intrin.h>
#endif

#define SIMD_EPSILON FLT_EPSILON

#if defined(AYA_SCALAR_OUTPUT_APPROXIMATION)
#define AYA_SCALAR_OUTPUT(x) (abs(x) < SIMD_EPSILON ? 0 : (x))
#else
#define AYA_SCALAR_OUTPUT(x) (x)
#endif


#define v1_5 (_mm_set_ps(1.5f, 1.5f, 1.5f, 1.5f))
#define v_0_5 (_mm_set_ps(-0.5f, -0.5f, -0.5f, -0.5f))
#define v0_5 (_mm_set_ps(0.5f, 0.5f, 0.5f, 0.5f))

template<class T>
inline T Min(const T &a, const T &b) {
	return a < b ? a : b;
}
template<class T>
inline T Max(const T &a, const T &b) {
	return a > b ? a : b;
}
template<class T>
inline void SetMax(T &a, const T &b) {
	if (b > a) a = b;
}
template<class T>
inline void SetMin(T &a, const T &b) {
	if (b < a) a = b;
}

inline float Radian(const float &deg) {
	return (float)(M_PI / 180.f) * deg;
}
inline float Degree(const float &rad) {
	return (float)(180.f / M_PI) * rad;
}

template<class T>
inline T Lerp(const T &t, const T &a, const T &b) {
	return a + t * (b - a);
}
template<class T>
inline T Clamp(const T &t, const T  &low, const T &high) {
	if (t < low) return low;
	if (t > high) return high;
	return t;
}

inline float RSqrt(const float &x)
{
#if defined(AYA_USE_SIMD) && defined(AYA_USE_SQRT_APPROXIMATION)
	const __m128 a = _mm_set_ss(x);
	const __m128 r = _mm_rsqrt_ps(a);
	const __m128 c = _mm_add_ps(_mm_mul_ps(v1_5, r),
		_mm_mul_ps(_mm_mul_ps(_mm_mul_ps(a, v_0_5), r), _mm_mul_ps(r, r)));
	return _mm_cvtss_f32(c);
#else
	return 1.f / sqrtf(x);
#endif
}
inline float Sqrt(const float &x) {
#if defined(AYA_USE_SIMD) && defined(AYA_USE_SQRT_APPROXIMATION)
	return 1.f / RSqrt(x);
#else
	return sqrtf(x);
#endif
}

#endif