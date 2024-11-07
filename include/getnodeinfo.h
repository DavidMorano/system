/* getnodeinfo HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* UNIX® Protocol-Family support */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This code was originally written.

	= 1996-05-22, David A­D­ Morano
	This subroutine was enhanced to get the local node-name if
	one if not supplied, using 'getnodename(3dam)'.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains some support for the UNIX® Address-Familt
	(AF) related operations.

*******************************************************************************/

#ifndef	GETNODEINFO_INCLUDE
#define	GETNODEINFO_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>


EXTERNC_begin

extern int getnodeinfo(cchar *,char *,char *,vecstr *,cc *) noex ;

EXTERNC_end


#endif /* GETNODEINFO_INCLUDE */


