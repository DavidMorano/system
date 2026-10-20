/* usys_hasx HEADER */
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
	usys_hasx

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSHASX_INCLUDE
#define	USYSHASX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifndef	SUBROUTINE_HASXX
#define	SUBROUTINE_HASXX
EXTERNC_begin
local inline bool hasmore(cchar *sp,int sl) noex {
    	return (sl && *sp) ;
} /* end */
local inline bool hasnone(cchar *sp,int sl) noex {
    	return ((sl == 0) || (*sp == '\0')) ;
} /* end */
EXTERNC_end
#endif /* SUBROUTINE_HASXX */


#endif /* USYSHASX_INCLUDE */


