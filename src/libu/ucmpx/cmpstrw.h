/* cmpstrw HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-comparison variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRW_INCLUDE
#define	CMPSTRW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern int cmpstrwbc(cchar *,cchar *,int) noex ;	/* base */
extern int cmpstrwic(cchar *,cchar *,int) noex ;	/* insensitive */
extern int cmpstrwfc(cchar *,cchar *,int) noex ;	/* fold */
EXTERNC_end

EXTERNC_begin
local inline int cmpstrw(cchar *bs,cchar *sp,int sl) noex {
	return cmpstrwbc(bs,sp,sl) ;
} /* end */
EXTERNC_end


#endif /* CMPSTRW_INCLUDE */


