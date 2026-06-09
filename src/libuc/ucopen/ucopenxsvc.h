/* ucopenxsvc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* LIBUC open facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCOPENXSVC_INCLUDE
#define	UCOPENXSVC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	uc_openusvc(cc *,cc *,cc *,int,mode_t,
			con mainv,con mainv,int) noex ;
extern int	uc_openfsvc(cc *,cc *,cc *,int,mode_t,
			con mainv,con mainv,int) noex ;
extern int	uc_openisvc(cc *,cc *,cc *,int,mode_t,
			con mainv,con mainv,int) noex ;

EXTERNC_end


#endif /* UCOPENXSVC_INCLUDE */


