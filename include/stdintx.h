/* stdintx HEADER */
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
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdint.h>


/* non-constants */

#ifndef	TYPEDEF_INT128
#define	TYPEDEF_INT128
typedef __int128		int128_t ;
#endif /* TYPEDEF_INT128 */

#ifndef	TYPEDEF_UINT128
#define	TYPEDEF_UINT128
typedef unsigned __int128	uint128_t ;
#endif /* TYPEDEF_UINT128 */

#ifndef	TYPEDEF_LONGLONG
#define	TYPEDEF_LONGLONG
typedef __int128		longlong ;
#endif /* TYPEDEF_LONGLONG */

#ifndef	TYPEDEF_ULONGLONG
#define	TYPEDEF_ULONGLONG
typedef unsigned __int128	ulonglong ;
#endif /* TYPEDEF_ULONGLONG */

/* constants */

#ifndef	TYPEDEF_CLONGLONG
#define	TYPEDEF_CLONGLONG
typedef const longlong		clonglong ;
#endif

#ifndef	TYPEDEF_CULONGLONG
#define	TYPEDEF_CULONGLONG
typedef const ulonglong		culonglong ;
#endif


#endif /* STDINTX_INCLUDE */


