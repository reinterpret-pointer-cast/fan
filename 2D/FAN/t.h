#pragma once

#define EMPTY
#define _ ,
#define NUL (1 - 1)

#define ARG(...) __VA_ARGS__
#define STR(_m) #_m
#define PB (
#define PE )
#define WITCH_concat_d_(l_m, r_m) l_m##r_m
#define concat_d(_0_m, _1_m) WITCH_concat_d_(_0_m, _1_m)
#define concat2_d concat_d
#define concat3_d(_0_m, _1_m, _2_m) concat2_d(concat2_d(_0_m, _1_m), _2_m)

#if defined(__unix__) || defined(__linux__) || defined(__FreeBSD__) || defined(__MACH__)
	#define WITCH_OS_UNIX
#endif
#if defined(_WIN32) || defined(_WIN64)
	#define WITCH_OS_WINDOWS
#endif

#if defined(WITCH_OS_UNIX)
	#if defined(WITCH_OS_WINDOWS)
		#if defined(__WINE__)
			#undef WITCH_OS_UNIX
		#else
			#error defined(WITCH_OS_UNIX) && defined(WITCH_OS_WINDOWS)
		#endif
	#endif
#endif

#if defined(WITCH_OS_WINDOWS)
	#undef min
	#undef max
	#define WIN32_LEAN_AND_MEAN
	#define WIN64_LEAN_AND_MEAN
	#include <intrin.h>
#endif

/* c99 o nada */
#define WITCH_LANGUAGE_C 1
#ifdef __cplusplus
	#define WITCH_LANGUAGE_CPP __cplusplus
#else
	#define WITCH_LANGUAGE_CPP 0
#endif

#ifndef WITCH_LIBC
#if defined(__has_include)
	#if __has_include(<stdio.h>)
		#define WITCH_LIBC 1
	#else
		#define WITCH_LIBC 0
	#endif
#else
	#error define WITCH_LIBC
#endif
#endif

#ifndef WITCH_LIBCPP
#if defined(__has_include)
	#if __has_include(<iostream>)
		#define WITCH_LIBCPP 1
	#else
		#define WITCH_LIBCPP 0
	#endif
#else
	#error define WITCH_LIBCPP
#endif
#endif

#include <stddef.h>

#define offsetless_d(ptr_m, t_m, d_m) \
	(t_m *)((uint8_t *)(ptr_m) - offsetof(t_m, d_m))

#ifndef WITCH_PAGE_SIZE
	#define WITCH_PAGE_SIZE 4096
#endif

#include <stdint.h>
#include <float.h>

#if UINTPTR_MAX == 0xffffffffffffffff
	#define ptrsize_dr 64
#elif UINTPTR_MAX == 0xffffffff
	#define ptrsize_dr 32
#elif UINTPTR_MAX == 0xffff
	#define ptrsize_dr 16
#elif UINTPTR_MAX == 0xff
	#define ptrsize_dr 8
#else
	#error Failed to find ptrsize_dr
#endif

#ifdef WITCH_LANGUAGE_CPP
	#if WITCH_LANGUAGE_CPP >= 201703L
		#define ifc if constexpr
	#else
		#define ifc if
	#endif
#else
	#define ifc if
#endif

#if WITCH_LANGUAGE_CPP
	#define WITCH_c(_m) _m
#else
	#define WITCH_c(_m) (_m)
#endif
#if WITCH_LANGUAGE_CPP
	#include <vector>
	#define WITCH_a(type_m) std::vector<type_m>
#else
	#define WITCH_a(type_m) (type_m[])
#endif

#if defined(_MSC_VER)
	#define FLEX 1
#elif WITCH_LANGUAGE_CPP
	#define FLEX 0
#else
	#define FLEX
#endif

#define SeekOfint_dr \
	(ptrsize_dr - 1)
#define ArraySize_dr(_m) \
	(sizeof(_m) / sizeof((_m)[0]))

#define Swap_d(type_m, l_m, r_m) \
	{type_m temp_mv = l_m; l_m = r_m; r_m = temp_mv;}

/* integer */
typedef uintptr_t ui_t;
typedef intptr_t si_t;
typedef uintptr_t uint_t;
typedef intptr_t sint_t;
typedef int8_t sint8_t;
typedef int16_t sint16_t;
typedef int32_t sint32_t;
typedef int64_t sint64_t;

typedef uint32_t auint32_t;
typedef sint32_t asint32_t;

/* float */
typedef float f32_t;
typedef double f64_t;
#if ptrsize_dr == 32
	typedef float f_t;
#elif ptrsize_dr == 64
	typedef double f_t;
#endif

#define abs_dr(_m) \
	((_m) < 0 ? -(_m) : (_m))

#define floor_dr (f_t)(si_t)

#define fmod_dr(n_m, m_m) \
	((n_m) - (floor_dr((n_m) / (m_m)) * (m_m)))

typedef struct T_ttcc_t T_ttcc_t;
struct T_ttcc_t{
	uint8_t *ptr;
	ui_t c, p;
	uint8_t (*f)(T_ttcc_t *);
	void *arg;
}; /* type that can call */

typedef struct{
	uint8_t *ptr;
	ui_t uint;
}pui_t;

typedef struct{
	const char *cstr;
	ui_t uint;
}T_csui_t;

static uint8_t CTZ(unsigned int x){
	#if defined(__GNUC__)
		return __builtin_ctz(x);
	#elif defined(_MSC_VER)
		unsigned long ret;
		_BitScanForward(&ret, x);
		return ret;
	#else
		for(uint8_t i = 0; x & 1; x >>= 1)
			return i;
	#endif
}
static uint8_t CTZ32(uint32_t x){
	#if defined(__GNUC__)
		return __builtin_ctzl(x);
	#elif defined(_MSC_VER)
		unsigned long ret;
		_BitScanForward(&ret, x);
		return ret;
	#else
		for(uint8_t i = 0; x & 1; x >>= 1)
			return i;
	#endif
}
static uint8_t CTZ64(uint64_t x){
	#if defined(__GNUC__)
		return __builtin_ctzll(x);
	#elif defined(_MSC_VER)
		unsigned long ret;
		_BitScanForward64(&ret, x);
		return ret;
	#else
		for(uint8_t i = 0; x & 1; x >>= 1)
			return i;
	#endif
}

#if WITCH_LANGUAGE_CPP
	constexpr auto GL_FLOAT_T = GL_FLOAT;
	#include <FAN/t_vector.h>
	#include <FAN/t_da.h>
#endif