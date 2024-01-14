/* snaddw HEADER */
/* lang=C20 */

/* make string version of some flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Ths object is used in the creation of flags strings.

******************************************************************************/

#ifndef	SNADDW_INCLUDE
#define	SNADDW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


EXTERNC_begin

extern int snaddw(char *,int,int,cchar *,int) noex ;

EXTERNC_end


#endif /* SNADDW_INCLUDE */


