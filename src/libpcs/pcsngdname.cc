/* pcsngdname SUPPORT */
/* lang=C++20 */

/* create a directory name when given a newsgroup name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcsngdname

	Description:
	This subroutine will take its argument to be a newsgroup
	name.  The subroutine will return the corresponding directory
	name in the spool area.

	Synopsis:
	int pcsngdname(cc *pcs,char *rbuf,cc *newsdname,cc *ngname) noex

	Arguments:
	pcs		PCS program root
	rbuf		the directory path to the newsgroup relative
			to the BBNEWS spool directory
	newsdname	the top of the BBNEWS spool directory
	ngname		a name of a newsgroup

	Returns:
	>0		length of returned directory name
	SR_FAULT	nullptr argument(s) was given
	SR_INVALID	an invalid argument was given
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bwops.h>
#include	<strn.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<pow.h>
#include	<mkchar.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"pcsngdname.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* external variables */


/* local structures */


/* forward references */

static int	mknewsdname(cchar *,char *,cchar *) noex ;
static int	mkdname(char *,int,cchar *,int) noex ;
static int	ndots(cchar *) noex ;
static bool	isNotOurs(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsngdname(cchar *pcs,char *rbuf,cchar *newsdname,cchar *ngname) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (pcs && rbuf && newsdname && ngname) {
	    rs = SR_INVALID ;
	    if (ngname[0]) {
	        cint	n = ndots(ngname) ;
	        if ((rs = mknewsdname(pcs,rbuf,newsdname)) >= 0) {
	            USTAT	sb ;
	            cint	npow = ipow(2,n) ;
	            int		rlen = rs ;
	            if (n > 0) {
	                for (int c = 0 ; c < npow ; c += 1) {
	                    if ((rs = mkdname(rbuf,rlen,ngname,c)) >= 0) {
		                len = rs ;
		                if ((rs = u_stat(rbuf,&sb)) >= 0) {
			            if (S_ISDIR(sb.st_mode)) break ;
			            rs = SR_NOTDIR ;
		                }
		            } /* end if (mkdname) */
		            if ((rs >= 0) || isNotOurs(rs)) break ;
	                } /* end for */
	            } else {
	                if ((rs = pathadd(rbuf,rlen,ngname)) >= 0) {
		            len = rs ;
		            if ((rs = u_stat(rbuf,&sb)) >= 0) {
			        if (! S_ISDIR(sb.st_mode)) rs = SR_NOTDIR ;
		            }
		        } /* end if (pathadd) */
	            } /* end if */
	        } /* end if (mkpath) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pcsngdname) */


/* local subroutines */

static int mknewsdname(cchar *pr,char *rbuf,cchar *newsdname) noex {
	int		rs ;
	if (newsdname[0] != '/') {
	    rs = mkpath2(rbuf,pr,newsdname) ;
	} else {
	    rs = mkpath1(rbuf,newsdname) ;
	}
	return rs ;
}
/* end subroutine (mknewsdname) */

static int mkdname(char *rbuf,int rlen,cchar *ngname,int mask) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = pathadd(rbuf,rlen,ngname)) >= 0) {
	    int		nlen = (rlen+1) ;
	    int		c = 0 ;
	    len = rs ;
	    if (cchar *tp ; (tp = strnchr(rbuf,rlen,'.')) != nullptr) {
		nlen = (tp-rbuf) ;
	    }
	    for (int i = (len-1) ; i >= nlen ; i -= 1) {
		cint	ch = mkchar(rbuf[i]) ;
		if (ch == '.') {
		    if (! bwtsti(mask,c)) {
			rbuf[i] = '/' ;
			c += 1 ;
		    }
		}
	    } /* end if */
	} /* end if (pathadd) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkdname) */

static int ndots(cchar *ngname) noex {
	int		n = 0 ;
	for (int i = 0 ; ngname[i] ; i += 1) {
	    if (ngname[i] == '.') n += 1 ;
	}
	return n ;
}
/* end subroutine (ndots) */

static bool isNotOurs(int rs) noex {
	bool		f = true ;
	if (isNotPresent(rs) || (rs == SR_NOTDIR)) {
	    f = false ;
	}
	return f ;
}
/* end subroutine (isNotOurs) */


