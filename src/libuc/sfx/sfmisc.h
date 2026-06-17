/* sfmisc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* various string-find operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SFMISC_INCLUDE
#define	SFMISC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int sfbaselib	(cchar *,int,cchar **) noex ;
extern int sfbracketval	(cchar *,int,cchar **) noex ;
extern int sfcookkey	(cchar *,int,cchar **) noex ;
extern int sfdequote	(cchar *,int,cchar **) noex ;
extern int sflast	(cchar *,int,int,cchar **) noex ;
extern int sfnamecomp	(cchar *,int,cchar **) noex ;

extern int sfprogroot	(cchar *,int,cchar **) noex ;
extern int sfshrink	(cchar *,int,cchar **) noex ;
extern int sfskipwhite	(cchar *,int,cchar **) noex ;
extern int sfsubstance	(cchar *,int,cchar **) noex ;
extern int sfthing	(cchar *,int,cchar *,cchar **) noex ;
extern int sfwhitedot	(cchar *,int,cchar **) noex ;
extern int sfword	(cchar *,int,cchar **) noex ;
extern int sfsign	(cchar *,int,cchar **,bool *) noex ;
extern int sfcenter	(cchar *,int,cchar *,cchar **) noex ;

EXTERNC_end


#endif /* SFMISC_INCLUDE */


