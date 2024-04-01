/* proglogid SUPPORT */
/* lang=C++20 */

/* process the service names given us */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable debug print-outs */
#define	CF_SERIAL	1		/* serial or PID */

/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was adopted from the DWD program.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:

	Description:
	This subroutine creates a log-id for use in logging entries
	to a log-file.

	Synopsis:
	int proglogid(PROGINFO *pip) noex

	Arguments:
	pip		pointer to PROGINFO object

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	For programs that are actually built-in SHELL commands, the
	compile-time switch flag CF_SERIAL must always be enabled
	(set to '1') so that the process PID is not used. This is
	because the process PID will always be the same for calls
	to built-in commands.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<ugetpid.h>
#include	<localmisc.h>		/* |LOGIDLEN| */

#include	"proginfo.h"


/* local defines */

#ifndef	NULLFNAME
#define	NULLFNAME	"/dev/null"
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern int	snsd(char *,int,cchar *,uint) ;
extern int	snsdd(char *,int,cchar *,uint) ;
extern int	snddd(char *,int,uint,uint) ;
extern int	snsds(char *,int,cchar *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	perm(cchar *,uid_t,gid_t,gid_t *,int) ;
extern int	fperm(int,uid_t,gid_t,gid_t *,int) ;
extern int	getpwd(char *,int) ;
extern int	mklogid(char *,int,cchar *,int,int) ;
extern int	getserial(cchar *) ;
extern int	isprintlatin(int) ;


/* externals variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int proglogid(PROGINFO *pip) noex {
	int		rs = SR_OK ;
	int		v = -1 ;
	int		ll = 0 ;

	if (pip->pid < 0) pip->pid = uc_getpid() ;

#if	CF_SERIAL
	{
	    cchar	*sfname = SERIALFNAME ;
	    cchar	*tdn ;
	    char	tbuf[MAXPATHLEN + 1] ;

	    tbuf[0] = '\0' ;
	    rs = mkpath3(tbuf,pip->pr,VCNAME,sfname) ;
	    if ((rs >= 0) && (tbuf[0] != '\0')) {
	        v = getserial(tbuf) ;
	    }

	    if ((rs >= 0) && (v < 0)) {
	        tdn = pip->tmpdname ;
	        if ((tdn != NULL) && (tdn[0] != '0')) {
	            if ((rs = mkpath2(tbuf,pip->tmpdname,sfname)) >= 0) {
	                if (tbuf[0] != '\0') {
	                    v = getserial(tbuf) ;
			}
		    }
	        }
	    }
	} /* end block */
#endif /* CF_SERIAL */

	if ((rs >= 0) && (v < 0)) {
	    v = ((int) pip->pid) ;
	}

	if (rs >= 0) {
	    cint	llen = LOGIDLEN ;
	    cchar	*nn = pip->nodename ;
	    char	lbuf[LOGIDLEN + 1] ;
	    if ((rs = mklogid(lbuf,llen,nn,-1,v)) > 0) {
		cchar	**vpp = &pip->logid ;
		ll = rs ;
	        rs = proginfo_setentry(pip,vpp,lbuf,ll) ;
	    }

	} /* end block (logid) */

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (proglogid) */


