#pragma once
#ifndef __Common_h__
#define __Common_h__

#include <memory>
#include <limits>

namespace Rattler {

#define Feimos_Ray_Bound_ErrorBound

static constexpr float Pi = 3.14159265358979323846;
static constexpr float InvPi = 0.31830988618379067154;
static constexpr float Inv2Pi = 0.15915494309189533577;
static constexpr float Inv4Pi = 0.07957747154594766788;
static constexpr float PiOver2 = 1.57079632679489661923;
static constexpr float PiOver4 = 0.78539816339744830961;
static constexpr float Sqrt2 = 1.41421356237309504880;
inline constexpr float Radians(float deg) { return (Pi / 180) * deg; }

#define MachineEpsilon (std::numeric_limits<float>::epsilon() * 0.5)
inline float gamma(int n) {
	return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

// 功能暂时不定义
#define CHECK_NE(a,b) 
#define DCHECK(a)
#define CHECK_GT(a,b)
#define CHECK(a)

#define MaxFloat std::numeric_limits<float>::max()
#define Infinity std::numeric_limits<float>::infinity()

static constexpr float ShadowEpsilon = 0.0001f;

template <typename T>
class Vector2;
template <typename T>
class Vector3;
typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;
template <typename T>
class Point3;
template <typename T>
class Point2;
typedef Point2<float> Point2f;
typedef Point2<int> Point2i;
typedef Point3<float> Point3f;
typedef Point3<int> Point3i;
template <typename T>
class Normal3;
template <typename T>
class Bounds2;
template <typename T>
class Bounds3;
typedef Bounds2<float> Bounds2f;
typedef Bounds2<int> Bounds2i;
typedef Bounds3<float> Bounds3f;
typedef Bounds3<int> Bounds3i;

template <int nSpectrumSamples>
class CoefficientSpectrum;
class RGBSpectrum;
typedef RGBSpectrum Spectrum;

struct Matrix4x4;
class Transform;
class Ray;

class Shape;
struct Interaction;
class SurfaceInteraction;
class MediumInteraction;

class Primitive;
class GeometricPrimitive;
class Aggregate;

class Camera;
class ProjectiveCamera;
class PerspectiveCamera;
class OrthographicCamera;

class Sampler;
class PixelSampler;
class GlobalSampler;
class HaltonSampler;
class ClockRandSampler;

class Sampler;
class Scene;
class BxDF;
class BSDF;
class Material;
class Light;
class AreaLight;
class VisibilityTester;

class Distribution1D;
class Distribution2D;
class LightDistribution;

class PhaseFunction;
class HenyeyGreenstein;
class Medium;
class MediumInterface;
class HomogeneousMedium;
class GridDensityMedium;

template <typename T, int logBlockSize = 2>
class BlockedArray;

template <typename T>
inline bool isNaN(const T x) {
	return std::isnan(x);
}
template <>
inline bool isNaN(const int x) {
	return false;
}
inline uint32_t FloatToBits(float f) {
	uint32_t ui;
	memcpy(&ui, &f, sizeof(float));
	return ui;
}

inline float BitsToFloat(uint32_t ui) {
	float f;
	memcpy(&f, &ui, sizeof(uint32_t));
	return f;
}
inline float NextFloatUp(float v) {
	// Handle infinity and negative zero for _NextFloatUp()_
	if (std::isinf(v) && v > 0.) return v;
	if (v == -0.f) v = 0.f;

	// Advance _v_ to next higher float
	uint32_t ui = FloatToBits(v);
	if (v >= 0)
		++ui;
	else
		--ui;
	return BitsToFloat(ui);
}
inline float NextFloatDown(float v) {
	// Handle infinity and positive zero for _NextFloatDown()_
	if (std::isinf(v) && v < 0.) return v;
	if (v == 0.f) v = -0.f;
	uint32_t ui = FloatToBits(v);
	if (v > 0)
		--ui;
	else
		++ui;
	return BitsToFloat(ui);
}

template <typename T>
inline constexpr bool IsPowerOf2(T v) {
	return v && !(v & (v - 1));
}

inline int32_t RoundUpPow2(int32_t v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return v + 1;
}

inline int64_t RoundUpPow2(int64_t v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v |= v >> 32;
	return v + 1;
}

inline float Log2(float x) {
	const float invLog2 = 1.442695040888963387004650940071;
	return std::log(x) * invLog2;
}

#define PBRT_IS_MSVC

inline int Log2Int(uint32_t v) {
#if defined(PBRT_IS_MSVC)
	unsigned long lz = 0;
	if (_BitScanReverse(&lz, v)) return lz;
	return 0;
#else
	return 31 - __builtin_clz(v);
#endif
}

inline int Log2Int(int32_t v) { return Log2Int((uint32_t)v); }

inline int Log2Int(uint64_t v) {
#if defined(PBRT_IS_MSVC)
	unsigned long lz = 0;
#if defined(_WIN64)
	_BitScanReverse64(&lz, v);
#else
	if (_BitScanReverse(&lz, v >> 32))
		lz += 32;
	else
		_BitScanReverse(&lz, v & 0xffffffff);
#endif // _WIN64
	return lz;
#else  // PBRT_IS_MSVC
	return 63 - __builtin_clzll(v);
#endif
}

inline int Log2Int(int64_t v) { return Log2Int((uint64_t)v); }

template <typename Predicate>
int FindInterval(int size, const Predicate &pred) {
	int first = 0, len = size;
	while (len > 0) {
		int half = len >> 1, middle = first + half;
		// Bisect range based on value of _pred_ at _middle_
		if (pred(middle)) {
			first = middle + 1;
			len -= half + 1;
		}
		else
			len = half;
	}
	return Clamp(first - 1, 0, size - 2);
}

template <typename T, typename U, typename V>
inline T Clamp(T val, U low, V high) {
	if (val < low)
		return low;
	else if (val > high)
		return high;
	else
		return val;
}
inline float Lerp(float t, float v1, float v2) { return (1 - t) * v1 + t * v2; }

template <typename T>
inline T Mod(T a, T b) {
	T result = a - (a / b) * b;
	return (T)((result < 0) ? result + b : result);
}
template <>
inline float Mod(float a, float b) {
	return std::fmod(a, b);
}

inline float Erf(float x) {
	// constants
	float a1 = 0.254829592f;
	float a2 = -0.284496736f;
	float a3 = 1.421413741f;
	float a4 = -1.453152027f;
	float a5 = 1.061405429f;
	float p = 0.3275911f;

	// Save the sign of x
	int sign = 1;
	if (x < 0) sign = -1;
	x = std::abs(x);

	// A&S formula 7.1.26
	float t = 1 / (1 + p * x);
	float y =
		1 -
		(((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

	return sign * y;
}

inline float ErfInv(float x) {
	float w, p;
	x = Clamp(x, -.99999f, .99999f);
	w = -std::log((1 - x) * (1 + x));
	if (w < 5) {
		w = w - 2.5f;
		p = 2.81022636e-08f;
		p = 3.43273939e-07f + p * w;
		p = -3.5233877e-06f + p * w;
		p = -4.39150654e-06f + p * w;
		p = 0.00021858087f + p * w;
		p = -0.00125372503f + p * w;
		p = -0.00417768164f + p * w;
		p = 0.246640727f + p * w;
		p = 1.50140941f + p * w;
	}
	else {
		w = std::sqrt(w) - 3;
		p = -0.000200214257f;
		p = 0.000100950558f + p * w;
		p = 0.00134934322f + p * w;
		p = -0.00367342844f + p * w;
		p = 0.00573950773f + p * w;
		p = -0.0076224613f + p * w;
		p = 0.00943887047f + p * w;
		p = 1.00167406f + p * w;
		p = 2.83297682f + p * w;
	}
	return p * x;
}

inline float InverseGammaCorrect(float value) {
	if (value <= 0.04045f) return value * 1.f / 12.92f;
	return std::pow((value + 0.055f) * 1.f / 1.055f, (float)2.4f);
}


}



#endif


