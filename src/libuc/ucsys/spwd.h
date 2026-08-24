/* spwd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* Apple-Darwin types */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	sqwd

	Description:
	This file contains the UNIX system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the MacOS operating system.

	Notes:
	1. The whole "shadow" password concept was created by
	Julianne (Julie) Frances Haugh II (in 1987 originally for
	SCO Xenix®).

*******************************************************************************/

#ifndef	SPWD_INCLUDE
#define	SPWD_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* POSIX® |size_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#ifndef	SPWD
#define	SPWD		struct spwd
#endif

#ifndef	CSPWD
#define	CSPWD		const struct spwd
#endif


#if	defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0)
/******************************************************************************/


#include	<shadow.h>		/* standard Solaris® header */


/******************************************************************************/
#else /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */
#ifdef	__cplusplus /* C++ only */
/******************************************************************************/


#ifndef	STRUCT_SPWD
#define	STRUCT_SPWD
struct spwd {
	char		*sp_namp ;
	char		*sp_pwdp ;
	unsigned long	sp_flag ;
	long		sp_lstchg ;
	long		sp_min ;
	long		sp_max ;
	long		sp_warn ;
	long		sp_inact ;
	long		sp_expire ;
} ; /* end struct (spwd) */
#endif /* STRUCT_SPWD */

namespace gnu {
    extern void	setspent() noex ;
    extern void	endspent() noex ;
    extern SPWD	*getspent() noex ;
    extern SPWD	*getspnam(cchar *) noex ;
    extern errno_t getspent_r(SPWD *,char *,size_t,CSPWD **) noex ;
    extern errno_t getspnam_r(cc *,SPWD *,char *,size_t,CSPWD **) noex ;
} /* end namespace (gnu) */


/******************************************************************************/
#endif /* __cplusplus (C++ only) */
#endif /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */


#endif /* SPWD_INCLUDE */


