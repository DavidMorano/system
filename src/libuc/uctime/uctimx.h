/* uctimx HEADER interval timer) */
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
	uctimx{x}

	Description:
	This is a facility to create process timers.  Note that unlike
	the normal POSIX® real-time interfaces, this facility only
	supports time resolution down to one microsecond (as opposed to
	one nanosecond in the regular stuff).

*******************************************************************************/

#ifndef	UCTIMX_INCLUDE
#define	UCTIMX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX system types */
#include	<sys/time.h>		/* POSIX <- interval timers are here */
#include	<time.h>		/* CSTD i-timer types */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<psem.h>		/* LIBUC POSIX® semaphore */


#define	UCTIMXNOTE	struct uctimxnote_head


EXTERNC_begin

typedef int (*uctimx_f)(void *objp,int timid,int arg) noex ;

EXTERNC_end

struct uctimxnote_head {
	uctimx_f	notf ;		/* notify function (C-linkage) */
	void		*objp ;		/* object pointer (function argument) */
	psem		*psemp ;	/* POSIX® Semaphore pointer */
	int		arg ;		/* function argument */
} ; /* end struct (uctimxnote_head) */

#ifdef	__cplusplus
struct uctimxnote : uctimxnote_head {
    	int load	(void *,psem *,uctimx_f,int) noex ;
} ; /* end struct (uctimxnote) */
#else /* __cplusplus */
typedef	UCTIMXNOTE	uctimxnote ;
#endif /* __cplusplus */

EXTERNC_begin

extern int uctimxnote_load	(uctimxnote *,void *,psem *,uctimx_f,int) noex ;

extern int uc_timxcreate	(con uctimxnote *) noex ;
extern int uc_timxdestroy	(int) noex ;
extern int uc_timxset		(int,time_t *,time_t) noex ;
extern int uc_timxget		(int,time_t *) noex ;
extern int uc_timxover		(int) noex ;

EXTERNC_end


#endif /* UCTIMX_INCLUDE */


