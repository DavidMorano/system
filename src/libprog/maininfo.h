/* maininfo HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* support object for early program initialization */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-05-05, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	MAININFO_INCLUDE
#define	MAININFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<pthread.h>		/* POSIX® */
#include	<signal.h>		/* CSTD */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<sighand.h>		/* LIBUC */


#define	MAININFO	struct maininfo
#define	MAININFO_FL	struct maininfo_flags


struct maininfo_flags {
	uint		progdash:1 ;	/* leading dash on program-name */
	uint		utilout:1 ;	/* utility is out running */
} ; /* end struct */

struct maininfo {
	sighand		sh ;
	vecstr		stores ;
	stack_t		astack ;
	cchar		*progdname ;
	cchar		*progename ;
	cchar		*progname ;
	cchar		*srchname ;
	cchar		*symname ;
	void		*mdata ;
	MAININFO_FL	have, f, changed, finval ;
	MAININFO_FL	open ;
	sigset_t	savemask ;
	pthread_t	tid ;
	size_t		msize ;
	volatile int	f_done ;
} ; /* end struct */

typedef MAININFO	maininfo ;

EXTERNC_begin

typedef void		(*maininfohand_t)(int,siginfo_t *,void *) noex ;

extern int maininfo_start	(maininfo *,int,cchar **) noex ;
extern int maininfo_finish	(maininfo *) noex ;
extern int maininfo_setentry	(maininfo *,cchar **,cchar *,int) noex ;
extern int maininfo_sigbegin	(maininfo *,maininfohand_t,cint *) noex ;
extern int maininfo_sigend	(maininfo *) noex ;
extern int maininfo_utilbegin	(maininfo *,int) noex ;
extern int maininfo_utilend	(maininfo *) noex ;
extern int maininfo_srchname	(maininfo *,cchar **) noex ;

EXTERNC_end


#endif /* MAININFO_INCLUDE */


