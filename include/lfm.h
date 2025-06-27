/* lfm HEADER */
/* charset=ISO8859-1 */
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
#include	<sys/types.h>		/* system types */
#include	<unistd.h>
#include	<usystem.h>


/* object defines */
#define	LFM_MAGIC		0x8a7b7c6d
#define	LFM			struct lfm_head
#define	LFM_IN			struct lfm_information
#define	LFM_CH			struct lfm_checkdata
/* lock-file types */
#define	LFM_TRECORD		0		/* record lock */
#define	LFM_TCREATE		1		/* old create file 0444 */
#define	LFM_TEXCLUSIVE		2		/* modern exclusive open */
#define	LFM_TOVERLAST		3
/* time-outs */
#define	LFM_TOLOCK		(5*60)
#define	LFM_TOMINCHECK		3
#define	LFM_TOMINSTAT		(60 + 3)
#define	LFM_CHBUFLEN		400


struct lfm_information {
	ino_t		ino ;
	dev_t		dev ;
	int		tocheck ;	/* check interval */
} ;

struct lfm_checkdata {
	cchar		*nodename ;	/* lock node name */
	cchar		*username ;	/* lock user name */
	cchar		*banner ;	/* banner */
	pid_t		pid ;		/* lock PID */
	int		status ;	/* status */
	char		buf[LFM_CHBUFLEN + 1] ;
} ;

struct lfm_head {
	cchar		*lfname ;	/* file name (processed) */
	ino_t		ino ;
	off_t		odate ;		/* offset to date */
	off_t		orewind ;	/* offset to start of write area */
	off_t		owrite ;	/* offset past last write */
	time_t		ti_check ;	/* last check on lock */
	time_t		ti_stat ;	/* last 'stat(2)' on lock */
	dev_t		dev ;
	pid_t		pid ;		/* our PID */
	pid_t		pid_lock ;	/* old times sake */
	uint		magic ;
	int		lfd ;
	int		type ;		/* lock type */
	int		tolock ;	/* lock timeout */
	int		tocheck ;	/* check interval */
} ;

typedef LFM		lfm ;
typedef	LFM_IN		lfm_in ;
typedef	LFM_CH		lfm_ch ;

EXTERNC_begin

extern int	lfm_start(lfm *,cc *,int,int,lfm_ch *,cc *,cc *,cc *) noex ;
extern int	lfm_setpoll(lfm *,int) noex ;
extern int	lfm_check(lfm *,lfm_ch *,time_t) noex ;
extern int	lfm_printf(lfm *,cchar *,...) noex ;
extern int	lfm_flush(lfm *) noex ;
extern int	lfm_getinfo(lfm *,lfm_in *) noex ;
extern int	lfm_getpid(lfm *,pid_t *) noex ;
extern int	lfm_rewind(lfm *) noex ;
extern int	lfm_finish(lfm *) noex ;

EXTERNC_end


#endif /* LFM_INCLUDE */


