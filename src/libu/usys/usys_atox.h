/* usys_atox HEADER */
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

  	Library:
	usys_atox

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSATOX_INCLUDE
#define	USYSATOX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern sint	atosi(cchar *) noex ;
extern uint	atoui(cchar *) noex ;
EXTERNC_end

EXTERNC_begin
extern slong	atosl(cchar *) noex ;
extern ulong	atoul(cchar *) noex ;
EXTERNC_end

#ifdef	__cplusplus
extern sint	atosi(cchar *,int) noex ;
extern uint	atoui(cchar *,int) noex ;
extern slong	atosl(cchar *,int) noex ;
extern ulong	atoul(cchar *,int) noex ;
#endif /* __cplusplus */


#endif /* USYSATOX_INCLUDE */


