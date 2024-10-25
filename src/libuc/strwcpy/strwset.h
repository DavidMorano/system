/* strwset HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRWSET_INCLUDE
#define	STRWSET_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char	*strwset(char *dp,int ch,int n) noex ;

static inline char *strwblanks(char *dp,int n) noex {
    return strwset(dp,' ',n) ;
}

EXTERNC_end


#endif /* STRWSET_INCLUDE */


