/* uctim HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX� library-3c */
/* virtual per-process timer management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uctim{x}

	Description:
	This is a facility to create process timers.

*******************************************************************************/

#ifndef	UCTIM_INCLUDE
#define	UCTIM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/time.h>		/* i-timer types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<callback.h>


EXTERNC_begin

extern int uc_timcreate(callback *) noex ;
extern int uc_timdestroy(int) noex ;
extern int uc_timset(int,CITIMERVAL *,ITIMERVAL *) noex ;
extern int uc_timget(int,ITIMERVAL *) noex ;
extern int uc_timover(int) noex ;

EXTERNC_end


#endif /* UCTIM_INCLUDE */


