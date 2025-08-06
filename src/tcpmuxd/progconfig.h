/* progconfig */

/* program configuration code */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#ifndef	PROGCONFIG_INCLUDE
#define	PROGCONFIG_INCLUDE	1


#include	<sys/types.h>

#include	<expcook.h>
#include	<paramfile.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


#define	PROGCONFIG	struct progconfig_head


struct progconfig_flags {
	uint		srvtab:1 ;	/* do we have an SRVTAB ? */
	uint		acctab:1 ;	/* do we have an ACCess TABle ? */
	uint		passwd:1 ;	/* PWFILE? */
	uint		pidfile:1 ;
	uint		lockfile:1 ;	/* have a lock file */
	uint		mspoll:1 ;	/* do MS polling */
	uint		zerospeed:1 ;	/* zero out 'speed' element */
	uint		nopass:1 ;	/* do not listen on PASS */
	uint		msfile:1 ;
	uint		logfile:1 ;
	uint		loglen:1 ;
	uint		lockint:1 ;
	uint		runint:1 ;
	uint		pollint:1 ;
	uint		speedint:1 ;
	uint		markint:1 ;
	uint		cmd:1 ;
	uint		p:1 ;		/* paramfile */
	uint		lockinfo:1 ;
	uint		portspec:1 ;
} ;

struct progconfig_head {
	unsigned long	magic ;
	cchar	*configfname ;
	cchar	*pidfname ;
	cchar	*lockfname ;		/* lock file */
	cchar	*svcfname ;		/* SVCTAB file */
	cchar	*accfname ;		/* ACCTAB file */
	cchar	*passfname ;		/* pass (FD) file */
	cchar	*reqfname ;		/* request file */
	cchar	*shmfname ;		/* SHM file */
	cchar	*msfname ;		/* MS file */
	cchar	*logfname ;
	cchar	*prog_rmail ;
	cchar	*prog_sendmail ;
	cchar	*orgcode ;		/* organization code */
	cchar	*speedname ;		/* CPUSPEED module name */
	cchar	*portspec ;
	struct proginfo	*pip ;
	struct progconfig_flags	f ;
	struct progconfig_flags	have ;
	struct progconfig_flags	change ;
	struct progconfig_flags	open ;
	struct progconfig_flags	final ;
	PARAMFILE	p ;
	EXPCOOK	cooks ;
	vecstr		stores ;
} ;

EXTERNC_begin

extern int progconfigstart(PROGCONFIG *,proginfo *,cchar *) noex ;
extern int progconfigcheck(PROGCONFIG *) noex ;
extern int progconfigread(PROGCONFIG *) noex ;
extern int progconfigfinish(PROGCONFIG *) noex ;

EXTERNC_end


#endif /* PROGCONFIG */


