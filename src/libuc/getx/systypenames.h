/* systypenames HEADER (System-Type-Numbers) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* system type numbers (and associated strings) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSTYPENAMES_INCLUDE
#define	SYSTYPENAMES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


enum systypenames {
	systypename_sysv,
	systypename_darwin,
	systypename_linux,
	systypename_overlast
} ; /* end enum (systypenames) */

extern ccharp		systypename[systypename_overlast + 1] ;


#endif /* SYSTYPENAMES_INCLUDE */


