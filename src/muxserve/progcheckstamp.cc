/* progstampcheck SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* generic (pretty much) front end program subroutine */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable print-outs */
#define	CF_DEBUG	0		/* switchable print-outs */

/* revision history:

	= 2008-09-01, David A­D­ Morano
        This subroutine was borrowed and modified from previous generic
        front-end 'main' subroutines!

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	Check our own (program) time-stamp.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |unllptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	GETFNAME_TYPELOCAL
#define	GETFNAME_TYPELOCAL	0	/* search locally first */
#define	GETFNAME_TYPEROOT	1	/* search programroot area first */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	procupdate(struct proginfo *) ;


/* local variables */


/* exported subroutines */

int progstampcheck(proginfo *pip) noex {
	int	rs = SR_OK ;
	int	f_process = FALSE ;

	if (pip->stampfname == NULL)
	    return SR_FAULT ;

	if (pip->stampfname[0] != '\0') {
	    cchar	*sf = pip->stampfname ;
	    ustat	sb ;
	    pip->daytime = time(NULL) ;
	    f_process = TRUE ;
	    if (u_stat(sf,&sb) >= 0) {
		f_process = ((pip->daytime - sb.st_mtime) >= pip->intmin) ;
	    } /* end if (stat) */
	    if (f_process)
		rs = procupdate(pip) ;
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progstampcheck: ret rs=%d f_proc=%u\n",
		rs,f_processf) ;
#endif

	return (rs >= 0) ? f_process : rs ;
}
/* end subroutine (progstampcheck) */


/* local subroutines */


static int procupdate(pip)
struct proginfo	*pip ;
{
	bfile	tsfile, *tfp = &tsfile ;
	int	rs ;
	cchar	*sf = pip->stampfname ;

	if ((rs = bopen(tfp,sf,"wct",0666)) >= 0) {
	    cint	elen = MAXNAMELEN ;
	    cchar	*nn = pip->nodename ;
	    cchar	*un = pip->username ;
	    char	timebuf[TIMEBUFLEN+1] ;
	    char	ebuf[MAXNAMELEN+1] ;

	    if ((rs = sncpy3(ebuf,elen,nn,"!",un)) >= 0) {
		cchar	*name = pip->name ;
		cchar	*fmt = "%s %s\n" ;
	        timestr_logz(pip->daytime,timebuf) ;
		if ((name != NULL) && (name[0] != '\0'))
	            fmt = "%s %s (%s)\n" ;
	        rs = bprintf(tfp,fmt,timebuf,ebuf,name) ;
	    }

	    bclose(tfp) ;
	} /* end if (opened) */

	return rs ;
}
/* end subroutine (procupdate) */


