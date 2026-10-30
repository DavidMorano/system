/* proglogid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process the service names given us */
/* version %I% last-modified %G% */

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
	int proglogid(PI *pip) noex

	Arguments:
	pip		pointer to PI object

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
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucgetpid.h>		/* LIBUC */
#include	<getserial.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mklogid.h>		/* LIBUC */
#include	<localmisc.h>		/* |LOGIDLEN| */

#include	"proginfo.hh"


/* local defines */

#ifndef	CF_SERIAL
#define	CF_SERIAL	0		/* serial or PID */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* externals variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_serial		= CF_SERIAL ;


/* exported variables */


/* exported subroutines */

int proglogid(PI *pip) noex {
	int		rs = SR_OK ;
	int		v = -1 ;
	int		ll = 0 ; /* return-value */

	if (pip->pid < 0) pip->pid = uc_getpid() ;

	if_constexpr (f_serial) {
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
	        if ((tdn != nullptr) && (tdn[0] != '0')) {
	            if ((rs = mkpath2(tbuf,pip->tmpdname,sfname)) >= 0) {
	                if (tbuf[0] != '\0') {
	                    v = getserial(tbuf) ;
			}
		    }
	        }
	    }
	} /* end if_constexpr (f_serial) */

	if ((rs >= 0) && (v < 0)) {
	    v = ((int) pip->pid) ;
	} /* end if */

	if (rs >= 0) {
	    cint	llen = LOGIDLEN ;
	    cchar	*nn = pip->nodename ;
	    char	lbuf[LOGIDLEN + 1] ;
	    if ((rs = mklogid(lbuf,llen,nn,-1,v)) > 0) {
		cchar	**vpp = &pip->logid ;
		ll = rs ;
	        rs = pip->setent(vpp,lbuf,ll) ;
	    }
	} /* end if (logid) */

	return (rs >= 0) ? ll : rs ;
} /* end subroutine (proglogid) */


