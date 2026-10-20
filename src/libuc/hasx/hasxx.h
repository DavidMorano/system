/* hasxx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASXX_INCLUDE
#define	HASXX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
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


#endif /* HASXX_INCLUDE */


