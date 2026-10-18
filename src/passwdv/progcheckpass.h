/* progcheckpass HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* check a password for validity */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGCHECKPASS_INCLUDE
#define	PROGCHECKPASS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	<proginfo.hh>

#ifndef	PI
#define	PI	proginfo
#endif


EXTERNC_begin

extern int progcheckpass(PI *,cc *,cc *,int) noex ;

EXTERNC_end


#endif /* PROGCHECKPASS_INCLUDE */


