/**
 * @file hbdinit_macros.h
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/18
 * @brief Some useful macros of HBDInit.
 *
 * Copyright (C) 2021 FMSoft <https://www.fmsoft.cn>
 *
 * This file is a part of HBDInit, which contains the examples of my course:
 * _the Best Practices of C Language_.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef HBDINIT_HBDINIT_MACROS_H
#define HBDINIT_HBDINIT_MACROS_H

#ifndef __has_declspec_attribute
#define __has_declspec_attribute(x) 0
#endif

#undef HBDINIT_EXPORT
#if defined(HBDINIT_NO_EXPORT)
#define HBDINIT_EXPORT
#elif defined(WIN32) || defined(_WIN32) || \
    defined(__CC_ARM) || defined(__ARMCC__) || \
    (__has_declspec_attribute(dllimport) && __has_declspec_attribute(dllexport))
#if BUILDING_HBDInit
#define HBDINIT_EXPORT __declspec(dllexport)
#else
#define HBDINIT_EXPORT __declspec(dllimport)
#endif /* BUILDING_HBDInit */
#elif defined(__GNUC__)
#define HBDINIT_EXPORT __attribute__((visibility("default")))
#else /* !defined(HBDINIT_NO_EXPORT) */
#define HBDINIT_EXPORT
#endif /* defined(HBDINIT_NO_EXPORT) */

#if !defined(HBDINIT_INLINE)
#if defined(__cplusplus)
#define HBDINIT_INLINE static inline
#elif defined(__GNUC__)
#define HBDINIT_INLINE static __inline__
#else
#define HBDINIT_INLINE static
#endif
#endif /* !defined(HBDINIT_INLINE) */

#ifndef __has_extension
#define __has_extension(x) 0
#endif

#if __has_extension(enumerator_attributes) && \
    __has_extension(attribute_unavailable_with_message)
#define HBDINIT_C_DEPRECATED(message) __attribute__((deprecated(message)))
#else
#define HBDINIT_C_DEPRECATED(message)
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if __has_attribute(unavailable)
#define HBDINIT_UNAVAILABLE __attribute__((unavailable))
#else
#define HBDINIT_UNAVAILABLE
#endif

#ifdef __cplusplus
#define HBDINIT_EXTERN_C_BEGIN extern "C" {
#define HBDINIT_EXTERN_C_END }
#else
#define HBDINIT_EXTERN_C_BEGIN
#define HBDINIT_EXTERN_C_END
#endif

#define HBDINIT_TABLESIZE(table)    (sizeof(table)/sizeof(table[0]))

#endif /*  HBDINIT_HBDINIT_MACROS_H */
