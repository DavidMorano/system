/* sfxname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* various string-find operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SFXNAME_INCLUDE
#define	SFXNAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>


EXTERNC_begin

extern int sfbasename(cchar *,int,cchar **) noex ;
extern int sfdirname(cchar *,int,cchar **) noex ;
extern int sfprogname(cchar *,int,cchar **) noex ;
extern int sfrootname(cchar *,int,cchar **) noex ;

EXTERNC_end


#endif /* SFXNAME_INCLUDE */


