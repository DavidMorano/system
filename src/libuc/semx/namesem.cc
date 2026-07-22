/* namesem SUPPORT (named semaphore) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* POSIX® real-time -- named -- Counting Semaphore (NAMESEM) */
/* version %I% last-modified %G% */

#define	CF_CONDUNLINK	1		/* conditional unlink */

/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	namesem

	Description:
	This object essentially (secretly) implements a POSIX®
	"named" semaphore!  This object does *not* have the same
	API as the regular POSIX® named-semaphore interface.  This
	module provides a sanitized version of the standard POSIX®
	semaphore facility provided with some new UNIX®i.  Some
	operating system problems are managed within these routines
	for the common stuff, like memory exhaustion.

	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |mode_t| */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<semaphore.h>		/* POSIX® semaphores */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cassert>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucgetpw.h>		/* LIBUC */
#include	<getxid.h>		/* LIBUC */
#include	<errtimer.hh>		/* LIBUC */
#include	<posname.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"namesem.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	NAMESEM_PATHPREFIX	"/tmp/namesem"

#define	NAMESEM_CHOWNVAR	_PC_CHOWN_RESTRICTED

#define	NAMESEM_USERNAME1	"sys"
#define	NAMESEM_USERNAME2	"adm"

#define	NAMESEM_GROUPNAME1	"sys"
#define	NAMESEM_GROUPNAME2	"sys"

#define	NAMESEM_UID	3
#define	NAMESEM_GID	3

#ifndef	CF_CONDUNLINK
#define	CF_CONDUNLINK	0		/* conditional unlink */
#endif

#define	NLPS		5	/* number of polls per second */


/* external subroutines */

