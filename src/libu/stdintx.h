/* stdintx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* extended integer types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STDINTX_INCLUDE
#define	STDINTX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdint.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */


/* non-constants */

#ifndef	TYPEDEF_INT128
#define	TYPEDEF_INT128
typedef __int128		int128_t ;
#endif /* TYPEDEF_INT128 */

#ifndef	TYPEDEF_SINT128
#define	TYPEDEF_SINT128
typedef signed __int128		sint128_t ;
#endif /* TYPEDEF_UINT128 */

#ifndef	TYPEDEF_UINT128
#define	TYPEDEF_UINT128
typedef unsigned __int128	uint128_t ;
#endif /* TYPEDEF_UINT128 */

#ifndef	TYPEDEF_LONGLONG
#define	TYPEDEF_LONGLONG
typedef __int128		longlong ;
#endif /* TYPEDEF_LONGLONG */

#ifndef	TYPEDEF_SLONGLONG
#define	TYPEDEF_SLONGLONG
typedef signed __int128		slonglong ;
#endif /* TYPEDEF_ULONGLONG */

#ifndef	TYPEDEF_ULONGLONG
#define	TYPEDEF_ULONGLONG
typedef unsigned __int128	ulonglong ;
#endif /* TYPEDEF_ULONGLONG */

/* constants */

#ifndef	TYPEDEF_CLONGLONG
#define	TYPEDEF_CLONGLONG
typedef const longlong		clonglong ;
#endif

#ifndef	TYPEDEF_CSLONGLONG
#define	TYPEDEF_CSLONGLONG
typedef const slonglong		cslonglong ;
#endif

#ifndef	TYPEDEF_CULONGLONG
#define	TYPEDEF_CULONGLONG
typedef const ulonglong		culonglong ;
#endif

/* pointers */

#ifndef	TYPEDEF_LONGLONGP
#define	TYPEDEF_LONGLONGP
typedef __int128 *		longlongp ;
#endif /* TYPEDEF_LONGLONGP */

#ifndef	TYPEDEF_SLONGLONGP
#define	TYPEDEF_SLONGLONGP
typedef signed __int128 *	slonglongp ;
#endif /* TYPEDEF_ULONGLONGP */

#ifndef	TYPEDEF_ULONGLONGP
#define	TYPEDEF_ULONGLONGP
typedef unsigned __int128 *	ulonglongp ;
#endif /* TYPEDEF_ULONGLONGP */

/* pointers-to-constant */

#ifndef	TYPEDEF_CLONGLONGP
#define	TYPEDEF_CLONGLONGP
typedef const longlong *	clonglongp ;
#endif

#ifndef	TYPEDEF_CSLONGLONGP
#define	TYPEDEF_CSLONGLONGP
typedef const slonglong *	cslonglongp ;
#endif

#ifndef	TYPEDEF_CULONGLONGP
#define	TYPEDEF_CULONGLONGP
typedef const ulonglong *	culonglongp ;
#endif


#ifdef	__cplusplus

extern longlong		llabs	(longlong) noex attrconst ;

#endif /* __cplusplus */

#ifndef	TYPEDEF_SINTX
#define	TYPEDEF_SINTX
typedef signed char		sint8 ;
typedef signed short		sint16 ;
typedef signed int		sint32 ;
typedef signed long		sint64 ;
typedef signed __int128		sint128 ;
#endif /* TYPEDEF_SINTX */

#ifndef	TYPEDEF_UINTX
#define	TYPEDEF_UINTX
typedef unsigned char		uint8 ;
typedef unsigned short		uint16 ;
typedef unsigned int		uint32 ;
typedef unsigned long		uint64 ;
typedef unsigned __int128	uint128 ;
#endif /* TYPEDEF_UINTX */

#ifndef	TYPEDEF_CSINTX
#define	TYPEDEF_CSINTX
typedef const signed char	csint8 ;
typedef const signed short	csint16 ;
typedef const signed int	csint32 ;
typedef const signed long	csint64 ;
typedef const signed __int128	csint128 ;
#endif /* TYPEDEF_CSINTX */

#ifndef	TYPEDEF_CUINTX
#define	TYPEDEF_CUINTX
typedef const unsigned char	cuint8 ;
typedef const unsigned short	cuint16 ;
typedef const unsigned int	cuint32 ;
typedef const unsigned long	cuint64 ;
typedef const unsigned __int128	cuint128 ;
#endif /* TYPEDEF_CUINTX */


#endif /* STDINTX_INCLUDE */


