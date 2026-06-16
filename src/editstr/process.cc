/* editstr_process SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process a file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_DEBUG	0		/* time-time debug print-outs */

/* revision history:

	= 96/03/01, David A­D­ Morano
	The subroutine was adapted from others programs that
	did similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Description:
	This subroutine just provides optional expansion of
	directories.  The real work is done by the PROCFILE subroutine.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<csignal>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<paramopt.h>
#include	<fsdir.h>
#include	<fsdirtree.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	PI		proginfo

#ifndef	SUFBUFLEN
#define	SUFBUFLEN	MAXNAMELEN
#endif


/* external subroutines */

extern int	procfile(PI *,paramopt *,cchar *) noex ;


/* external variables */

extern int	if_int ;


/* local structures */


/* forward references */

local bool	isdotdir(cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int process(PI *pip,paramopt *pop,cchar *fname) noex {
	ustat	sb ;
	int	rs ;
	int	c ;
	int	f_dir ;

	if (fname == NULL) return SR_FAULT ;
	if (fname[0] == '\0') return SR_INVALID ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("process: fname=%s\n",fname) ;
#endif /* CF_DEBUG */

	c = 0 ;
	rs = u_stat(fname,&sb) ;

	f_dir = S_ISDIR(sb.st_mode) ;
	if ((rs >= 0) && f_dir && (! pip->fl.follow)) {

	    rs = u_lstat(fname,&sb) ;

	    f_dir = S_ISDIR(sb.st_mode) ;
	}

	if (rs < 0)
	    goto ret0 ;

	if (f_dir) {

	    int		del ;

	    char	tmpfname[MAXPATHLEN + 1] ;


	    if (pip->fl.recurse) {
		cauto &fdm = fsdirtreem ;
	        fsdirtree	dt ;
	        int	fdo = 0 ;
	        char	dename[MAXPATHLEN + 1] ;
	        fdo |= ((pip->fl.follow) ? fdm.follow : 0) ;
	        if ((rs = fsdirtree_open(&dt,fname,fmo)) >= 0) {
		    cint	mpl = MAXPATHLEN ;
	            while (! if_int) {

	                del = fsdirtree_read(&dt,&sb,dename,mpl) ;

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("process: fsdirtree_read() rs=%d\n",
	                        del) ;
#endif

	                if (del <= 0) break ;

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("process: de name=%s\n",dename) ;
#endif

	                if (isdotdir(fname)) {
	                    rs = mkpath1(tmpfname,dename) ;
	                } else {
	                    rs = mkpath2(tmpfname,fname,dename) ;
			}

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("process: tmpfname=%s\n",tmpfname) ;
#endif

	                if (rs > 0) {
	                    rs = procfile(pip,pop,tmpfname) ;
			    c += rs ;
	                }
	                if ((rs < 0) && (! pip->fl.nostop)) break ;
	            } /* end while (looping through entries) */
	            fsdirtree_close(&dt) ;
	        } /* end if (opened directory tree) */
	    } else {
	        fsdir		d ;
	        fsdir_ent	ds ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("process: not recursing\n") ;
#endif

	        if ((rs = fsdir_open(&d,fname)) >= 0) {
	            while ((! if_int) &&
	                ((del = fsdir_read(&d,&ds)) > 0)) {

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("process: de name=%s\n",ds.name) ;
#endif

	                if (ds.name[0] == '.') {
	                    if ((ds.name[1] == '\0') ||
	                        ((ds.name[1] == '.') && (ds.name[2] == '\0')))
	                        continue ;
	                }

	                if (isdotdir(fname)) {
	                    rs = mkpath1(tmpfname,ds.name) ;
			} else {
	                    rs = mkpath2(tmpfname,fname,ds.name) ;
			}

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("process: tmpfname=%s\n",
	                        tmpfname) ;
#endif

	                if (rs > 0) {
	                    rs = procfile(pip,pop,tmpfname) ;

#if	CF_DEBUG
	                    if (DEBUGLEVEL(2))
	                        debugprintf("process: procfile() rs=%d\n",rs) ;
#endif

	                    if (rs > 0)
	                        c += 1 ;

	                }

	                if ((rs < 0) && (! pip->fl.nostop))
	                    break ;

	            } /* end while (looping through entries) */
	            fsdir_close(&d) ;
	        } /* end if (opened directory tree) */
	    } /* end if (directory) */
	} else if (S_ISREG(sb.st_mode)) {
	    rs = procfile(pip,pop,fname) ;
	    c += rs ;
	} /* end if */

ret0:
bad0:

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("process: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (process) */


/* local subroutines */

local bool isdotdir(cchar *dname) noex {
	int	f = false ;
	if (dname[0] == '.') {
	    f = (dname[1] == '\0') ;
	    if (! f) {
	        f = ((dname[1] == '/') && (dname[2] == '\0')) ;
	    }
	}
	return f ;
}
/* end subroutine (isdotdir) */


