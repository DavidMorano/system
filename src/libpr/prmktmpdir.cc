/* prmktmpdir SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a TMP-PR directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	prmktmpdir

	Description:
	This subroutine creates a PR-TMP directory.  For example, given:
	+ a PR named 'pcs' 
	+ a tmp-dir '/tmp'
	+ a directory name 'junker' to create
	the directory:
		/tmp/pcs/junker/
	will be created.

	Synopsis:
	int prmktmpdir(cc *pr,char *rbuf,cc *tmpdname,cc *dname,mode_t m) noex

	Arguments:
	pr		program-root
	rbuf		buffer to receive resulting created directory name
	tmpdname	TMPDIR to use
	dname		basename of directory to create
	m		directory creation mode

	Returns:
	>0		length of resulting directory name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<mkdirs.h>
#include	<strlibval.hh>
#include	<isnot.h>		/* |isNotPresent(3uc)| */
#include	<localmisc.h>

#include	"prmktmpdir.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#define	SI		subinfo

#ifndef	TMPDMODE
#define	TMPDMODE	S_IAMB
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mker {
	cchar		*pr ;
	cchar		*tmpdname ;
	cchar		*dname ;
	char		*rbuf ;
	cmode		dm = TMPDMODE ;
	mode_t		ªm ;
	mker(cc *p,char *r,cc *t,cc *d,mode_t µm) noex : pr(p) {
	    tmpdname = t ;
	    dname = d ;
	    rbuf = r ;
	    ªm = µm ;
	} ; /* end ctor */
	operator int () noex ;
	int wrap	() noex ;
	int suber	(char *,int) noex ;
    } ; /* end struct (mker) */
    struct subinfo {
	cchar		*pr ;
	uid_t		euid, puid, tuid ;
	gid_t		egid, pgid, tgid ;
	mode_t		ªm ;
	subinfo(cchar *p,mode_t µm) noex : pr(p), ªm(µm) { } ;
	int start	() noex ;
	int finish	() noex ;
	int mkprtmp	(char *,cchar *) noex ;
	int mkext	(char *,char *,cchar *) noex ;
	int chown	(cchar *) noex ;
	int ckmode	(cchar *,ustat *,mode_t) noex ;
    } ; /* end struct (subinfo) */
} /* end namespace */


/* forward references */

local int	ensureattr(cchar *,mode_t,uid_t,gid_t) noex ;


/* local variables */

constexpr uid_t		uidend = -1 ;
constexpr gid_t		gidend = -1 ;

constexpr mode_t	mmask = 07777 ;

static strlibval	vtmpdir(strlibval_tmpdir) ;


/* exported variables */


/* exported subroutines */

