/* prmkfname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* set program-root (oriented) file-name */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-11-01, David A­D­ Morano
	Written to have a place for the various KSH initialization subroutines.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	PRMKFNAME_INCLUDE
#define	PRMKFNAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int prmkfname(cc *,char *,cc *,int,int,cc *,cc *,cc *) noex ;

static inline int prsetfname(cc *pr,char *fn,cc *ep,int el,int f,cc *dn,
		cc *n,cc *s) noex {
	return prmkfname(pr,fn,ep,el,f,dn,n,s) ;
}

EXTERNC_end


#endif /* PRMKFNAME_INCLUDE	*/


