/* usys_shadow HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Apple Darwin operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_shadow

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

	Notes:
	1. The whole "shadow" password concept was created by
	Julianne (Julie) Frances Haugh II (in 1987 originally for
	SCO Xenix®).

	2. There have been at least four (4) different implementations
	of the reentrant shadow-password API since the inception
	of the concept in 1987.  These were: 
	a. the original by Julie Haugh (above) in SCO Zenix® in 1987
	b. the one implemented in UNIX® System V Release 3.2 in 1988
	c. the one implemented by GNU for the standard­C library (1992)
	d. the one implemented in Solaris® (2.6) in 1997

	The version implemented in UNIX® System V Realase 4 (in 1988)
	was either the same or similar to the one in System V Rlease 3.2
	or the one eventually implemented by GNU in 1992.

*******************************************************************************/

#ifndef	USYSSHADOW_INCLUDE
#define	USYSSHADOW_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#ifndef	SPWD
#define	SPWD		struct spwd
#endif
#ifndef	CSPWD
#define	CSPWD		const struct spwd
#endif


#if	defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0)
/******************************************************************************/


#include	<shadow.h>		/* standard (Solaris®) header */


/******************************************************************************/
#else /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */
/******************************************************************************/


#ifndef	STRUCT_SPWD
#define	STRUCT_SPWD
struct spwd {
    char *sp_namp;     /* Login name */
    char *sp_pwdp;     /* Encrypted password */
    long  sp_lstchg;   /* Date of last change
                          (measured in days since
                          1970-01-01 00:00:00 +0000 (UTC)) */
    long  sp_min;      /* Min # of days between changes */
    long  sp_max;      /* Max # of days between changes */
    long  sp_warn;     /* # of days before password expires
                          to warn user to change it */
    long  sp_inact;    /* # of days after password expires
                          until account is disabled */
    long  sp_expire;   /* Date when account expires
                          (measured in days since
                          1970-01-01 00:00:00 +0000 (UTC)) */
    unsigned long sp_flag;  /* Reserved */
} ; /* end struct (spwd) */
#endif /* STRUCT_SPWD */

#ifndef	SUBROUTINE_GETSP
#define	SUBROUTINE_GETSP
#ifdef	__cplusplus /* (C++ only) */

namespace solaris {
    extern CSPWD *getspent_r(SPWD *,char *,int) noex ;
    extern CSPWD *getspnam_r(cchar *,SPWD *,char *,int) noex ;
}

namespace gnu {
    extern errno_t getspent_r(SPWD *,char *,size_t,SPWD **) noex ;
    extern errno_t getspnam_r(cchar *,SPWD *,char *,size_t,SPWD **) noex ;
}

#endif /* __cplusplus (C++ only) */
#endif /* SUBROUTINE_GETSP */


/******************************************************************************/
#endif /* defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0) */


#endif /* USYSSHADOW_INCLUDE */


