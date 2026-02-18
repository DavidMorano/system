/* prsetfname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* set program-root (oriented) file-name */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-11-01, David A­D­ Morano
	Written to have a place for the various KSH initialization subroutines.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	PRSETFNAME_INCLUDE
#define	PRSETFNAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	prsetfname(cc *,char *,cc *,int,int,
			cchar *,cchar *,cchar *) noex ;

EXTERNC_end


#endif /* PRSETFNAME_INCLUDE */


