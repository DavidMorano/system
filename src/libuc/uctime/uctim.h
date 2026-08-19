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
#include	<sys/types.h>		/* POSIX system types */
#include	<sys/time.h>		/* POSIX <- interval timers are here */
#include	<time.h>		/* CSTD i-timer types */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<psem.h>		/* LIBUC POSIX® semaphore */


#define	UCTIMNOTE	struct uctimnote_head


EXTERNC_begin

typedef int (*uctim_f)(void *objp,int timid,int arg) noex ;

EXTERNC_end

struct uctimnote_head {
	uctim_f		notf ;		/* notify function (C-linkage) */
	void		*objp ;		/* object pointer (function argument) */
	psem		*psemp ;	/* POSIX® Semaphore pointer */
	int		arg ;		/* function argument */
} ; /* end struct (uctim_note) */

#ifdef	__cplusplus
struct uctimeote : uctimenote_head {
    	int load	(void *,psem *,uctim_f,int) noex ;
} ; /* end struct (uctimnote) */
#endif /* __cplusplus */
typedef	UCTIMNOTE	uctimnote ;
#endif /* __cplusplus */

EXTERNC_begin

extern int uctimnote_load	(uctimnote *,void *,psem *,uctim_f,int) noex ;

extern int uc_timcreate		(con uctimnote *) noex ;
extern int uc_timdestroy	(int) noex ;
extern int uc_timset		(int,CITIMERVAL *,ITIMERVAL *) noex ;
extern int uc_timget		(int,ITIMERVAL *) noex ;
extern int uc_timover		(int) noex ;

EXTERNC_end


#endif /* UCTIM_INCLUDE */


