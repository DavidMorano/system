/* usys_getauid HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_getauid

	Description:
	This is part of the famous "Basic-Security-Module" (BSM) that
	was first created by Sub Mircosystems and then ported to 
	Apple-Darwin by McAffe (yes the anti-virus company).

*******************************************************************************/

#ifndef	USYSGETAUID_INCLUDE
#define	USYSGETAUID_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

/* GETAUID begin */
#if	(!defined(SYSHAS_GETAUID)) || (SYSHAS_GETAUID == 0)


#ifndef	TYPEDEF_AUIDT
#define	TYPEDEF_AUIDT
typedef uid_t		au_id_t ;
#endif	/* TYPEDEF_AUIDT */

#ifndef	SUBROUTINE_GETAUID
#define	SUBROUTINE_GETAUID
EXTERNC_begin
/* 0=not-found, 1=found */
extern int	getauid(au_id_t *) noex ;
EXTERNC_end
#endif /* SUBROUTINE_GETAUID */


#endif /* (!defined(SYSHAS_GETAUID)) || (SYSHAS_GETAUID == 0) */
/* GETAUID end */


#endif /* USYSGETAUID_INCLUDE */


