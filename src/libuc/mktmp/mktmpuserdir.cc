/* mktmpuserdir SUPPORT */
/* lang=C++20 */

/* make a directory in the TMP-USER area */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mktmpuserdir

	Description:
	This subroutine creates a directory that is specific to a
	user reserved directory inside the TMP directory.  For
	example, given a user named 'fred' and a directory name
	'junker' to create, the directory:
		/tmp/users/fred/junker/
	will be created.

	Synopsis:
	int mktmpuserdir(char *rbuf,cchar *un,cchar *dn,mode_t m) noex

	Arguments:
	rbuf		buffer to receive resulting created directory name
	un		optional username
	dn		basename of directory to create
	m		directory creation mode

	Returns:
	>=0		length of resulting directory name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getxusername.h>
#include	<dirs.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"mktmp.h"


/* local defines */


/* local namespaces */


/* local typenames */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int mkourdir(cchar *,mode_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mktmpuser(char *rbuf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf) {
	    char	*ubuf{} ;
	    rbuf[0] = '\0' ;
	    if ((rs = malloc_un(&ubuf)) >= 0) {
		cint	ulen = rs ;
	        if ((rs = getusername(ubuf,ulen,-1)) >= 0) {
		    rs = mktmpuserx(rbuf,ubuf) ;
		    rl = rs ;
		} /* end if (getusername) */
		rs1 = uc_free(ubuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mktmpuser) */

int mktmpuserx(char *rbuf,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf) {
	    rbuf[0] = '\0' ;
	    if ((rs = mktmpusers(rbuf)) >= 0) {
		cint	tl = rs ;
		if ((rs = pathadd(rbuf,tl,un)) >= 0) {
		    USTAT	sb ;
		    cmode	dm = 0775 ;
		    rl = rs ;
		    if ((rs = uc_stat(rbuf,&sb)) >= 0) {
			if (! S_ISDIR(sb.st_mode)) {
			    if ((rs = rmdirs(rbuf)) >= 0) {
				rs = mkourdir(rbuf,dm) ;
			    }
			}
		    } else if (isNotPresent(rs)) {
			rs = mkourdir(rbuf,dm) ;
		    } /* end if */
		} /* end if (pathadd) */
	    } /* end if (mktmpusers) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mktmpuserx) */

int mktmpuserdir(char *rbuf,cchar *un,cchar *dname,mode_t dm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf && un && dname) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (un[0] && dname[0]) {
		char	*tbuf{} ;
		if ((rs = malloc_mp(&tbuf)) >= 0) {
		    if ((rs = mktmpuserx(tbuf,un)) >= 0) {
		        rs = mktmpdir(rbuf,tbuf,dm) ;
		        len = rs ;
	            } /* end if (ok) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mktmpuserdir) */


/* local subroutines */

static int mkourdir(cchar *rbuf,cmode dm) noex {
	int		rs ;
	if ((rs = u_mkdir(rbuf,dm)) >= 0) {
	    rs = uc_minmod(rbuf,dm) ;
	}
	return rs ;
}
/* end subroutine (mkourdir) */


