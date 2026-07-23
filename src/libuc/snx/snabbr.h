/* snabbr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* counted c-string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNABBR_INCLUDE
#define	SNABBR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int snabbrerr	(char *,int,int) noex ;
extern int snabbrsig	(char *,int,int) noex ;
extern int snabbrname	(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* SNABBR_INCLUDE */