int prmktmpdir(cchar *pr,char *rbuf,cc *tmpdname,cc *dname,mode_t m) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (pr[0]) {
		if (mker mo(pr,rbuf,tmpdname,dname,m) ; (rs = mo) >= 0) {
		    len = rs ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (prmktmpdir) */

/* local subroutines */

mker::operator int () noex {
	int		rs = SR_INVALID ;
	if ((ªm & (~ mmask)) == 0) {
	    if (tmpdname == nullptr) tmpdname = vtmpdir ;
	    if (tmpdname == nullptr) tmpdname = sysword.w_tmpdir ;
	    rs = wrap() ;
	} /* end if (valid) */
	return rs ;
} /* end method (mker::operator) */

int mker::wrap() noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (char *dbuf ; (rs = lm_mp(&dbuf)) >= 0) {
	    cint dlen = rs ;
	    {
		dbuf[0] = '\0' ;
		rs = suber(dbuf,dlen) ;
		len = rs ;
	    }
	    rs1 = lm_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end method (mker::wrap) */

int mker::suber(char *dbuf,int) noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (subinfo si(pr,ªm) ; (rs = si.start()) >= 0) {
	    /* phase one */
	    if ((rs = si.mkprtmp(dbuf,tmpdname)) >= 0) {
	        cbool f_prtmp = (rs > 0) ;
		/* phase two */
	        if ((dname != nullptr) && (dname[0] != '\0')) {
		    rs = si.mkext(rbuf,dbuf,dname) ;
		    len = rs ;
	        } else {
		    rs = mkpath(rbuf,dbuf) ;
	            len = rs ;
	 	} /* end if */
	        if ((rs >= 0) && (! f_prtmp)) {
	            rs = si.chown(dbuf) ;
		}
	    } /* end if (prmktmp) */
	    rs1 = si.finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */
	return (rs >= 0) ? len : rs ;
} /* end method (mker::suber) */

int subinfo::start() noex {
	int		rs = SR_FAULT ;
	if (pr) ylikely {
	    euid = geteuid() ;
	    egid = getegid() ;
	    if (ustat prsb ; (rs = u_stat(pr,&prsb)) >= 0) ylikely {
	        puid = prsb.st_uid ;
	        pgid = prsb.st_gid ;
	        tuid = -1 ;
	        tgid = -1 ;
	    } /* end if (u_stat) */
	} /* end if (non-null) */
	return rs ;
} /* end method (subinfo::start) */

int subinfo::finish() noex {
    	return SR_OK ;
} /* end method (subinfo::finish) */

int subinfo::mkprtmp(char *dbuf,cc *tmpdname) noex {
	int		rs ;
	int		f_create = false ; /* return-value */
	cchar		*rn{} ;
	if (int rl ; (rl = sfbasename(pr,-1,&rn)) > 0) ylikely {
	    if ((rs = mkpath2w(dbuf,tmpdname,rn,rl)) >= 0) ylikely {
		cmode	dm = (TMPDMODE | S_ISVTX) ;
		if (ustat sb ; (rs = u_stat(dbuf,&sb)) >= 0) ylikely {
	            tuid = sb.st_uid ;
	            tgid = sb.st_gid ;
	            if (S_ISDIR(sb.st_mode)) {
			rs = ckmode(dbuf,&sb,dm) ;
		    } else {
			rs = SR_NOTDIR ;
		    }
	        } else if (isNotPresent(rs)) {
	            f_create = true ;
	            if ((rs = mkdirs(dbuf,dm)) >= 0) {
	                tuid = puid ;
	                tgid = pgid ;
	                rs = ensureattr(dbuf,dm,puid,pgid) ;
	            }
		} /* end if (stat) */
	    } /* end if (mkpath2) */
	} else {
	    rs = SR_NOENT ;
	}
	return (rs >= 0) ? f_create : rs ;
} /* end method (subinfo::mkprtmp) */

int subinfo::mkext(char *rbuf,char *dbuf,cchar *dname) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	cmode		dm = TMPDMODE ;
        if ((rs = mkpath2(rbuf,dbuf,dname)) >= 0) {
            len = rs ;
            if (ustat sb ; (rs = u_stat(rbuf,&sb)) >= 0) {
                rs = SR_NOTDIR ;
                if (S_ISDIR(sb.st_mode)) {
                    rs = ckmode(rbuf,&sb,dm) ;
                }
            } else if (isNotPresent(rs)) {
                if ((rs = u_mkdir(rbuf,ªm)) >= 0) {
                    uid_t   u = puid ;
                    gid_t   g = pgid ;
                    if (u == euid) u = uidend ;
                    if (g == egid) g = gidend ;
                    if ((u != uidend) || (g != gidend)) {
                        rs = ensureattr(rbuf,ªm,u,g) ;
                    }
                } /* end if (u_mkdir) */
            } /* end if (stat) */
        } /* end if (mkpath) */
	return (rs >= 0) ? len : rs ;
} /* end method (subinfo::mkext) */

int subinfo::chown(cchar *dbuf) noex {
	const uid_t	u = puid ;
	const gid_t	g = pgid ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (tuid == euid) {
	    if ((u != euid) || (g != egid)) {
	        rs1 = u_chown(dbuf,u,g) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (we own it) */
	return rs ;
} /* end method (subinfo::chown) */

int subinfo::ckmode(cc *dname,ustat *sbp,mode_t dm) noex {
	int		rs = SR_OK ;
	int		f = false ; /* return-value */
	if ((sbp->st_uid == euid) && ((sbp->st_mode & dm) != dm)) {
	    cmode	ndm = ((sbp->st_mode & (~ S_IFMT)) | dm) ;
	    f = true ;
	    rs = u_chmod(dname,ndm) ;
	} /* end if */
	return (rs >= 0) ? f : rs ;
} /* end method (subinfo::ckmode) */

local int ensureattr(cchar *tmpdname,mode_t nm,uid_t puid,gid_t pgid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (tmpdname) ylikely {
	    rs = SR_INVALID ;
	    if (tmpdname[0]) ylikely {
	        if ((rs = u_open(tmpdname,O_RDONLY,0666)) >= 0) ylikely {
	            cint	fd = rs ;
	            if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
	                mode_t	cm = sb.st_mode & (~ S_IFMT) ;
	                nm &= (~ S_IFMT) ;
	                if ((cm & nm) != nm) {
	                    f = true ;
	                    nm |= cm ;
	                    rs = u_fchmod(fd,nm) ;
	                }
	                if (rs >= 0) ylikely {
			    if ((puid != uidend) || (pgid != gidend)) {
	                        if ((puid != uidend) && (puid == sb.st_uid)) {
				    puid = uidend ;
			        }
	                        if ((pgid != gidend) && (pgid == sb.st_gid)) {
				    pgid = gidend ;
			        }
	                        if ((puid != uidend) || (pgid != gidend)) {
	                            rs = u_fchown(fd,puid,pgid) ;
		                }
			    } /* end if */
	                } /* end if (ok) */
	            } /* end if (stat) */
	            rs1 = u_close(fd) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (file) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ensureattr) */


