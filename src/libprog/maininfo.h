/* maininfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* version %I% last-modified %G% */


/* revision history:

	= 2004-05-05, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	MAININFO_INCLUDE
#define	MAININFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pthread.h>
#include	<csignal>
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecstr.h>

#include	"sighand.h"


#define	MAININFO	struct maininfo
#define	MAININFO_FL	struct maininfo_flags


typedef void		(*maininfohand_t)(int,siginfo_t *,void *) noex ;


struct maininfo_flags {
	uint		progdash:1 ;	/* leading dash on program-name */
	uint		utilout:1 ;	/* utility is out running */
} ; /* end struct */

struct maininfo {
	SIGHAND		sh ;
	vecstr		stores ;
	stack_t		astack ;
	cchar		*progdname ;
	cchar		*progename ;
	cchar		*progname ;
	cchar		*srchname ;
	cchar		*symname ;
	void		*mdata ;
	MAININFO_FL	have, f, changed, final ;
	MAININFO_FL	open ;
	sigset_t	savemask ;
	pthread_t	tid ;
	size_t		msize ;
	volatile int	f_done ;
} ; /* end struct */

EXTERNC_begin

extern int maininfo_start(MAININFO *,int,cchar **) noex ;
extern int maininfo_finish(MAININFO *) noex ;
extern int maininfo_setentry(MAININFO *,cchar **,cchar *,int) noex ;
extern int maininfo_sigbegin(MAININFO *,maininfohand_t,cint *) noex ;
extern int maininfo_sigend(MAININFO *) noex ;
extern int maininfo_utilbegin(MAININFO *,int) noex ;
extern int maininfo_utilend(MAININFO *) noex ;
extern int maininfo_srchname(MAININFO *,cchar **) noex ;

EXTERNC_end


#endif /* MAININFO_INCLUDE */


