/* strsub HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRSUB_INCLUDE
#define	STRSUB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char	*strbasesub(cchar *,cchar *) noex ;
extern char	*strcasesub(cchar *,cchar *) noex ;
extern char	*strfoldsub(cchar *,cchar *) noex ;

static inline char *strsub(cchar *sp,cchar *ss) noex {
    	return strbasesub(sp,ss) ;
}

EXTERNC_end


#endif /* STRSUB_INCLUDE */


