/* mkchar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a character (8-bit) out of an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
	= 2018-11-27, David A-D- Morano
	I created an inline version for the C language. We will
	likely do something for C++ at some other time.

	= 2023-10-23, David A-D- Morano
	At some point I added an inline (here) for the C++ language.

*/

/* Copyright � 1998,2018,23023 David A-D- Morano.  All rights reserved. */

#ifndef	MKCHAR_INCLUDE
#define	MKCHAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>		/* |UCHAR_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#ifndef	MKCHAR
#define	MKCHAR(ch)	(ch & UCHAR_MAX)
#endif /* MKCHAR */


#ifdef	MODULE

extern int mkchar(int ch) noex {
	return (ch & UCHAR_MAX) ;
}

#else /* MODULE */
#if	defined(__cplusplus)

constexpr inline int mkchar(int ch) noex {
	return (ch & UCHAR_MAX) ;
}

#else /* __cplusplus */
#define	MKCHAR_STATICINLINE

static inline int mkchar(int ch) noex {
	return (ch & UCHAR_MAX) ;
}

#endif /* _cplusplus */
#endif /* MODULE */


#endif /* MKCHAR_INCLUDE */


