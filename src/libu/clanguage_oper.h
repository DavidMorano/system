/* clanguage_oper HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* C-language defines */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGEOPER_INCLUDE
#define	CLANGUAGEOPER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifdef	__cplusplus
#ifndef	neg
#define	neg			-
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	bitnot
#define	bitnot			~
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	bitxor
#define	bitxor			xor
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	bitand_eq
#define	bitand_eq		&=
#define	bitor_eq		|=
#define	bitxor_eq		^=
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	logand
#define	logand			and
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	logor
#define	logor			or
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	lognot
#define	lognot			not
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	logand_eq
#define	logand_eq		&&=	/* <- does not exist! */
#define	logor_eq		||=	/* <- does not exist! */
#define	logxor_eq		^^=	/* <- does not exist! */
#endif
#endif /* __cplusplus */


#endif /* CLANGUAGEOPER_INCLUDE */


