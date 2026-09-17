/* usys_sysctl HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrive the Maximum-Message-Length from the operating system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sysctl

	Description:
	This subroutine retrives the Maximum-Message-Length from
	the operating system.

	Synosis:
	int sysctl() noex

	Arguments:

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#ifndef	USYSSYSCTL_INCLUDE
#define	USYSSYSCTL_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD |size_t| */
#include	<stdlib.h>		/* CSTD */
#include	<stdint.h>		/* CSTD |uint32_t| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


/* USYSDARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
/******************************************************************************/


/* Apple-Darwin has this subroutine. */


/******************************************************************************/
#else /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/******************************************************************************/

#ifndef	SYSCTL_DECLARATION
#define	SYSCTL_DECLARATION
EXTERNC_begin
extern int sysctl(int *,uint32_t,void *,size_t *,void *,size_t) noex ;
extern int sysctlbyname(cchar *,void *,size_t *,void *,size_t) noex ;
EXTERNC_end
#endif /* SYSCTL_DECLARATION */

/******************************************************************************/
#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */


#endif /* __cplusplus */
#endif /* USYSSYSCTL_INCLUDE */


