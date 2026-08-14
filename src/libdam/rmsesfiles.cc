/* rmsesfiles SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove session files and session directories (as appropriate) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A-D- Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

	= 2018-07-02 David A-D- Morano
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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<sigblocker.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC |uc_lockf(3uc)| */
#include	<ucprochave.h>		/* LIBUC |uc_prochave(3uc)| */
#include	<cfdec.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<fsdir.h>		/* LIBUC */
#include	<vecpstr.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<removes.h>		/* LIBUC */
#include	<pathadd.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<isoneof.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<strnul.hh>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"rmsesfiles.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	RMSESFILES_LOCKFILE	"LOCK"
#define	RMSESFILES_NENT		512


/* external subroutines */


/* external variables */


/* external variables */


/* local structures */

struct openstate {
	uint		f_created ;
} ; /* end struct */


/* forward references */

local int	rmsesfiler(ids *,char *,cchar *) noex ;

local int	lockbegin(char *,int) noex ;
local int	lockend(char *,int,int) noex ;

local int	rmsesdir(ids *,char *,int) noex ;
local int	rmdirfiles(char *,int) noex ;

local int	vecpstr_dirload(vecpstr *,char *,int) noex ;
local int	vecpstr_dirdel(vecpstr *,char *,int) noex ;

local int	openstate_open(openstate *,cchar *,int,mode_t) noex ;
local int	rmfile(cchar *) noex ;
local int	NotRunning(cchar *,int) noex ;

local bool	isLocked(int) noex ;


/* local variables */

constexpr int		rslocked[] = {
	SR_EXISTS,
	SR_AGAIN,
	SR_ACCESS,
	0
} ; /* end array */


/* exported variables */


/* exported subroutines */

