/* mfs-log */

/* version %I% last-modified %G% */


/* revision history:

	= 2011-01-25, David A�D� Morano
	Originally written for Rightcore Network Services.

	= 2017-08-10, David A�D� Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright � 2011,2017 David A�D� Morano.  All rights reserved. */

#ifndef	MFSLOG_INCLUDE
#define	MFSLOG_INCLUDE	1


#include	<envstandards.h>

#include	<sys/types.h>

#include	<msfile.h>
#include	<userinfo.h>
#include	<lfm.h>

#include	"mfsmain.h"
#include	"defs.h"		/* for PROGINFO */


#ifdef	__cplusplus
extern "C" {
#endif

extern int	logbegin(PROGINFO *,USERINFO *) ;
extern int	logend(PROGINFO *) ;
extern int	logflush(PROGINFO *) ;
extern int	logcheck(PROGINFO *) ;
extern int	logprint(PROGINFO *,cchar *,int) ;
extern int	logprintf(PROGINFO *,cchar *,...) ;
extern int	logssprint(PROGINFO *,cchar *,cchar *,int) ;
extern int	logssprintf(PROGINFO *,cchar *,cchar *,...) ;
extern int	logprogname(PROGINFO *) ;
extern int	logmark(PROGINFO *,int) ;
extern int	loglisteners(PROGINFO *) ;
extern int	logreport(PROGINFO *) ;
extern int	loginvalidcmd(PROGINFO *,cchar *) ;
extern int	loginfo(PROGINFO *) ;
extern int	logoutfile(PROGINFO *,cchar *,cchar *,cchar *) ;
extern int	loglock(PROGINFO *,LFM_CHECK *,cchar *,cchar *) ;

#ifdef	__cplusplus
}
#endif

#endif /* MFSLOG_INCLUDE */


