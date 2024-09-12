/* mfs-adj(unct) HEADER */
/* lang=C++20 */

/* MFS adjunct subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-08-10, David A­D­ Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright © 2005,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	MFSADJ_INCLUDE
#define	MFSADJ_INCLUDE


#include	<envstandards.h>
#include	<poller.h>
#include	"defs.h"


EXTERNC_begin

extern int	mfsadj_begin(PROGINFO *) noex ;
extern int	mfsadj_end(PROGINFO *) noex ;
extern int	mfsadj_poll(PROGINFO *,POLLER *,int,int) noex ;
extern int	mfsadj_register(PROGINFO *,POLLER *) noex ;

EXTERNC_end


#endif /* MFSADJ_INCLUDE */


