/* sfnext HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* various string-find operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SFNEXT_INCLUDE
#define	SFNEXT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int sfnext	(cchar *,int,cchar **) noex ;
extern int sfnextchr	(cchar *,int,int,cchar **) noex ;
extern int sfnextbrk	(cchar *,int,cchar *,cchar **) noex ;
extern int sfnextterm	(cchar *,int,cchar *,cchar **) noex ;
extern int sfnextqtok	(cchar *,int,cchar **) noex ;
local inline int nextfield(cchar *sp,int sl,cchar **rpp) noex {
	return sfnext(sp,sl,rpp) ;
}

EXTERNC_end


#endif /* SFNEXT_INCLUDE */


