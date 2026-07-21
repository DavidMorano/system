/* gettmpdir HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the name of the TMPDIR (however one can get it) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETTMPDIR_INCLUDE
#define	GETTMPDIR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern cchar	*gettmpdirx(con mainv) noex ;

EXTERNC_end

#ifdef	__cplusplus

local inline cchar *gettmpdir(con mainv envv = nullptr) noex {
	return gettmpdirx(envv) ;
}

#else /* __cplusplus */

local inline cchar *gettmpdir(con mainv envv) noex {
	return gettmpdirx(envv) ;
}

#endif /* __cplusplus */


#endif /* GETTMPDIR_INCLUDE */


