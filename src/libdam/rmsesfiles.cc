/* rmsesfiles SUPPORT */
/* lang=C++20 */

/* remove session files and session directories (as appropriate) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

	= 2018-07-02 David A.D. Morano
	Enhanced for slightly better error checking on PID values.

*/

/* Copyright (c) 1998,2018 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rmsesfiles

	Description:
	Remove files from a specified directory that are not longer
	needed.

	Synopsis:
	int rmsesfiles(cchar *dname) noex

	Arguments:
	dname		directory name (as a c-string)

	Returns:
	<0		error
	>=0		number of files removed (deleted)


	Notes:
	If a lock file is left by a previous process that was killed,
	we try to "adopt" it so that after we are done, we delete
	it as the original process that created it was supposed to
	do.  In this way, the directory is never permanently locked.
	Our real "lock" is not the existence of the the lock file
	itself, but rather our record lock on the lock file.  Record
	locks are automatically deleted with the file-descriptor
	holding the record lock is closed.  So a process that is
	killed while holding the record lock, implicitly deletes
	the record lock on its death.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sigblocker.h>
#include	<cfdec.h>
#include	<ids.h>
#include	<fsdir.h>
#include	<vecpstr.h>
#include	<xperm.h>
#include	<pathadd.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<strnul.hh>
#include	<mkchar.h>
#include	<hasx.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"rmsesfiles.h"


/* local defines */

#define	RMSESFILES_LOCKFILE	"LOCK"
#define	RMSESFILES_NENT	512


/* external subroutines */

extern "C" {
    extern int	removes(cchar *) noex ;
}


/* external variables */


/* external variables */


/* local structures */

struct openstate {
	uint		f_created ;
} ;


/* forward references */

static int	rmsesfiler(ids *,char *,cchar *) noex ;

static int	lockbegin(char *,int) noex ;
static int	lockend(char *,int,int) noex ;

static int	rmsesdir(ids *,char *,int) noex ;
static int	rmdirfiles(char *,int) noex ;

static int	vecpstr_dirload(vecpstr *,char *,int) noex ;
static int	vecpstr_dirdel(vecpstr *,char *,int) noex ;

static int	openstate_open(openstate *,cchar *,int,mode_t) noex ;
static int	rmfile(cchar *) noex ;

static bool	isLocked(int) noex ;
static bool	isNotRunning(cchar *,int) noex ;


/* local variables */

constexpr int		rslocked[] = {
	SR_EXISTS,
	SR_AGAIN,
	SR_ACCESS,
	0
} ;


/* exported variables */


/* exported subroutines */

