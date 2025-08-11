/* rmdirfiles SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove directory files (as specified with an age interval) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch but based on previous
	versions.

	= 2017-12-29, David A­D­ Morano
        No big deal, but I now capture any unexpected bad return from
        |u_stat(3u)| using |isNotPresent(3dam)|.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rmdirfiles

	Description:
	Remove files from a specified directory that are older than
	a specified interval (in seconds), and which match a prefix
	c-string.

	Synopsis:
	int rmdirfiles(cchar *dname,cchar *prefix,int to) noex

	Arguments:
	dname		directory name (as a string)
	prefix		optional prefix string for selection for removal
	to		time-out interval in seconds

	Returns:
	>=0		number of files removed (deleted)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<fsdir.h>
#include	<vecstr.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<nleadstr.h>
#include	<hasx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"rmdirs.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	vecstr_load(vecstr *,cchar *,cchar *,int) noex ;
static int	vecstr_dirfilesdelete(vecstr *) noex ;
static int	premat(cchar *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int rmdirfiles(cchar *dn,cchar *pre,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (dn) {
	    rs = SR_INVALID ;
	    if (dn[0]) {
	        if (vecstr fs ; (rs = fs.start(0,0)) >= 0) {
	            if ((rs = vecstr_load(&fs,dn,pre,to)) > 0) {
	                rs = vecstr_dirfilesdelete(&fs) ;
	                c += rs ;
	            }
	            rs1 = fs.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (files) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (rmdirfiles) */


/* local subroutines */

static int vecstr_load(vecstr *flp,cchar *dn,cchar *pre,int to) noex {
	custime		dt = time(nullptr) ;
	int		rs ;
	int		rs1 ;
	int		prelen = 0 ;
	int		c = 0 ;
	if (pre) prelen = lenstr(pre) ;
	if (char *rbuf ; (rs = malloc_mp(&rbuf)) >= 0) {
	    if ((rs = mkpath1(rbuf,dn)) >= 0) {
	        cint	rl = rs ;
		if (char *nbuf ; (rs = malloc_mn(&nbuf)) >= 0) {
		    cint	nlen = rs ;
		    if (fsdir dir ; (rs = fsdir_open(&dir,dn)) >= 0) {
		        fsdir_ent	de ;
	                while ((rs = fsdir_read(&dir,&de,nbuf,nlen)) > 0) {
	                    cchar	*sp = de.name ;
	                    cint	sl = rs ;
	                    if (hasNotDots(sp,sl)) {
	                        if (premat(pre,prelen,sp,sl)) {
	                            if ((rs = pathadd(rbuf,rl,sp)) >= 0) {
	            		        USTAT	usb ;
	                                cint	ql = rs ;
	                                if ((rs = u_stat(rbuf,&usb)) >= 0) {
	                                    const time_t ft = usb.st_mtime ;
	                                    if ((dt - ft) >= to) {
	                                        c += 1 ;
	                                        rs = vecstr_add(flp,rbuf,ql) ;
	                                    }
				        } else if (isNotPresent(rs)) {
				            rs = SR_OK ;
	                                } /* end if (stat) */
	                            } /* end if (pathadd) */
	                        } /* end if (premat) */
	                    } /* end if (hasNotDots) */
	                    if (rs < 0) break ;
	                } /* end while (fsdir_read) */
	                rs1 = fsdir_close(&dir) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (fsdir) */
	    	    rs1 = uc_free(nbuf) ;
	    	    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (mkpath) */
	    rs1 = uc_free(rbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_loaddirfiles) */

static int vecstr_dirfilesdelete(vecstr *flp) noex {
    	int		rsn = SR_NOTFOUND ;
    	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*fp{} ;
	for (int i = 0 ; (rs1 = flp->get(i,&fp)) >= 0 ; i += 1) {
	    if ((fp != nullptr) && (fp[0] != '\0')) {
	        c += 1 ;
	        u_unlink(fp) ;
	    }
	} /* end for */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_dirfilesdelete) */

static int premat(cchar *prefix,int prelen,cchar *sp,int sl) noex {
	int		f = true ;
	if ((prefix != nullptr) && (prefix[0] != '\0')) {
	    cint	m = nleadstr(prefix,sp,sl) ;
	    f = (m == prelen) ;
	}
	return f ;
}
/* end subroutine (premat) */


