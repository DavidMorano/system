/* mkxdisp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* qualify an X display specification */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKXDISP_INCLUDE
#define	MKXDISP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>		/* |UCHAR_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	mkxdisp(char *,int,cc *,cc *,cc *) noex ;

EXTERNC_end


#endif /* MKXDISP_INCLUDE */


