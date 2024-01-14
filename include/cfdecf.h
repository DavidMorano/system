/* cfdecf HEADER */
/* lang=C20 */

/* convert a decimal digit string to its binary floating value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFDECF_INCLUDE
#define	CFDECF_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


EXTERNC_begin

extern int cfdecf(cchar *,int,double *) noex ;

EXTERNC_end


#endif /* CFDECF_INCLUDE */


