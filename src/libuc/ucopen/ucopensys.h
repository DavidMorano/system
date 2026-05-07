/* ucopensys HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* additional UNIX® support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ucopen

	Description:
	Additional (or supplemental) support for UNIX® limits.

*******************************************************************************/

#ifndef	UCOPENSYS_INCLUDE
#define	UCOPENSYS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int uc_opensys(cchar *,int,mode_t,con mainv,int,int) noex ;

EXTERNC_end


#endif /* UCOPENSYS_INCLUDE */


