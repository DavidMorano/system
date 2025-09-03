/* progconfig HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program configuration code */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGCONFIG_INCLUDE
#define	PROGCONFIG_INCLUDE

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<expcook.h>
#include	<paramfile.h>
#include	<vecstr.h>

#include	"config.h"
#include	"defs.h"


#define	PROGCONFIG	struct progconfig_head
#define	PROGCONFIG_FL	struct progconfig_flags


struct proginfo_flags {
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
	proginfo	*pip ;
	paramfile	p ;
	expcool		cooks ;
	vecstr		stores ;
	PROFCONFIG_FL	fl ;
	PROFCONFIG_FL	have ;
	PROFCONFIG_FL	change ;
	PROFCONFIG_FL	open ;
	PROFCONFIG_FL	final ;
	uint		magic ;
} ;

EXTERNC_begin

extern int progconfigstart(PROGCONFIG *,proginfo *,cchar *) noex ;
extern int progconfigcheck(PROGCONFIG *) noex ;
extern int progconfigread(PROGCONFIG *) noex ;
extern int progconfigfinish(PROGCONFIG *) noex ;

EXTERNC_end


#endif /* PROGCONFIG */


