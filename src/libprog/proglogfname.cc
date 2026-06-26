/* proglogfname */

/* program log-fname-name calculation */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_DEBUG	0		/* run-time debugging */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Descrption:
	This subroutine calculates the log filename for a program.

	Synopsis:
	int proglogfname(PI *pip,char *tmpfname,cc *logcname,cc *logfname) noex

	Arguments:
	pip		program-information pointer
	tmpfname	buffer to receive result
	logcname	directory component name for log-directory
	logfname	the name of the (supposed) log file

	Returns:
	>=0		length of created file-name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LINU */

#include	<proginfo.hh>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int proglogfname(PI *pip,char *rbuf,cchar *logcname,cchar *lfname) noex {
	int		rs = SR_OK ;
	int		pl = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	debugprintf("proglogfname: ent lfname=%s\n",lfname) ;
#endif

	rbuf[0] = '\0' ;
	if ((lfname == NULL) || (lfname[0] == '+'))
	    lfname = pip->searchname ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	debugprintf("proglogfname: adj lfname=%s\n",lfname) ;
#endif

	if ((lfname != NULL) && (lfname[0] != '\0') && (lfname[0] != '-')) {

	    if (lfname[0] != '/') {

	        if (strchr(lfname,'/') != NULL) {
	            rs = mkpath2(rbuf,pip->pr,lfname) ;
	        } else {
		    const char	*logdname ;
		    char	tmpdname[MAXPATHLEN+1] ;
	            if (logcname == NULL) logcname = LOGCNAME ;
		    logdname = logcname ;
		    if (logdname[0] != '/') {
			rs = mkpath2(tmpdname,pip->pr,logcname) ;
		        logdname = tmpdname ;
		    }
		    if (rs >= 0)
	                rs = mkpath2(rbuf,logdname,lfname) ;
	        }
	        pl = rs ;

	    } /* end if */

	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	debugprintf("proglogfname: ret rbuf=%s\n",rbuf) ;
	debugprintf("proglogfname: ret rs=%d pl=%u\n",rs,pl) ;
	}
#endif

	return (rs >= 0) ? pl : rs ;
} /* end subroutine (proglogfname) */


