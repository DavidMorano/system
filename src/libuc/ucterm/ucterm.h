/* ucterm HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* object to handle UNIX terminal stuff */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCTERM_INCLUDE
#define	UCTERM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX system types |uid_t| */
#include	<termios.h>		/* POSIX */
#include	<time.h>		/* CSTD |time_t| */
#include	<limits.h>		/* CSTD |UCHAR_MAX| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<charq.h>		/* LIBUC */
#include	<termcmd.h>		/* LIBUC */


#define	UCTERM		struct ucterm_head
#define	UCTERM_FL	struct ucterm_flags
#define	UCTERM_LD	struct ucterm_loader
#define	UCTERM_PR	struct ucterm_prompter
#define	UCTERM_MAGIC	0x33442281
#define	UCTERM_TSZ	((UCHAR_MAX + 1) / szof(char))


enum uctermcmds {
	uctermcmd_noop,
	uctermcmd_getuid,
	uctermcmd_getsid,
	uctermcmd_getmode,
	uctermcmd_setmode,
	uctermcmd_setrterms,
	uctermcmd_reestablish,
	uctermcmd_getmesg,
	uctermcmd_setmesg,
	uctermcmd_getbiff,
	uctermcmd_setbiff,
	uctermcmd_getlines,
	uctermcmd_setlines,
	uctermcmd_getpgrp,
	uctermcmd_setpgrp,
	uctermcmd_getpop,
	uctermcmd_setpop,
	uctermcmd_overlast
} ; /* end enum */

struct ucterm_flags {
	uint		cntl_o:1 ;	/* control-O */
	uint		cntl_c:1 ;	/* control-C */
	uint		cntl_y:1 ;	/* control-Y */
	uint		cntl_z:1 ;	/* control-Z */
	uint		dle:1 ;		/* data-link-escape */
	uint		rw:1 ;		/* ?? */
	uint		suspend:1 ;	/* output suspended */
	uint		read:1 ;	/* read in progress */
	uint		nosig:1 ;	/* no-signal-generation mode */
	uint		nosigecho:1 ;	/* no echo for signals */
	uint		noctty:1 ;	/* the terminal is not controlling */
	uint		noflow:1 ;	/* no output flow control */
} ; /* end struct */

/* prompt-output before input */
struct ucterm_prompter {
	cchar		*pbuf ;
	int		plen ;
} ; /* end struct */

/* pre-loading the input buffer */
struct ucterm_loader {
	cchar		*lbuf ;
	int		llen ;
} ; /* end struct */

struct ucterm_head {
	TERMIOS		ts_old ;
	TERMIOS		ts_new ;
	charq		taq ;
	charq		ecq ;
	time_t		ti_start ;
	uid_t		uid ;
	UCTERM_FL	fl ;
	uint		magval ;
	int		fd ;
	int		loopcount ;
	int		timeout ;	/* timeout timer counter */
	int		mode ;
	int		ch_read ;
	int		ch_write ;
	int		status ;
	uchar		rterms[UCTERM_TSZ] ;
} ; /* end struct (ucterm_head) */

typedef UCTERM		ucterm ;
typedef	UCTERM_FL	ucterm_fl ;
typedef	UCTERM_LD	ucterm_ld ;
typedef	UCTERM_PR	ucterm_pr ;

EXTERNC_begin

extern int ucterm_start		(ucterm *,int) noex ;
extern int ucterm_control	(ucterm *,int,...) noex ;
extern int ucterm_status	(ucterm *,int,...) noex ;
extern int ucterm_read		(ucterm *,char *,int) noex ;
extern int ucterm_reade		(ucterm *,char *,int,int,int,
					ucterm_pr *,ucterm_ld *) noex ;
extern int ucterm_readcmd	(ucterm *,termcmd *,int,int) noex ;
extern int ucterm_write		(ucterm *,cchar *,int) noex ;
extern int ucterm_suspend	(ucterm *) noex ;
extern int ucterm_resume	(ucterm *) noex ;
extern int ucterm_restore	(ucterm *) noex ;
extern int ucterm_ensure	(ucterm *) noex ;
extern int ucterm_getmesg	(ucterm *) noex ;
extern int ucterm_getbiff	(ucterm *) noex ;
extern int ucterm_getpop	(ucterm *) noex ;
extern int ucterm_setpop	(ucterm *,int) noex ;
extern int ucterm_finish	(ucterm *) noex ;

EXTERNC_end


#endif /* UCTERM_INCLUDE */


