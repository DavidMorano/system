/* qualdisplay HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* qualify an X display specification */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	QUALDISPLAY_INCLUDE
#define	QUALDISPLAY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>		/* |UCHAR_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int qualdisplay(cc *,cc *,cc *,char *,int) noex ;

EXTERNC_end


#endif /* QUALDISPLAY_INCLUDE */


