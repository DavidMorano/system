/* progspec SSUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This subroutine just provides optional expansion of
	directories.  The real work is done by the |progfile()|
	subroutine.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<paramopt.h>
#include	<fsdir.h>
#include	<fsdirtree.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	SUFBUFLEN
#define	SUFBUFLEN	MAXNAMELEN
#endif


/* external subroutines */

extern int	mkpath1(char *,const char *) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	matstr(const char **,const char *,int) ;
extern int	matcasestr(const char **,const char *,int) ;
extern int	sfbasename(const char *,int,const char **) ;
extern int	isDotDir(const char *) ;

extern int	progabort(PROGINFO *) ;
extern int	progfile(PROGINFO *,PARAMOPT *,const char *) ;

extern char	*timestr_logz(time_t,char *) ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int progspec(PROGINFO *pip,PARAMOPT *pop,cchar *fname) noex {
	USTAT		sb ;
	int		rs ;
	int		c = 0 ;
	int		f_dir ;

	if (fname == NULL)
	    return SR_FAULT ;

	if (fname[0] == '\0')
	    return SR_INVALID ;

	rs = u_stat(fname,&sb) ;

	f_dir = S_ISDIR(sb.st_mode) ;
	if ((rs >= 0) && f_dir && (! pip->fl.follow)) {
	    rs = u_lstat(fname,&sb) ;
	    f_dir = S_ISDIR(sb.st_mode) ;
	}

	if (rs >= 0) {

	    if (f_dir) {
	        int	del ;
	        char	tmpfname[MAXPATHLEN + 1] ;

	        if (pip->fl.recurse) {
	            FSDIRTREE		dt ;
	            FSDIRTREE_STAT	fsb ;
	            int		dtopts = 0 ;
	            char	dename[MAXPATHLEN + 1] ;

	            dtopts |= ((pip->fl.follow) ? FSDIRTREE_MFOLLOW : 0) ;
	            if ((rs = fsdirtree_open(&dt,fname,dtopts)) >= 0) {
	                const int	mpl = MAXPATHLEN ;

	                while (rs >= 0) {
	                    del = fsdirtree_read(&dt,&fsb,dename,mpl) ;
	                    if (del == 0) break ;

	                    rs = del ;
	                    if (rs < 0) break ;

	                    if (isDotDir(fname)) {
	                        rs = mkpath1(tmpfname,dename) ;
	                    } else {
	                        rs = mkpath2(tmpfname,fname,dename) ;
			    }

	                    if (rs > 0) {
	                        rs = progfile(pip,pop,tmpfname) ;
	                        if (rs > 0) c += 1 ;
	                    } /* end if */

	                    if (progabort(pip) > 0) break ;
	                    if ((rs < 0) && (! pip->fl.nostop)) break ;
	                } /* end while (looping through entries) */

	                fsdirtree_close(&dt) ;
	            } /* end if (opened directory tree) */

	        } else {
	            FSDIR	d ;
	            FSDIR_ENT	ds ;

	            if ((rs = fsdir_open(&d,fname)) >= 0) {

	                while ((rs = fsdir_read(&d,&ds)) > 0) {
	                    del = rs ;

	                    if (ds.name[0] == '.') continue ;

	                    if ((rs = mkpath2(tmpfname,fname,ds.name)) > 0) {

	                        rs = progfile(pip,pop,tmpfname) ;
	                        if (rs > 0) c += 1 ;
	                    } /* end if */

	                    if (progabort(pip) > 0) break ;
	                    if ((rs < 0) && (! pip->fl.nostop)) break ;
	                } /* end while (looping through entries) */

	                fsdir_close(&d) ;
	            } /* end if (opened directory tree) */

	        } /* end if (directory) */

	    } else if (S_ISREG(sb.st_mode)) {

	        rs = progfile(pip,pop,fname) ;

	        if (rs > 0)
	            c += 1 ;

	    } /* end if */

	    if (rs == SR_ACCESS) {
	        if (! pip->fl.quiet) {
	            printf(pip->efp,
	                "%s: could not open directory (%d)\n",
	                pip->progname,rs) ;
	            printf(pip->efp,
	                "%s: directory=%s\n",
	                pip->progname,fname) ;
	        }
	    } /* end if (could not open directory) */

	} /* end if (ok) */

ret0:
bad0:

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (progspec) */


