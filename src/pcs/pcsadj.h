/* pcs-adj(unct) */
/* lang=C20 */

/* PCS adjunct subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSADJ_INCLUDE
#define	PCSADJ_INCLUDE


EXTERNC_begin

extern int	pcsadj_begin(PROGINFO *) noex ;
extern int	pcsadj_end(PROGINFO *) noex ;
extern int	pcsadj_req(PROGINFO *,int) noex ;

EXTERNC_end


#endif /* PCSADJ_INCLUDE */


