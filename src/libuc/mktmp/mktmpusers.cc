/* mktmpusers SUPPORT */
/* lang=C++20 */

/* make the USERS directory in TMP directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mktmpusers

	Description:
	This subroutine creates the USERS directory in the TMP directory.
	That is:
		/tmp/users

	Synopsis:
	int mktmpusers(char *rbuf) noex

	Arguments:
	rbuf		buffer to receive resulting created directory name

	Returns:
	>=0		length of resulting directory name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<strlibval.hh>
#include	<getxusername.h>
#include	<syswords.hh>
#include	<mkpathx.h>
#include	<strlibval.hh>
#include	<localmisc.h>

#include	"mktmp.h"


/* local defines */

#ifndef	TMPUSERDMODE
#define	TMPUSERDMODE	(S_IAMB | S_ISVTX)
#endif


/* local namespaces */


/* local typenames */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	mktmpuser(char *,cchar *) noex ;
static int	ensuremode(cchar *,mode_t) noex ;


/* local variables */

static strlibval	strtmpdir(strlibval_tmpdir) ;


/* exported variables */


/* exported subroutines */

int mktmpusers(char *rbuf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf) {
	    char	*dbuf{} ;
	    if ((rs = malloc_un(&dbuf)) >= 0) {
		static cchar	*tmpdir = strtmpdir ;
		cint		dlen = rs ;
		rs = SR_NOMEM ;
		if (tmpdir != nullptr) {
		    cchar	*users = sysword.w_users ;
		    if ((rs = mkpath(rbuf,tmpdir,users)) >= 0) {
			USTAT	sb ;
			rl = rs ;
			if ((rs = uc_stat(rbuf,&sb)) >= 0) {
			    if (! S_ISDIR(sb.st_mode)) {
				if ((rs = rmdirs(rbuf)) >= 0) {
			    	    rs = tmpusers(rbuf) ;
				} /* end if (rmdirs) */
			    } /* end if */
			} else if isNotPresent(rs)) {
			    rs = tmpusers(rbuf) ;
			} /* end if 
		    } /* end if (mkpath) */
		} /* end if (strtmpdir) */
		rs1 = uc_free(ubuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mktmpusers) */

int mktmpuserdir(char *rbuf,cchar *un,cchar *dname,mode_t m) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && un && dname) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (un[0] && dname[0]) {
	        char		unbuf[USERNAMELEN + 1] ;
	        if ((un == NULL) || (un[0] == '\0') || (un[0] == '-')) {
	            un = unbuf ;
	            rs = getusername(unbuf,USERNAMELEN,-1) ;
	        } /* end if */
	        if (rs >= 0) {
	            char	tmpuserdname[MAXPATHLEN + 1] ;
	            if ((rs = mktmpuser(tmpuserdname,un)) >= 0) {
	                if ((rs = mkpath2(rbuf,tmpuserdname,dname)) >= 0) {
		            USTAT	sb ;
	                    len = rs ;
	                    if ((rs = u_stat(rbuf,&sb)) >= 0) {
		                if (! S_ISDIR(sb.st_mode)) {
		                    rs = SR_NOTDIR ;
			        }
	                    } else {
	                        rs = u_mkdir(rbuf,m) ;
			    }
	                } /* end if (mkpath) */
	            } /* end if (mktmpuser) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mktmpuserdir) */


/* local subroutines */

static int mktmpuser(char *rbuf,cchar *un) noex {
	USTAT		sb ;
	cint		tmpuserdmode = TMPUSERDMODE ;
	int		rs ;
	cchar		*udname = TMPUSERDNAME ;
	cchar		*tdname = getenv(VARTMPDNAME) ;
	char		tmpuserdname[MAXPATHLEN + 1] ;

	if (tdname == NULL) tdname = TMPDNAME ;

	if ((rs = mkpath2(tmpuserdname,tdname,udname)) >= 0) {
	    if ((rs = u_stat(tmpuserdname,&sb)) == SR_NOEXIST) {
	        if ((rs = u_mkdir(tmpuserdname,tmpuserdmode)) >= 0) {
	            rs = ensuremode(tmpuserdname,tmpuserdmode) ;
	        }
	    }
	    if (rs >= 0) {
	        if ((rs = mkpath2(rbuf,tmpuserdname,un)) >= 0) {
	            if ((rs = u_stat(rbuf,&sb)) == SR_NOEXIST) {
	                rs = u_mkdir(rbuf,S_IAMB) ;
	            }
	        } /* end if (mkpath) */
	    } /* end if (ok) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (mktmpuser) */

static int ensuremode(cchar *rbuf,mode_t nm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rbuf[0]) {
	        if ((rs = u_open(rbuf,O_RDONLY,0666)) >= 0) {
	            USTAT	sb ;
	            cint	fd = rs ;
	            if ((rs = u_fstat(fd,&sb)) >= 0) {
	                mode_t	cm = sb.st_mode & (~ S_IFMT) ;
	                nm &= (~ S_IFMT) ;
	                if ((cm & nm) != nm) {
		            f = true ;
	                    nm |= cm ;
		            rs = u_fchmod(fd,nm) ;
	                }
	            } /* end if (stat) */
	            rs1 = u_close(fd) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (open-close) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ensuremode) */


