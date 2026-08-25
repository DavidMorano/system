/* uctimeconv HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* virtual per-process timer management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uctimeconv

	Description:
	This is a facility to create process timers.

*******************************************************************************/

#ifndef	UCTIMECONV_INCLUDE
#define	UCTIMECONV_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<sys/time.h>		/* POSIX® i-timer types */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	uc_timelocal	(custime *,mut TM *) noex ;
extern int	uc_timegm	(custime *,mut TM *) noex ;
extern int	uc_timex	(custime *,mut TM *,int) noex ;
extern int	uc_mktime	(mut TM *,mut time_t *) noex ;

EXTERNC_end


#endif /* UCTIMECONV_INCLUDE */


