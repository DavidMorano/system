/* openshm HEADER */
/* lang=C20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENSHM_INCLUDE
#define	OPENSHM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


EXTERNC_begin

extern int openshmtmpx(mode_t) noex ;
extern int openshmtmp(char *,int,mode_t) noex ;

EXTERNC_end


#endif /* OPENSHM_INCLUDE */


