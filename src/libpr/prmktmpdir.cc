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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<mkdirs.h>
#include	<isnot.h>		/* |isNotPresent(3uc)| */
#include	<localmisc.h>

#include	"prmktmpdir.h"

import libutil ;

/* local defines */

#define	SI		subinfo

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPDMODE
#define	TMPDMODE	S_IAMB
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	cchar		*pr ;
	uid_t		euid, puid, tuid ;
	gid_t		egid, pgid, tgid ;
} ;


/* forward references */

static int	subinfo_start(SI *,cchar *) noex ;
static int	subinfo_finish(SI *) noex ;
static int	subinfo_mkprtmp(SI *,char *,cchar *) noex ;
static int	subinfo_chown(SI *,cchar *) noex ;
static int	subinfo_ckmode(SI *,cchar *,ustat *,mode_t) noex ;

static int	ensureattr(cchar *,mode_t,uid_t,gid_t) noex ;


/* local variables */

constexpr uid_t		uidend = -1 ;
constexpr gid_t		gidend = -1 ;


/* exported variables */


/* exported subroutines */

int prmktmpdir(cchar *pr,char *rbuf,cc *tmpdname,cc *dname,mode_t m) noex {
	subinfo		si, *sip = &si ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	int		f_prtmp = false ;
	cmode		dm = TMPDMODE ;
	char		prtmpdname[MAXPATHLEN + 1] ;

	if (pr == nullptr) return SR_FAULT ;

	if (rbuf == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	rbuf[0] = '\0' ;
	prtmpdname[0] = '\0' ;

	if (tmpdname == nullptr) tmpdname = getenv(VARTMPDNAME) ;
	if (tmpdname == nullptr) tmpdname = TMPDNAME ;

	if ((rs = subinfo_start(sip,pr)) >= 0) {

/* phase one */

	    if ((rs = subinfo_mkprtmp(sip,prtmpdname,tmpdname)) >= 0) {
	        f_prtmp = (rs > 0) ;

/* phase two */

	        if ((dname != nullptr) && (dname[0] != '\0')) {
	            ustat	sb ;
	            if ((rs = mkpath2(rbuf,prtmpdname,dname)) >= 0) {
	                len = rs ;
	                if ((rs = u_stat(rbuf,&sb)) >= 0) {
	                    if (! S_ISDIR(sb.st_mode)) rs = SR_NOTDIR ;
	                    if (rs >= 0) rs = subinfo_ckmode(sip,rbuf,&sb,dm) ;
	                } else if (isNotPresent(rs)) {
	                    if ((rs = u_mkdir(rbuf,m)) >= 0) {
	                        uid_t	u = si.puid ;
	                        gid_t	g = si.pgid ;
	                        if (u == si.euid) u = uidend ;
	                        if (g == si.egid) g = gidend ;
	                        if ((u != uidend) || (g != gidend)) {
	                            rs = ensureattr(rbuf,m,u,g) ;
				}
	                    }
			} /* end if (stat) */
	            } /* end if (mkpath) */
	        } else {
	            len = lenstr(rbuf) ;
	 	}
	        if ((rs >= 0) && (! f_prtmp)) {
	            rs = subinfo_chown(sip,prtmpdname) ;
		}
	    } /* end if (prmktmp) */
	    rs1 = subinfo_finish(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (prmktmpdir) */


/* local subroutines */

static int subinfo_start(SI *sip,cchar *pr) noex {
	int		rs = SR_FAULT ;
	if (sip && pr) ylikely {
	    memclear(sip) ;
	    sip->pr = pr ;
	    sip->euid = geteuid() ;
	    sip->egid = getegid() ;
	    if (ustat prsb ; (rs = u_stat(pr,&prsb)) >= 0) ylikely {
	        sip->puid = prsb.st_uid ;
	        sip->pgid = prsb.st_gid ;
	        sip->tuid = -1 ;
	        sip->tgid = -1 ;
	    } /* end if (u_stat) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) ylikely {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_mkprtmp(SI *sip,char *prtmpdname,cc *tmpdname) noex {
	int		rs ;
	int		f_create = false ;
	cchar		*rn{} ;
	if (int rl ; (rl = sfbasename(sip->pr,-1,&rn)) > 0) ylikely {
	    if ((rs = mkpath2w(prtmpdname,tmpdname,rn,rl)) >= 0) ylikely {
		cmode		dm = (TMPDMODE | S_ISVTX) ;
		if (ustat sb ; (rs = u_stat(prtmpdname,&sb)) >= 0) ylikely {
	            sip->tuid = sb.st_uid ;
	            sip->tgid = sb.st_gid ;
	            if (S_ISDIR(sb.st_mode)) {
			rs = subinfo_ckmode(sip,prtmpdname,&sb,dm) ;
		    } else {
			rs = SR_NOTDIR ;
		    }
	        } else if (isNotPresent(rs)) {
	            f_create = true ;
	            if ((rs = mkdirs(prtmpdname,dm)) >= 0) {
	                sip->tuid = sip->puid ;
	                sip->tgid = sip->pgid ;
	                rs = ensureattr(prtmpdname,dm,sip->puid,sip->pgid) ;
	            }
		} /* end if (stat) */
	    } /* end if (mkpath2) */
	} else {
	    rs = SR_NOENT ;
	}
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (subinfo_mkprtmp) */

static int subinfo_chown(SI *sip,cchar *prtmpdname) noex {
	uid_t		u = sip->puid ;
	gid_t		g = sip->pgid ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->tuid == sip->euid) {
	    if ((u != sip->euid) || (g != sip->egid)) {
	        rs1 = u_chown(prtmpdname,u,g) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (we own it) */
	return rs ;
}
/* end subroutine (subinfo_chown) */

static int subinfo_ckmode(SI *sip,cc *dname,ustat *sbp,mode_t dm) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if ((sbp->st_uid == sip->euid) && ((sbp->st_mode & dm) != dm)) {
	    cmode	ndm = ((sbp->st_mode & (~ S_IFMT)) | dm) ;
	    f = true ;
	    rs = u_chmod(dname,ndm) ;
	} /* end if */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_ckmode) */

static int ensureattr(cchar *tmpdname,mode_t nm,uid_t puid,gid_t pgid) noex {
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
}
/* end subroutine (ensureattr) */


