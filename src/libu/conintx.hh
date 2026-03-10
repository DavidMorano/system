/* conintx HEADER (Concepts-Integer-X) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extended integer types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONINTX_INCLUDE
#define	CONINTX_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdint.h>
#include	<type_traits>
#include	<concepts>
#include	<clanguage.h>
#include	<stdintx.h>


template<typename T>
struct is_stdintx : std::false_type { } ;

template<>
struct is_stdintx<signed char> : std::true_type { } ;

template<>
struct is_stdintx<signed short> : std::true_type { } ;

template<>
struct is_stdintx<signed int> : std::true_type { } ;

template<>
struct is_stdintx<signed long> : std::true_type { } ;

template<>
struct is_stdintx<signed longlong> : std::true_type { } ;

template<>
struct is_stdintx<unsigned char> : std::true_type { } ;

template<>
struct is_stdintx<unsigned short> : std::true_type { } ;

template<>
struct is_stdintx<unsigned int> : std::true_type { } ;

template<>
struct is_stdintx<unsigned long> : std::true_type { } ;

template<>
struct is_stdintx<unsigned longlong> : std::true_type { } ;

template<typename T>
inline constexpr bool is_stdintx_v = is_stdintx<T>::value ;

template<typename T>
concept stdintx = is_stdintx<T>::value ;


#endif /* __cplusplus */
#endif /* CONINTX_INCLUDE */


