/* clanguage_cast HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */

#define	CF_SATURATING		0	/* what name to use for saturating? */

/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	Make better sense out of these C++ casts.

	Notes:
	1. The required header file for the saturating cast
	(|std::satrurating_cast|) is 'cnumeric'.
	2. Which name to use for saturating cast?  For now, the
	maintainer decides using the pre-processor define
	|CF_SATURATING|.

*******************************************************************************/

#ifndef	CLANGUAGECAST_INCLUDE
#define	CLANGUAGECAST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cnumeric>		/* C++ only */


#ifndef	CF_SATURATING
#define	CF_SATURATING		0	/* what name to use for saturating? */
#endif


#ifdef	__cplusplus
#ifndef	cast_static
#define	cast_static		static_cast
#define	cast_dynamic		dynamic_cast
#define	cast_const		const_cast
#define	cast_reinterpret	reinterpret_cast
#define	cast_bit		bit_cast
#define	cast_clock		clock_cast
#define	cast_duration		duration_cast
#define	cast_any		any_cast
#endif /* cast_static */
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	saturate
#if	defined(__cpp_lib_saturation_arithmetic) && CF_SATURATING
#define	saturate		std::saturating_cast
#else
#define	saturate		std::saturate_cast
#endif /* __cpp_lib_saturation_arithmetic */
#endif /* saturate */
#endif /* __cplusplus */

/* NOTE: this is GCC-specific (since GCC-14) and not C++26 */
/* NOTE: C++26 says the proper cast is |std:saturating_cast(3c++)| */
#ifdef	__cplusplus
#ifndef	cast_saturate
#if	defined(__cpp_lib_saturation_arithmetic) && CF_SATURATING
#define	cast_saturate		saturating_cast
#define	cast_saturate		saturating_cast
#define	cast_sat		saturating_cast
#else
#define	cast_saturate		saturate_cast
#define	cast_saturate		saturate_cast
#define	cast_sat		saturate_cast
#endif /* __cpp_lib_saturation_arithmetic */
#endif /* cast_saturate */
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	CAST_R
#define	CAST_R			cast_reinterpret
#endif
#endif /* __cplusplus */


#endif /* CLANGUAGECAST_INCLUDE */