extern "C" {
    extern int uc_unlink(cchar *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int namesem_magic(namesem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == NAMESEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (namesem_magic) */

local int	namesemopen(namesem *,cc *,int,mode_t,int) noex ;
local int	namesemclose(namesem *) noex ;

local int	namesemdircheck(cchar *) noex ;
local int	namesemdiradd(cchar *,mode_t) noex ;
local int	namesemdirrm(cchar *) noex ;

local int	getnamesemgid(void) noex ;


/* local variables */

cbool		f_condunlink = CF_CONDUNLINK ;


/* exported variables */


/* exported subroutines */

int namesem_open(namesem *op,cchar *name,int of,mode_t om,uint c) noex {
    	NAMESEM		*hop = op ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	assert(op && name) ;
	if (op && name) ylikely {
	    memclear(hop) ;
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
		cchar	*nn ;
		if (posname pn ; (rs = pn.start(name,-1,&nn)) >= 0) ylikely {
		    if ((rs = namesemopen(op,nn,of,om,c)) >= 0) ylikely {
			if (char *bp ; (rs = lm_mn(&bp)) >= 0) ylikely {
			    cint	mnlen = rs ;
			    op->name = bp ;
	                    strwcpy(bp,nn,mnlen) ;
	                    if (of & O_CREAT) {
				rs = namesemdiradd(nn,om) ;
			    }
			    if (rs >= 0) {
	                        op->magic = NAMESEM_MAGIC ;
			    }
			    if (rs < 0) {
				lm_free(op->name) ;
				op->name = nullptr ;
			    } /* end if (error) */
			} /* end if (memory-acquire) */
	            } /* end if (opened) */
		    rs1 = pn.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (posname) */
	    } /* end if (non-zero name) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (namesem_open) */

int namesem_close(namesem *op) noex {
	int		rs ;
	int		rs1 ;
	assert(op) ;
	if ((rs = namesem_magic(op)) >= 0) ylikely {
	    {
	        rs1 = namesemclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->name) ylikely {
		rs1 = lm_free(op->name) ;
		if (rs >= 0) rs = rs1 ;
		op->name = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (namesem_close) */

int namesem_wait(namesem *op) noex {
	int		rs ;
	assert(op) ;
	if ((rs = namesem_magic(op)) >= 0) ylikely {
	    repeat {
	        if ((rs = sem_wait(op->sp)) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (namesem_wait) */

/* wait and wakeup on interrupt */
int namesem_waiti(namesem *op) noex {
	int		rs ;
	assert(op) ;
	if ((rs = namesem_magic(op)) >= 0) ylikely {
	   if ((rs = sem_wait(op->sp)) < 0) {
	       rs = (- errno) ;
	   }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (namesem_waiti) */

int namesem_waiter(namesem *op,int to) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	assert(op) ;
	if (to < 0) to = (INT_MAX / (2 * NLPS)) ;
	if (op) ylikely {
	    cint	mint = (1000 / NLPS) ;
	    cint	cto = (to * NLPS) ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = sem_trywait(op->sp)) < 0) {
		    rs = (- errno) ;
		    switch (rs) {
	    	    case SR_AGAIN:
		        if (c++ < cto) {
			    msleep(mint) ;
		        } else {
			    rs = SR_TIMEDOUT ;
			    f_exit = true ;
		        }
		        break ;
		    case SR_INTR:
		        break ;
		    default:
		        f_exit = true ;
		        break ;
		    } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (namesem_waiter) */

int namesem_trywait(namesem *op) noex {
	int		rs ;
	assert(op) ;
	if ((rs = namesem_magic(op)) >= 0) ylikely {
	    repeat {
	        if ((rs = sem_trywait(op->sp)) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (namesem_trywait) */

int namesem_post(namesem *op) noex {
	int		rs ;
	assert(op) ;
	if ((rs = namesem_magic(op)) >= 0) ylikely {
	    repeat {
	        if ((rs = sem_post(op->sp)) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (namesem_post) */

int namesem_unlink(namesem *op) noex {
	int		rs ;
	assert(op) ;
	if ((rs = namesem_magic(op)) >= 0) ylikely {
	    if (op->name[0]) ylikely {
		rs = unlinknamesem(op->name) ;
		if_constexpr (f_condunlink) {
		    if (rs >= 0) {
			namesemdirrm(op->name) ;
		    }
		} else {
		    namesemdirrm(op->name) ;
		}
	    } /* end if (not zero name) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (namesem_unlink) */

int namesem_count(namesem *op) noex {
	int		rs ;
	int		c = 0 ;
	assert(op) ;
	if ((rs = namesem_magic(op)) >= 0) ylikely {
	    while ((rs = namesem_trywait(op)) >= 0) {
		c += 1 ;
	    } /* end while */
	    if (rs == SR_AGAIN) {
		rs = SR_OK ;
		for (int i = 0 ; (rs >= 0) && (i < c) ; i += 1) {
		    rs = namesem_post(op) ;
		} /* end for */
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (namesem_count) */


/* OTHER API (but related) */

int namesemunlink(cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
		cchar	*nn ;
		if (posname pn ; (rs = pn.start(name,-1,&nn)) >= 0) {
	            repeat {
	                if ((rs = sem_unlink(nn)) < 0) {
			    rs = (- errno) ;
			}
	            } until (rs != SR_INTR) ;
		    if_constexpr (f_condunlink) {
		        if (rs >= 0) {
	    		    namesemdirrm(nn) ;
		        }
		    } else {
		        namesemdirrm(nn) ;
		    }
		    rs1 = pn.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (posname) */
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (namesemunlink) */

int unlinknamesem(cchar *name) noex {
	return namesemunlink(name) ;
} /* end subroutine (unlinknamesem) */


/* local subroutines */

local int namesemopen(namesem *op,cc *name,int of,mode_t om,int c) noex {
	errtimer	to_mfile = utimeout[uto_mfile] ;
	errtimer	to_nfile = utimeout[uto_nfile] ;
	errtimer	to_nomem = utimeout[uto_nomem] ;
	errtimer	to_nospc = utimeout[uto_nospc] ;
        reterr          r ;
    	int		rs ;
        repeat {
            rs = SR_OK ;
	    if ((op->sp = sem_open(name,of,om,c)) == SEM_FAILED) {
                rs = (- errno) ;
                r(rs) ;                 /* <- default causes exit */
	        switch (rs) {
	        case SR_MFILE:
                    r = to_mfile(rs) ;
		    break ;
	        case SR_NFILE:
                    r = to_nfile(rs) ;
		    break ;
		case SR_NOMEM:
                    r = to_nomem(rs) ;
		    break ;
	        case SR_NOSPC:
                    r = to_nospc(rs) ;
		    break ;
	        case SR_INTR:
		    r(false) ;
	            break ;
	        } /* end switch */
		rs = r ;
	    } /* end if (error) */
	} until ((rs >= 0) || r.fexit) ;
	return rs ;
} /* end subroutine (namesemopen) */

local int namesemclose(namesem *op) noex {
    	int		rs = SR_BUGCHECK ;
	if (op->sp) ylikely {
	    repeat {
	        if ((rs = sem_close(op->sp)) < 0) {
		    rs = (- errno) ;
		}
	    } until (rs != SR_INTR) ;
	    op->sp = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (namesemclose) */

local int namesemdiradd(cchar *name,mode_t om) noex {
	cint		rsn = SR_NOENT ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) ylikely {
	    if (char *tmpfname ; (rs = lm_mp(&tmpfname)) >= 0) ylikely {
		cchar	*pp = NAMESEM_PATHPREFIX ;
		if ((rs = mkpath2(tmpfname,pp,name)) >= 0) ylikely {
	            if ((rs = u_creat(tmpfname,om)) == rsn) {
	                if ((rs = namesemdircheck(pp)) >= 0) {
	                    rs = u_creat(tmpfname,om) ;
	                }
	            }
	            if (rs >= 0) u_close(rs) ;
		} /* end if (mkpath2) */
		rs1 = lm_free(tmpfname) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (lm-tmpfname) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (namesemdiradd) */

local int namesemdirrm(cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) ylikely {
	    if (char *tmpfname ; (rs = lm_mp(&tmpfname)) >= 0) ylikely {
	        cchar	*pp = NAMESEM_PATHPREFIX ;
	        if ((rs = mkpath2(tmpfname,pp,name)) >= 0) ylikely {
		    rs = u_unlink(tmpfname) ;
	        } /* end if (mkpath2) */
	        rs1 = lm_free(tmpfname) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (lm-tmpfnam) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (namesemdirrm) */
 
local int namesemdirchecker(char *pwbuf,int pwlen,cchar *pp) noex ;
local int namesemdircheckers(char *pwbuf,int pwlen,cchar *pp) noex ;

local int namesemdircheck(cchar *pp) noex {
	int		rs ;
	int		rs1 ;
	if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	    cint	pwlen = rs ;
	    if (ustat sb ; (rs = u_stat(pp,&sb)) >= 0) ylikely {
		if (! S_ISDIR(sb.st_mode)) {
		    if ((rs = uc_unlink(pp)) >= 0) {
		        rs = namesemdirchecker(pwbuf,pwlen,pp) ;
		    }
		}
	    } else if (isNotPresent(rs)) {
		rs = namesemdirchecker(pwbuf,pwlen,pp) ;
	    }
	    rs1 = lm_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (namesemdircheck) */

local int namesemdirchecker(char *pwbuf,int pwlen,cchar *pp) noex {
    	int		rs ;
	cmode		dm = 0777 ;
	if ((rs = u_mkdir(pp,dm)) >= 0) ylikely {
	    if ((rs = u_chmod(pp,(dm | S_ISVTX))) >= 0) ylikely {
		cint	cmd = NAMESEM_CHOWNVAR ;
	    	if (long cv ; (rs = u_pathconf(pp,cmd,&cv)) >= 0) {
		    if (cv) {
			rs = namesemdircheckers(pwbuf,pwlen,pp) ;
		    }
		} else if (rs == isNotPresent(rs)) {
			rs = namesemdircheckers(pwbuf,pwlen,pp) ;
		}
	    } /* end if (was able to CHMOD) */
	} /* end if (directory did not exist) */
	return rs ;
} /* end subroutine (namesemdirchecker) */

local int namesemdircheckers(char *pwbuf,int pwlen,cchar *pp) noex {
	cauto		getpw = uc_getpwnam ;
	const uid_t	euid = geteuid() ;
	uid_t		uid = NAMESEM_UID ;
	cchar		*un = NAMESEM_USERNAME1 ;
    	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if (ucentpw pw ; (rs = getpw(&pw,pwbuf,pwlen,un)) >= 0) {
	    uid = pw.pw_uid ;
	} else if (rs == rsn) {
	    un = NAMESEM_USERNAME2 ;
	    if ((rs = getpw(&pw,pwbuf,pwlen,un)) >= 0) {
		uid = pw.pw_uid ;
	    } else if (rs == rsn) {
		rs = SR_OK ;
	    }
	} /* end if (pw) */
	if ((rs >= 0) && (euid != uid)) {
	    if ((rs = getnamesemgid()) >= 0) {
		const gid_t	gid = rs ;
		rs = u_chown(pp,uid,gid) ;
	    }
	} /* end if (UIDs different) */
	return rs ;
} /* end subroutine (namesemdircheckers) */

local int getnamesemgid(void) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	cchar		*gn = NAMESEM_GROUPNAME1 ;
	if ((rs = getgid_group(gn,-1)) == rsn) {
	    gn = NAMESEM_GROUPNAME2 ;
	    if ((rs = getgid_group(gn,-1)) == rsn) {
		rs = NAMESEM_GID ;
	    }
	}
	return rs ;
} /* end subroutine (getnamesemgid) */

int namesem::open(cchar *aname,int of,mode_t om,uint c) noex {
	return namesem_open(this,aname,of,om,c) ;
}

void namesem::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("namesem",rs,"fini-finish") ;
	}
} /* end method (namesem::dtor) */

namesem::operator int () noex {
    	return namesem_count(this) ;
} /* end method (namesem::operator) */

int namesem_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case namesemmem_close:
	        rs = namesem_close(op) ;
	        break ;
	    case namesemmem_wait:
	        rs = namesem_wait(op) ;
	        break ;
	    case namesemmem_waiti:
	        rs = namesem_waiti(op) ;
	        break ;
	    case namesemmem_waiter:
	        rs = namesem_waiter(op,a) ;
	        break ;
	    case namesemmem_trywait:
	        rs = namesem_trywait(op) ;
	        break ;
	    case namesemmem_post:
	        rs = namesem_post(op) ;
	        break ;
	    case namesemmem_unlink:
	        rs = namesem_unlink(op) ;
	        break ;
	    case namesemmem_count:
	        rs = namesem_count(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (namesem_co::operator) */


