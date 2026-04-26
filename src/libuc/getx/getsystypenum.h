/* getsystypenum HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the system type number and c-strings of other system information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETSYSTYPENUM_INCLUDE
#define	GETSYSTYPENUM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


enum systypenames {
	systypename_sysv,
	systypename_darwin,
	systypename_linux,
	systypename_overlast
} ; /* end enum (systypenames) */

EXTERNC_begin

extern int	getsystypenum(char *,char *,cc *,cc *) noex ;

EXTERNC_end


#endif /* GETSYSTYPENUM_INCLUDE */


