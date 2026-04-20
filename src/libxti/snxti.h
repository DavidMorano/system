/* snxti HEADER (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* additional UNIX® limits support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	snxti

	Description:
	This is the nehanced (cleaned up) API for the
	X/Open Transport Interface.

*******************************************************************************/

#ifndef	SNXTI_INCLUDE
#define	SNXTI_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int snxtilook	(char *,int,int) noex ;
extern int snxtierr	(char *,int,int) noex ;

EXTERNC_end


#endif /* SNXTI_INCLUDE */