int rmsesfiles(cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (dname) ylikely {
	    rs = SR_INVALID ;
	    if (dname[0]) ylikely {
	        if (ids id ; (rs = ids_load(&id)) >= 0) ylikely {
		    if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) ylikely {
		        {
		            rs = rmsesfiler(&id,pbuf,dname) ;
			    c = rs ;
		        }
		        rs1 = lm_free(pbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		    rs1 = ids_release(&id) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (ids) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (rmsesfiles) */


/* local subroutines */

local int rmsesfiler(ids *idp,char *pbuf,cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mkpath(pbuf,dname)) >= 0) ylikely {
	    cint	pl = rs ;
	    if (char *nbuf ; (rs = lm_mn(&nbuf)) >= 0) ylikely {
		cint	nlen = rs ;
	        if (sigblocker s ; (rs = s.start) >= 0) ylikely {
	            if ((rs = lockbegin(pbuf,pl)) >= 0) ylikely {
	                cint	lfd = rs ;
	                if (fsdir d ; (rs = d.open(pbuf)) >= 0) ylikely {
	                    fsdir_ent	de ;
	                    while ((rs = d.read(&de,nbuf,nlen)) > 0) {
	                        cchar	*sp = de.name ;
	                        if (hasNotDots(sp,rs) && (sp[0] == 's')) {
	                            if ((rs = pathadd(pbuf,pl,sp)) >= 0) {
	                                rs = rmsesdir(idp,pbuf,rs) ;
	                                c += rs ;
	                            } /* end if (pathadd) */
	                        } /* end (not dots) */
	                        if (rs < 0) break ;
	                    } /* end while */
	                    rs1 = d.close ;
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
		rs1 = lm_free(nbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (mkpath) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (rmsesfiler) */

local int lockbegin(char *pbuf,int plen) noex {
	int		rs ;
	int		lfd = -1 ;
	cchar		*lfn = RMSESFILES_LOCKFILE ;
	if ((rs = pathadd(pbuf,plen,lfn)) >= 0) ylikely {
	    cint	of = (O_CREAT|O_RDWR|O_TRUNC) ;
	    cmode	om = 0666 ;
	    if (openstate ols ; (rs = openstate_open(&ols,pbuf,of,om)) >= 0) {
	        lfd = rs ;
	        if (ols.f_created) {
	            rs = u_fchmod(lfd,om) ;
	        }
	        if (rs >= 0) ylikely {
	            rs = uc_lockf(lfd,F_TWLOCK,0) ;
	        }
	        if (rs < 0) {
	            if (ols.f_created) {
	                u_unlink(pbuf) ;
	            }
	            u_close(lfd) ;
	        } /* end if (error) */
	    } /* end if (open) */
	    pbuf[plen] = '\0' ; /* <- restore */
	} /* end if (pathadd) */
	return (rs >= 0) ? lfd : rs ;
} /* end subroutine (lockbegin) */

local int lockend(char *pbuf,int plen,int lfd) noex {
	int		rs ;
	cchar		*lfn = RMSESFILES_LOCKFILE ;
	if ((rs = pathadd(pbuf,plen,lfn)) >= 0) ylikely {
	    u_unlink(pbuf) ;
	    u_close(lfd) ;
	}
	return rs ;
} /* end subroutine (lockend) */

local int rmsesdir(ids *idp,char *pbuf,int plen) noex {
	int		rs ;
	int		c = 0 ;
	if (ustat sb ; (rs = u_stat(pbuf,&sb)) >= 0) ylikely {
	    if (S_ISDIR(sb.st_mode)) {
	        cint	am = (R_OK|W_OK|X_OK) ;
	        if ((rs = permids(idp,&sb,am)) >= 0) ylikely {
	            if ((rs = rmdirfiles(pbuf,plen)) > 0) ylikely {
	                cchar	*cp{} ;
	                c += rs ;
	                if (int cl ; (cl = sfbasename(pbuf,plen,&cp)) > 0) {
	                    if ((rs = NotRunning(cp,cl)) > 0) {
	                        rs = removes(pbuf) ;
	                    }
	                }
	            } /* end if (rmdirfiles) */
	        } else if (isNotAccess(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (is-dir) */
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	} /* end if (stat) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (rmsesdir) */

local int rmdirfiles(char *pbuf,int plen) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if (pbuf) ylikely {
	    rs = SR_INVALID ;
	    if (pbuf[0]) ylikely {
	        cint	vn = RMSESFILES_NENT ;
		cint	vo = 0 ;
	        if (vecpstr files ; (rs = files.start(0,vn,vo)) >= 0) ylikely {
	            int		c = 0 ;
	            if ((rs = vecpstr_dirload(&files,pbuf,plen)) > 0) ylikely {
	                c = rs ;
	                if ((rs = vecpstr_dirdel(&files,pbuf,plen)) >= 0) {
	                    if (c == rs) rc = (rs+1) ;
	                }
	            } else if (rs == 0) {
	                rc = +1 ;
	            }
	            rs1 = files.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (files) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (rmdirfiles) */

local int vecpstr_dirload(vecpstr *flp,char *pbuf,int plen) noex {
	strnul		dname(pbuf,plen) ;
	int		rs = SR_NOMEM ;
	int		rs1 ;
	int		c = 0 ;
	if (cchar *dn = dname ; dn) ylikely {
	    if (char *nbuf ; (rs = lm_mn(&nbuf)) >= 0) ylikely {
		cint	nlen = rs ;
	        if (fsdir dir ; (rs = dir.open(dn)) >= 0) ylikely {
	            fsdir_ent	de ;
	            while ((rs = dir.read(&de,nbuf,nlen)) > 0) {
	                cint	sl = rs ;
	                cchar	*sp = de.name ;
	                if (hasNotDots(sp,sl)) {
	                    c += 1 ;
	                    rs = vecpstr_add(flp,sp,sl) ;
	                } /* end if (hasNotDots) */
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = dir.close ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (fsdir) */
		rs1 = lm_free(nbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (strnul) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecpstr_dirload) */

local int vecpstr_dirdel(vecpstr *flp,char *pbuf,int plen) noex {
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	cchar		*sp ;
	for (int i = 0 ; flp->get(i,&sp) >= 0 ; i += 1) {
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        if ((rs = pathadd(pbuf,plen,sp)) >= 0) ylikely {
	            if (sp[0] == 'p') {
	                if ((rs = NotRunning(sp,-1)) > 0) {
	                    rs = rmfile(pbuf) ;
	                    c += rs ;
	                } /* end if (NotRunning) */
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
} /* end subroutine (vecpstr_dirdel) */

local int openstate_open(openstate *lsp,cchar *fn,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (lsp && fn) ylikely {
	    rs = SR_INVALID ;
	    memclear(lsp) ;
	    if (fn[0]) ylikely {
	        if ((rs = u_open(fn,(of|O_EXCL),om)) >= 0) {
	            lsp->f_created = true ;
	        } else if ((!(of&O_EXCL)) && (rs == SR_EXISTS)) {
	            rs = u_open(fn,of,om) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (openstate_open) */

local int rmfile(cchar *fn) noex {
	int		rs ;
	if ((rs = u_unlink(fn)) >= 0) {
	    rs = 1 ;
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (rmfile) */

local int NotRunning(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	    cint	sch = mkchar(sp[0]) ;
	    rs = SR_OK ;
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
	    } /* end if */
	} /* end if (getlenstr) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (NotRunning) */

local bool isLocked(int rs) noex {
	return isOneOf(rslocked,rs) ;
} /* end subroutine (isLocked) */