int rmsesfiles(cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (dname) {
	    rs = SR_INVALID ;
	    if (dname[0]) {
	        if (ids id ; (rs = ids_load(&id)) >= 0) {
		    if (char *pbuf{} ; (rs = malloc_mp(&pbuf)) >= 0) {
		        {
		            rs = rmsesfiler(&id,pbuf,dname) ;
			    c = rs ;
		        }
		        rs = rsfree(rs,pbuf) ;
		    } /* end if (m-a-f) */
		    rs1 = ids_release(&id) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (ids) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (rmsesfiles) */


/* local subroutines */

static int rmsesfiler(ids *idp,char *pbuf,cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mkpath(pbuf,dname)) >= 0) {
	    cint	pl = rs ;
	    if (char *nbuf{} ; (rs = malloc_mn(&nbuf)) >= 0) {
	        sigblocker	s ;
		cint		nlen = rs ;
	        if ((rs = s.start) >= 0) {
	            if ((rs = lockbegin(pbuf,pl)) >= 0) {
	                cint		lfd = rs ;
	                fsdir		d ;
	                fsdir_ent	de ;
	                if ((rs = fsdir_open(&d,pbuf)) >= 0) {
	                    while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
	                        cchar	*sp = de.name ;
	                        if (hasNotDots(sp,rs) && (sp[0] == 's')) {
	                            if ((rs = pathadd(pbuf,pl,sp)) >= 0) {
	                                rs = rmsesdir(idp,pbuf,rs) ;
	                                c += rs ;
	                            } /* end if (pathadd) */
	                        } /* end (not dots) */
	                        if (rs < 0) break ;
	                    } /* end while */
	                    rs1 = fsdir_close(&d) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (fsdir) */
	                pbuf[pl] = '\0' ;
	                rs1 = lockend(pbuf,pl,lfd) ;
	                if (rs >= 0) rs = rs1 ;
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            } else if (isLocked(rs)) {
	                rs = SR_OK ;
	            } /* end if (lockend) */
	            rs1 = s.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblocker) */
		rs = rsfree(rs,nbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (mkpath) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (rmsesfiler) */

static int lockbegin(char *pbuf,int plen) noex {
	int		rs ;
	int		lfd = -1 ;
	cchar		*lfn = RMSESFILES_LOCKFILE ;
	if ((rs = pathadd(pbuf,plen,lfn)) >= 0) {
	    openstate	ols ;
	    cmode	om = 0666 ;
	    cint	of = (O_CREAT|O_RDWR|O_TRUNC) ;
	    if ((rs = openstate_open(&ols,pbuf,of,om)) >= 0) {
	        lfd = rs ;
	        if (ols.f_created) {
	            rs = u_fchmod(lfd,om) ;
	        }
	        if (rs >= 0) {
	            rs = uc_lockf(lfd,F_TWLOCK,0) ;
	        }
	        if (rs < 0) {
	            if (ols.f_created) {
	                u_unlink(pbuf) ;
	            }
	            u_close(lfd) ;
	        }
	    } /* end if (open) */
	    pbuf[plen] = '\0' ; /* <- restore */
	} /* end if (pathadd) */
	return (rs >= 0) ? lfd : rs ;
}
/* end subroutine (lockbegin) */

static int lockend(char *pbuf,int plen,int lfd) noex {
	int		rs ;
	cchar		*lfn = RMSESFILES_LOCKFILE ;
	if ((rs = pathadd(pbuf,plen,lfn)) >= 0) {
	    u_unlink(pbuf) ;
	    u_close(lfd) ;
	}
	return rs ;
}
/* end subroutine (lockend) */

static int rmsesdir(ids *idp,char *pbuf,int plen) noex {
	USTAT		sb ;
	int		rs ;
	int		c = 0 ;
	if ((rs = u_stat(pbuf,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        cint	am = (R_OK|W_OK|X_OK) ;
	        if ((rs = sperm(idp,&sb,am)) >= 0) {
	            if ((rs = rmdirfiles(pbuf,plen)) > 0) {
	                cchar	*cp{} ;
	                c += rs ;
	                if (int cl ; (cl = sfbasename(pbuf,plen,&cp)) > 0) {
	                    if ((rs = isNotRunning(cp,cl)) > 0) {
	                        rs = removes(pbuf) ;
	                    }
	                }
	            } /* end if (rmdirfiles) */
	        } else if (isNotAccess(rs)) {
	            rs = SR_OK ;
	        }
	    }
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	} /* end if (stat) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (rmsesdir) */

static int rmdirfiles(char *pbuf,int plen) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if (pbuf) {
	    rs = SR_INVALID ;
	    if (pbuf[0]) {
	        vecpstr		files ;
	        cint		vn = RMSESFILES_NENT ;
		cint		vo = 0 ;
	        if ((rs = vecpstr_start(&files,0,vn,vo)) >= 0) {
	            int		c = 0 ;
	            if ((rs = vecpstr_dirload(&files,pbuf,plen)) > 0) {
	                c = rs ;
	                if ((rs = vecpstr_dirdel(&files,pbuf,plen)) >= 0) {
	                    if (c == rs) rc = (rs+1) ;
	                }
	            } else if (rs == 0) {
	                rc = 1 ;
	            }
	            rs1 = vecpstr_finish(&files) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (files) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (rmdirfiles) */

static int vecpstr_dirload(vecpstr *flp,char *pbuf,int plen) noex {
	int		rs = SR_NOMEM ;
	int		rs1 ;
	int		c = 0 ;
	strnul		dname(pbuf,plen) ;
	if (cchar *dn = dname ; dn != nullptr) {
	    if (char *nbuf{} ; (rs = malloc_mn(&nbuf)) >= 0) {
	        fsdir	dir ;
		cint	nlen = rs ;
	        if ((rs = fsdir_open(&dir,dn)) >= 0) {
	            fsdir_ent	de ;
	            while ((rs = fsdir_read(&dir,&de,nbuf,nlen)) > 0) {
	                cint	sl = rs ;
	                cchar	*sp = de.name ;
	                if (hasNotDots(sp,sl)) {
	                    c += 1 ;
	                    rs = vecpstr_add(flp,sp,sl) ;
	                } /* end if (hasNotDots) */
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = fsdir_close(&dir) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (fsdir) */
		rs = rsfree(rs,nbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (strnul) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_dirload) */

static int vecpstr_dirdel(vecpstr *flp,char *pbuf,int plen) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*sp ;
	for (int i = 0 ; vecpstr_get(flp,i,&sp) >= 0 ; i += 1) {
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        if ((rs = pathadd(pbuf,plen,sp)) >= 0) {
	            if (sp[0] == 'p') {
	                if ((rs = isNotRunning(sp,-1)) > 0) {
	                    rs = rmfile(pbuf) ;
	                    c += rs ;
	                } /* end if (isNotRunning) */
	            } else {
	                rs = rmfile(pbuf) ;
	                c += rs ;
	            } /* end if (type) */
	        } /* end if (pathadd) */
	    }
	    if (rs < 0) break ;
	} /* end for */
	pbuf[plen] = '\0' ; /* restore */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_dirdel) */

static int openstate_open(openstate *lsp,cchar *fn,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (lsp && fn) {
	    rs = SR_INVALID ;
	    memclear(lsp) ;
	    if (fn[0]) {
	        if ((rs = u_open(fn,(of|O_EXCL),om)) >= 0) {
	            lsp->f_created = true ;
	        } else if ((!(of&O_EXCL)) && (rs == SR_EXISTS)) {
	            rs = u_open(fn,of,om) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (openstate_open) */

static int rmfile(cchar *fn) noex {
	int		rs ;
	if ((rs = u_unlink(fn)) >= 0) {
	    rs = 1 ;
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (rmfile) */

static bool isLocked(int rs) noex {
	return isOneOf(rslocked,rs) ;
}
/* end subroutine (isLocked) */

static bool isNotRunning(cchar *sp,int sl) noex {
	cint		sch = mkchar(sp[0]) ;
	int		rs = SR_OK ;
	int		f = false ;
	if (sl < 0) sl = strlen(sp) ;
	if ((sl > 1) && ((sch == 'p') || (sch == 's'))) {
	    sp += 1 ;
	    sl -= 1 ;
	    if (int v{} ; (rs = cfdeci(sp,sl,&v)) >= 0) {
	        const pid_t	pid = v ;
	        if ((rs = uc_prochave(pid)) == 0) {
	            f = true ;
	        }
	    } else if (isNotValid(rs)) {
	        f = true ;
	        rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isNotRunning) */


