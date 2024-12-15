/* tmstrs HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* TM structure strings processing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	TMSTRS_INCLUDE
#define	TMSTRS_INCLUDE


#include	<envstandards.h>	/* first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int tmstrsday(cchar *,int) noex ;
extern int tmstrsmonth(cchar *,int) noex ;
extern int tmstrsyear(cchar *,int) noex ;

EXTERNC_end


#endif /* TMSTRS_INCLUDE */


