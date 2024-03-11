/* prmktmpdir SUPPORT */
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
#include	<localmisc.h>

#include	"prmktmpdir.h"


/* local defines */

#define	SUBINFO		struct subinfo

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	2048
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

static int	subinfo_start(SUBINFO *,cchar *) ;
static int	subinfo_finish(SUBINFO *) ;
static int	subinfo_mkprtmp(SUBINFO *,char *,cchar *) ;
static int	subinfo_chown(SUBINFO *,cchar *) ;
static int	subinfo_ckmode(SUBINFO *,cchar *,USTAT *,mode_t) ;

static int	ensureattr(cchar *,mode_t,uid_t,gid_t) ;


/* local variables */


/* exported variables */


/* exported subroutines */

int prmktmpdir(cchar *pr,char *rbuf,cc *tmpdname,cc *dname,mode_t m) noex {
	SUBINFO		si, *sip = &si ;
	cmode		dm = TMPDMODE ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	int		f_prtmp = FALSE ;
	char		prtmpdname[MAXPATHLEN + 1] ;

	if (pr == NULL) return SR_FAULT ;

	if (rbuf == NULL) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	rbuf[0] = '\0' ;
	prtmpdname[0] = '\0' ;

	if (tmpdname == NULL) tmpdname = getenv(VARTMPDNAME) ;
	if (tmpdname == NULL) tmpdname = TMPDNAME ;

	if ((rs = subinfo_start(sip,pr)) >= 0) {

/* phase one */

	    if ((rs = subinfo_mkprtmp(sip,prtmpdname,tmpdname)) >= 0) {
	        f_prtmp = (rs > 0) ;

/* phase two */

	        if ((dname != NULL) && (dname[0] != '\0')) {
	            USTAT	sb ;

	            if ((rs = mkpath2(rbuf,prtmpdname,dname)) >= 0) {
	                len = rs ;

	                rs1 = u_stat(rbuf,&sb) ;
	                if (rs1 >= 0) {
	                    if (! S_ISDIR(sb.st_mode)) rs = SR_NOTDIR ;
	                    if (rs >= 0) rs = subinfo_ckmode(sip,rbuf,&sb,dm) ;
	                } else if (rs1 == SR_NOEXIST) {
	                    if ((rs = u_mkdir(rbuf,m)) >= 0) {
	                        uid_t	u = si.puid ;
	                        gid_t	g = si.pgid ;
	                        if (u == si.euid) u = -1 ;
	                        if (g == si.egid) g = -1 ;
	                        if ((u >= 0) || (g >= 0))
	                            rs = ensureattr(rbuf,m,u,g) ;
	                    }
	                } else
	                    rs = rs1 ;

	            } /* end if (mkpath) */

	        } else {
	            len = strlen(rbuf) ;
	 	}

	        if ((rs >= 0) && (! f_prtmp)) {
	            rs = subinfo_chown(sip,prtmpdname) ;
		}

	    } /* end if (prmktmp) */

	    subinfo_finish(sip) ;
	} /* end if (subinfo) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (prmktmpdir) */


/* local subroutines */


static int subinfo_start(SUBINFO *sip,cchar *pr)
{
	USTAT	prsb ;
	int		rs ;

	memset(sip,0,sizeof(SUBINFO)) ;
	sip->pr = pr ;
	sip->euid = geteuid() ;
	sip->egid = getegid() ;

	rs = u_stat(pr,&prsb) ;
	sip->puid = prsb.st_uid ;
	sip->pgid = prsb.st_gid ;

	sip->tuid = -1 ;
	sip->tgid = -1 ;

	return rs ;
}
/* end subroutine (subinfo_start) */


static int subinfo_finish(SUBINFO *sip)
{

	if (sip == NULL) return SR_FAULT ;

	return SR_OK ;
}
/* end subroutine (subinfo_finish) */


static int subinfo_mkprtmp(SUBINFO *sip,char *prtmpdname,cchar *tmpdname)
{
	USTAT	sb ;
	cmode	dm = (TMPDMODE | S_ISVTX) ;
	int		rs ;
	int		rs1 ;
	int		rl ;
	int		f_create = FALSE ;
	cchar		*rn ;

	if ((rl = sfbasename(sip->pr,-1,&rn)) > 0) {
	    if ((rs = mkpath2w(prtmpdname,tmpdname,rn,rl)) >= 0) {
	        if ((rs1 = u_stat(prtmpdname,&sb)) >= 0) {
	            sip->tuid = sb.st_uid ;
	            sip->tgid = sb.st_gid ;
	            if (! S_ISDIR(sb.st_mode)) rs = SR_NOTDIR ;
	            if (rs >= 0) rs = subinfo_ckmode(sip,prtmpdname,&sb,dm) ;
	        } else if (rs1 == SR_NOEXIST) {
	            f_create = TRUE ;
	            rs = mkdirs(prtmpdname,dm) ;
	            if (rs >= 0) {
	                sip->tuid = sip->puid ;
	                sip->tgid = sip->pgid ;
	                rs = ensureattr(prtmpdname,dm,sip->puid,sip->pgid) ;
	            }
	        } else
	            rs = rs1 ;
	    } /* end if (mkpath2) */
	} else
	    rs = SR_NOENT ;

	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (subinfo_mkprtmp) */


static int subinfo_chown(SUBINFO *sip,cchar *prtmpdname)
{
	uid_t	u = sip->puid ;
	gid_t	g = sip->pgid ;
	int	rs = SR_OK ;
	int	rs1 ;

	if (sip->tuid == sip->euid) {

	    if (u == sip->euid) u = -1 ;
	    if (g == sip->egid) g = -1 ;

	    if ((u >= 0) || (g >= 0)) {
	        rs1 = u_chown(prtmpdname,u,g) ;
	        if (rs >= 0) rs = rs1 ;
	    }

	} /* end if (we own it) */

	return rs ;
}
/* end subroutine (subinfo_chown) */


static int subinfo_ckmode(SUBINFO *sip,cchar *dname,USTAT *sbp,mode_t dm)
{
	int		rs = SR_OK ;
	int		f = FALSE ;

	if ((sbp->st_uid == sip->euid) && ((sbp->st_mode & dm) != dm)) {
	    cmode	ndm = ((sbp->st_mode & (~ S_IFMT)) | dm) ;
	    f = TRUE ;
	    rs = u_chmod(dname,ndm) ;
	} /* end if */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_ckmode) */


static int ensureattr(cchar *tmpdname,mode_t nm,uid_t puid,gid_t pgid)
{
	int		rs ;
	int		f = FALSE ;

	if (tmpdname == NULL) return SR_FAULT ;

	if (tmpdname[0] == '\0') return SR_INVALID ;

	if ((rs = u_open(tmpdname,O_RDONLY,0666)) >= 0) {
	    USTAT	sb ;
	    cint	fd = rs ;

	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	        mode_t	cm = sb.st_mode & (~ S_IFMT) ;

	        nm &= (~ S_IFMT) ;
	        if ((cm & nm) != nm) {
	            f = TRUE ;
	            nm |= cm ;
	            rs = u_fchmod(fd,nm) ;
	        }

	        if ((rs >= 0) && ((puid >= 0) || (pgid >= 0))) {
	            if ((puid >= 0) && (puid == sb.st_uid)) puid = -1 ;
	            if ((pgid >= 0) && (pgid == sb.st_gid)) pgid = -1 ;
	            if ((puid >= 0) || (pgid >= 0)) {
	                rs = u_fchown(fd,puid,pgid) ;
		    }
	        }

	    } /* end if (stat) */

	    u_close(fd) ;
	} /* end if (file) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ensureattr) */


