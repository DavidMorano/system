/* usys_strtox HEADER */
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
	usys_strtox

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSSTRTOX_INCLUDE
#define	USYSSTRTOX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern sint	strtosi(cchar *,char **,int) noex ;
extern uint	strtoui(cchar *,char **,int) noex ;
EXTERNC_end

EXTERNC_begin
extern slong	strtosl(cchar *,char **,int) noex ;
EXTERNC_end

EXTERNC_begin
local inline int strtoi(cchar *s,char **endpp,int b) noex {
	return strtosi(s,endpp,b) ;
} /* end subroutine (strtoi) */
EXTERNC_end


#endif /* USYSSTRTOX_INCLUDE */


