/* lfm HEADER */
/* lang=C20 */

/* manage file-lock operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LFM_INCLUDE
#define	LFM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<usystem.h>		/* for 'ino_t' */
#include	<localmisc.h>		/* for types */


/* object defines */
#define	LFM_MAGIC	0x8a7b7c6d
#define	LFM		struct lfm_head
#define	LFM_INFO	struct lfm_i
#define	LFM_CHECK	struct lfm_c

/*  lock-file types */
#define	LFM_TRECORD		0		/* record lock */
#define	LFM_TCREATE		1		/* old create file 0444 */
#define	LFM_TEXCLUSIVE		2		/* modern exclusive open */
#define	LFM_TOVERLAST		3

#define	LFM_TOLOCK		(5*60)
#define	LFM_TOMINCHECK		3
#define	LFM_TOMINSTAT		(60 + 3)

#define	LFM_CHECKBUFLEN		400


struct lfm_i {
	dev_t		dev ;
	ino_t		ino ;
	int		tocheck ;	/* check interval */
} ;

struct lfm_c {
	cchar		*nodename ;	/* lock node name */
	cchar		*username ;	/* lock user name */
	cchar		*banner ;	/* banner */
	pid_t		pid ;		/* lock PID */
	int		stat ;		/* status */
	char		buf[LFM_CHECKBUFLEN + 1] ;
} ;

struct lfm_head {
	cchar		*lfname ;	/* file name (processed) */
	ino_t		ino ;
	dev_t		dev ;
	off_t		odate ;		/* offset to date */
	off_t		orewind ;	/* offset to start of write area */
	off_t		owrite ;	/* offset past last write */
	time_t		ti_check ;	/* last check on lock */
	time_t		ti_stat ;	/* last 'stat(2)' on lock */
	pid_t		pid ;		/* our PID */
	pid_t		pid_lock ;	/* old times sake */
	uint		magic ;
	int		lfd ;
	int		type ;		/* lock type */
	int		tolock ;	/* lock timeout */
	int		tocheck ;	/* check interval */
} ;

typedef LFM		lfm ;

EXTERNC_begin

extern int	lfm_start(LFM *,cchar *,int,int,LFM_CHECK *,
			cchar *,cchar *,cchar *) noex ;
extern int	lfm_setpoll(LFM *,int) noex ;
extern int	lfm_check(LFM *,LFM_CHECK *,time_t) noex ;
extern int	lfm_printf(LFM *,cchar *,...) noex ;
extern int	lfm_flush(LFM *) noex ;
extern int	lfm_getinfo(LFM *,LFM_INFO *) noex ;
extern int	lfm_getpid(LFM *,pid_t *) noex ;
extern int	lfm_finish(LFM *) noex ;

EXTERNC_end


#endif /* LFM_INCLUDE */


