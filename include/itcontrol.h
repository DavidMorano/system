/* itcontrol HEADER */
/* encoding=ISO8859-1 */
/* lang=C99 */

/* interval-timer control (block) */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	itcontrol

	Description:
	This file provides the definition of the ITCONTROL object.
	This is the control-block for specifying how an interval-timer
	expiration is handled.

*******************************************************************************/

#ifndef	ITCONTROL_INCLUDE
#define	ITCONTROL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<psem.h>


#define	ITCONTROL	struct itcontrol_head


EXTERNC_begin

typedef int (*itcontrol_f)(void *objp,int timid,int arg) noex ;

EXTERNC_end

struct itcontrol_head {
	void		*objp ;		/* object pointer */
	psem		*psemp ;	/* POSIX® Semaphore pointer */
	itcontrol_f	memf ;		/* function subroutine (C-linkage) */
	ITIMERVAL	it ;		/* i-timer-value */
	int		id ;		/* timer-ID */
	int		arg ;		/* function argument */
} ;

typedef ITCONTROL	itcontrol ;

EXTERNC_begin

extern int itcontrol_load(itcontrol *,void *,psem *,itcontrol_f,int) noex ;

EXTERNC_end


#endif /* ITCONTROL_INCLUDE */


