#pragma once

#include <cstring>

#ifndef __empty_struct
#define __empty_struct __empty_struct
struct __empty_struct {

};
#endif

#ifndef __is_type_same
#define __is_type_same std::is_same_v
#endif

#ifndef __MemoryCopy
#define __MemoryCopy(src, dst, size) memcpy(dst, src, size)
#endif

#ifndef __MemorySet
#define __MemorySet(src, dst, size) memset(dst, src, size)
#endif

#ifndef __MemoryMove
#define __MemoryMove(dst, src, size) memmove(dst, src, size)
#endif

#ifndef __abort
#define __abort() fan::throw_error_impl()
#endif

#ifndef __cta
#define __cta(x) static_assert(x)
#endif

#ifndef __abort
#define __abort() fan::throw_error_impl()
#endif