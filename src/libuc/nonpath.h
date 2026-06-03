/* nonpath HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* determine if the given string represents a non-path filename */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NONPATH_INCLUDE
#define	NONPATH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum nonpaths {
    	nonpath_reg,			/* regular: absolute or relative */
    	nonpath_user,
    	nonpath_var,
	nonpath_dialer,
	nonpath_fsvc,
	nonpath_usvc,
	nonpath_overlast
} ; /* end enum (nonpaths) */


EXTERNC_begin

extern int	nonpath(cchar *,int) noex ;

EXTERNC_end

extern cchar	nonpathchs[nonpath_overlast + 1] ;


#endif /* NONPATH_INCLUDE */


