/* tmstrs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* calculate an ordinal (an integer number) from a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TMSTRS_INCLUDE
#define	TMSTRS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int tmstrsday	(cchar *,int) noex ;
extern int tmstrsmonth	(cchar *,int) noex ;
extern int tmstrsyear	(cchar *,int) noex ;

EXTERNC_end


#endif /* TMSTRS_INCLUDE */


