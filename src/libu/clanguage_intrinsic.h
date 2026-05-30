/* clanguage_intrinsic HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGEINTRINSIC_INCLUDE
#define	CLANGUAGEINTRINSIC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	va_begin
#define	va_begin(ap,arg)	va_start((ap),(arg))
#endif
#ifndef	va_get
#define	va_get(ap,atype)	va_arg(ap,atype)
#endif

#ifndef	nelements
#define	nelements(n)		((int) (sizeof(n) / sizeof((n)[0])))
#endif

#ifndef	nelem
#define	nelem(n)		((int) (sizeof(n) / sizeof((n)[0])))
#endif

#ifdef	__cplusplus
#ifndef	npack
#define	npack(args)		int(sizeof...(args))
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	szof
#define	szof(T)			int(sizeof(T))
#endif
#endif /* __cplusplus */


#endif /* CLANGUAGEINTRINSIC_INCLUDE */


