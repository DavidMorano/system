/* uctim HEADER */
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
	uctim{x}

	Description:
	This is a facility to create process timers.  Note that unlike
	the normal POSIX® real-time interfaces, this facility only
	supports time resolution down to one microsecond (as opposed to
	one nanosecond in the regular stuff).

*******************************************************************************/

#ifndef	UCTIM_INCLUDE
#define	UCTIM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/time.h>		/* <- interval timers are here */
#include	<time.h>		/* i-timer types */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<psen.h>		/* POSIX® semaphore */


#define	UCTIM_ENT	struct uctim_entry


EXTERNC_begin

typedef int (*uctim_f)(void *objp,int timid,int arg) noex ;

EXTERNC_end

struct uctim_entry {
	void		*objp ;		/* fcuntion argument (object pointer) */
	psem		*psemp ;	/* POSIX® Semaphore pointer */
	uctim_f		notf ;		/* notify function (C-linkage) */
	ITIMERVAL	it ;		/* i-timer-value */
	int		id ;		/* timer-ID */
	int		arg ;		/* function argument */
} ; /* end struct (uctim) */

typedef	UCTIM_ENT		uctim_ent ;

EXTERNC_begin

extern int uctim_load		(uctim_ent *,void *,psem *,uctim_f,int) noex ;

extern int uc_timcreate		(uctim_ent *) noex ;
extern int uc_timdestroy	(int) noex ;
extern int uc_timset		(int,CITIMERVAL *,ITIMERVAL *) noex ;
extern int uc_timget		(int,ITIMERVAL *) noex ;
extern int uc_timover		(int) noex ;

EXTERNC_end


#endif /* UCTIM_INCLUDE */


