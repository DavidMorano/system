/* strwcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-comparison variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRWCMP_INCLUDE
#define	STRWCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int strwbasecmp(cchar *,cchar *,int) noex ;
extern int strwcasecmp(cchar *,cchar *,int) noex ;
extern int strwfoldcmp(cchar *,cchar *,int) noex ;

local inline int strwcmp(cchar *bs,cchar *sp,int sl) noex {
	return strwbasecmp(bs,sp,sl) ;
}

EXTERNC_end


#endif /* STRWCMP_INCLUDE */


