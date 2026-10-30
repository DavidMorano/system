/* proglogent SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* check a file name */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	proglogent

  	Description:
	Check the given named file for a log length limit.

	Synopsis:
	int proglogent(PI *pip,cchar *name,int logsize) noex

	Arguments:
	pip		global data pointer
	name		log file name
	logsize		size to check for

	Returns:
	>0		skip this directory entry in directory walk
	0		continue with directory walk as usual
	<0		exit directory walk altogether (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strochr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<varsub.h>		/* LIBUC */
#include	<fsdir.h>		/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	checkstar	(PI *,cchar *,int) noex ;
local int	checkdir	(PI *,cchar *,int) noex ;
local int	checkfile	(PI *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int proglogent(PI *pip,cchar *name,int logsize) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl, l2 ;
	int		cl ;
	int		f_star = false ;
	cchar	*fname ;
	cchar	*cp ;
	char		buf[BUFLEN + 1] ;
	char		tmpfname[MAXPATHLEN + 1] ;

#ifdef	COMMENT

	if (((sl = varsub_subbuf(dp,ep,name,-1,buf,BUFLEN)) > 0) &&
	    ((l2 = expander(pip,buf,sl,tmpfname,MAXPATHLEN)) > 0)) {

	    name = buf ;
	    rs = snwcpy(buf,BUFLEN,tmpfname,l2) ;

	}

	if (rs < 0)
	    goto ret0 ;

#endif /* COMMENT */

/* continue with regular file stuff */

	fname = name ;
	if (name[0] != '/') {

	    f_star = (strchr(name,'*') != nullptr) ;

	    fname = tmpfname ;
	    rs = mkpath2(tmpfname, pip->logrootdname,name) ;

	}

	if (rs < 0)
	    goto ret0 ;

/* check (again if not already) for a star */

	if (! f_star) {

	    cl = sfbasename(fname,-1,&cp) ;

	    if ((cl > 0) && (strnchr(cp,cl,'*') != nullptr)) {
	        f_star = true ;
	    }

	}

/* do it */

	if (f_star) {

	    rs = checkstar(pip,fname,logsize) ;

	} else {
	    ustat	sb ;
	    if ((rs1 = u_stat(fname,&sb)) >= 0) {

	        if (S_ISDIR(sb.st_mode)) {
	            rs = checkdir(pip,fname,logsize) ;
	        } else if (S_ISREG(sb.st_mode)) {
	            pip->stab.total += 1 ;
	            rs = checkfile(pip,fname,logsize) ;
	        } /* end */

	    } else if (pip->debuglevel > 0) {
	        pip->stab.total += 1 ;
	        pip->eprintf("file=%s (%d)\n", fname,rs1) ;
	    } /* end */

	} /* end if */

ret1:
ret0:
	return rs ;
} /* end subroutine (proglogent) */


/* local subroutines */

local int checkstar(PI *pip,cchar *name,int logsize) noex {
	int		rs, rs1 ;
	int		cl ;
	cchar	*cp ;
	char		dname[MAXPATHLEN + 1] ;
	char		bname[MAXNAMELEN + 1] ;
	char		fname[MAXPATHLEN + 1] ;

	cl = sfdirname(name,-1,&cp) ;

	rs = snwcpy(dname,MAXPATHLEN,cp,cl) ;

	if (rs < 0)
	    goto ret0 ;

	cl = sfbasename(name,-1,&cp) ;

	rs = snwcpy(bname,MAXNAMELEN,cp,cl) ;

	if (rs < 0)
	    goto ret0 ;

	fsdir		d ;
	if ((rs1 = fsdir_open(&d,dname)) >= 0) {
	fsdir_ent	ds ;

	    while (fsdir_read(&d,&ds) > 0) {
	        if (ds.name[0] == '.') continue ;

	        pip->stab.total += 1 ;
	        if (starmat(bname,ds.name)) {
	            if ((rs = mkpath2(fname,dname,ds.name)) >= 0) {
	                rs = checkfile(pip,fname,logsize) ;
		    }
	        } /* end if */

	        if (rs < 0) break ;
	    } /* end while */

	    fsdir_close(&d) ;
	} /* end if */

	if ((rs1 < 0) && (pip->debuglevel > 0)) {
	    pip->eprintf("directory=%s (%d)\n",dname,rs1) ;
	}

ret0:
	return rs ;
} /* end subroutine (checkstar) */

local int checkdir(PI *pip,cc *dname,int logsize) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	char		fname[MAXPATHLEN + 1] ;

	if (fsdir d ; (rs1 = fsdir_open(&d,dname)) >= 0) {
	    fsdir_ent	ds ;

	    while (fsdir_read(&d,&ds) > 0) {
	        if (ds.name[0] == '.') continue ;

	        pip->stab.total += 1 ;
	        if ((rs = mkpath2(fname,dname,ds.name)) >= 0) {
	            rs = checkfile(pip,fname,logsize) ;
		}

	        if (rs < 0) break ;
	    } /* end while */

	    fsdir_close(&d) ;
	} /* end if */

	if ((rs1 < 0) && (pip->debuglevel > 0)) {
	    pip->eprintf("directory=%s (%d)\n",dname,rs1) ;
	}

ret0:
	return rs ;
} /* end subroutine (checkdir) */

local int checkfile(PI *pip,cc *fname,int logsize) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		perm ;
	int		f_over = false ;

#ifdef	COMMENT
	perm = R_OK | W_OK ;
	rs = u_access(fname,perm) ;
	if (rs < 0) goto ret0 ;
#endif /* COMMENT */

	logfile cur ;
	if ((rs1 = logfile_open(&cur,fname,0,0666,"checklogs")) >= 0) {

	    pip->stab.checked += 1 ;
	    rs = 0 ;
	    if (! pip->fl.nogo) {
	        rs = logfile_cksize(&cur,logsize) ;
	    }

	    if (rs > 0) {
	        f_over = true ;
	        pip->stab.oversized += 1 ;
	    }

	    logfile_close(&cur) ;
	} /* end if */

	if (pip->debuglevel > 0) {
	    pip->eprintf("file=%s (%d) ov=%u\n",fname,rs1,f_over) ;
	}

ret0:
	return rs ;
} /* end subroutine (checkfile) */


