/* clanguage_attr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGEATTR_INCLUDE
#define	CLANGUAGEATTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifdef	__cplusplus
#ifndef	attrweak
#define	attrweak		__attribute__((weak))
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	attrweakref
#define	attrweakref(target)	__attribute__((weakref(target)))
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	attrconst
#define	attrconst		__attribute__((const))
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	attrpure
#define	attrpure		__attribute__((pure))
#endif
#endif /* __cplusplus */


#endif /* CLANGUAGEATTR_INCLUDE */


