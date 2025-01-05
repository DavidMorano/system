/* cfdecf HEADER */
/* encoding=ISO8859-1 */
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


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<stdintx.h>


EXTERNC_begin

extern int	cfdecf(cchar *,int,double *) noex ;

/* historical (from about 1995 or earlier) */
static inline int cfdouble(cchar *sp,int sl,double *rp) noex {
    	return cfdecf(sp,sl,rp) ;
}

EXTERNC_end


#endif /* CFDECF_INCLUDE */


