/* prqotd_open SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */

#define	CF_OPENMASK	0		/* use |openmask()| */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	prqotd_open

	Description:
	I open a QOTD quote-file and return a file-descriptor to
	it (which must be eventually closed).  The quote-file is
	identified by its Modified-Julian-Day (MJD).

	Synopsis:
	int prqotd_open(cchar *pr,int mjd,int of,int to) noex

	Arguments:
	pr		program-root
	mjd		modified-julian-day
	of		open-flags
	to		time-out

	Returns:
	>=0		FD of QOTD
	<0		error (system-return)

	Notes:

	- open flags:
		O_NOCTTY (delete)
		O_EXCL (expiration)
		O_SYNC
		O_NDELAY
		O_NONBLOCK

	= Implementation Notes:

	Q. Why does this subroutine have to be multi-thread-safe?
	A. Because all code needs to be multi-thread-safe today!

	Q. Why is this code not already naturally multi-thread-safe?
	A. Because this code uses file-record-locks; file-record-locks
	are inherently not multi-thread-safe!  Yes, Virginia, there are
	semi-secret pieces of system-supplied code all over the place
	that are not natually multi-thread-safe.

	Q. How do we fix the problem with using file-record-locks to make
	this subroutine mutli-thread-safe?
	A. We add a mutual-exclusion lock around the code that creates
	file-record-locks so that multiple threads can come into the code
	and not foul up due to the file-record-locks.

	Q. Since creating a new QOTD locally is such a rare event, why
	can we not call our own |_fini()| subroutine after we have
	completed that task?
	A. Beucase currently there is no way to un-register our
	|atexit()| handler from the system; currently this is only done
	when the code module containing us (this subroutine) is
	unloaded from the process memory address space.  Alternately
	there is (indeed) a way to un-register an |atfork()| handler --
	because I created that capability myself -- but we have not
	(yet) created the capability to un-register |atexit()|
	handlers.  

	Q. Why has not an un-register function been created to un-register
	an |atexit()| handler that had been previously registered?
	A. After some research into the matter, there is no *easy* way
	to create an |atexit()| un-register capability given the
	current X/Open® interface and implementation citcumstances.
	The reason is because the private data (locks and memory) that
	implement the ATEXIT facility are kept privately scoped inside
	the LIBC library (actally inside the |atexit()| subroutine code
	translation unit itself).  There is no way to access it in
	order to sneak in a little un-register function.  It would have
	been easy to implement had access to the private data
	structures been available, but they are not!  Thanks to the
	X/Open® and the previous UNIX® folks!  Got to love them!

	Q. How did we create a mutual-exclusion lock around the
	|lockfile()| subroutine?
	A. We created our own mutual-exclusion lock with our
	implementation consisting of |openqotd_capbegin()| and
	|openqotd_capend()|.

	Q. Why did not we just use |ptm_lockbegin()| and |ptm_lockend()|
	as our mutual-exclusion lock around |lockfile()|?
	A. Because:
	a. it is not a good idea to put POSIX® mutexes around a large
	piece of code (that calls unknown subroutines)
	b. the code inside the exclusion zone calls |uc_fork(3uc)|
	and that would create a deadlock (because of the "fork"-related
	pre-lock and post-unlock operations)

	Q. This is ridiculously complicated.  Were there not much easier
	ways to do this?
	A. Yes.  There are other mutual exlusion mechanisms available
	for file-system operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<csignal>		/* |sig_atomic_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucfork.h>
#include	<ucatfork.h>
#include	<ucatexit.h>
#include	<ucopen.h>
#include	<ucdesc.h>
#include	<bufsizevar.hh>
#include	<ptm.h>
#include	<ptc.h>
#include	<tmtime.hh>
#include	<storebuf.h>
#include	<vecpstr.h>
#include	<fsdir.h>
#include	<pathadd.h>
#include	<sfx.h>
#include	<getmjd.h>		/* |getmjd(3uc)| */
#include	<prmktmpdir.h>
#include	<lockfile.h>
#include	<hasx.h>
#include	<isnot.h>		/* |isNotAccess(3uc)| */
#include	<localmisc.h>

#include	"prqotd.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	VTMPDNAME	"/var/tmp"
#define	QCNAME		"qotd"

#define	OPENQOTD	openqotd_head
#define	OPENQOTD_SUB	openqotd_sub

#define	TTL_EXPIRE	(30*24*3600)		/* default 30 days */

#define	NFNAME		"qotd.ndeb"


/* imported namespaces */


/* local typedefs */

typedef volatile sig_atomic_t	vaflag ;


/* external subroutines */

extern "C" {
    extern int uc_mkdir(cchar *,mode_t) noex ;
    extern int uc_mkfifo(cchar *,mode_t) noex ;
    extern int uc_chmod(cchar *,mode_t) noex ;
    extern int uc_stat(cchar *,ustat *) noex ;
    extern int uc_unlink(cchar *) noex ;
    extern int uc_unlinkshm(cchar *) noex ;
    extern int uc_open(cchar *,int,mode_t) noex ;
    extern int uc_socket(int,int,int) noex ;
    extern int uc_sockjoin(int,SOCKADDR *,int,int,mode_t) noex ;
    extern int uc_openshm(cchar *,int,mode_t) noex ;
    extern int uc_duper(int,int) noex ;
    extern int uc_pipe(int *) noex ;
    extern int uc_bind(int,cvoid *,int) noex ;
    extern int uc_fstat(int,ustat *) noex ;
    extern int uc_fsize(int) noex ;
    extern int uc_fchown(int,uid_t,gid_t) noex ;
    extern int uc_fminmod(int,mode_t) noex ;
    extern int uc_connect(int,cvoid *,int) noex ;
    extern int uc_connecte(int,cvoid *,int,int) noex ;
    extern int uc_lockf(int,int,off_t) noex ;
    extern int uc_read(int,void *,int) noex ;
    extern int uc_write(int,cvoid *,int) noex ;
    extern int uc_writen(int,cvoid *,int) noex ;
    extern int uc_iocctl(int,int,...) noex ;
    extern int uc_ftruncate(int,off_t ) noex ;
    extern int uc_closeonexec(int,int) noex ;
    extern int uc_setsockopt(int,int,int,int *,int) noex ;
    extern int uc_linger(int,int) noex ;
} /* end extern */

extern "C" {
    int	openqotd_init() noex ;
    int	openqotd_fini() noex ;
}


/* external variables */


/* local structures */

struct openqotd_head {
	ptm		mx ;		/* data mutex */
	ptc		cn ;		/* condition variable */
	volatile int	waiters ;
	vaflag		f_capture ;	/* capture flag */
	vaflag		f_init ;
	vaflag		f_initdone ;
} ; /* end struct */

struct openqotd_sub {
	cchar		*pr ;
	cchar		*vtmpdname ;
	cchar		*qcname ;
	cchar		*qfname ;
	int		of ;
	int		ttl ;
	int		mjd ;
	mode_t		dm ;
} ;


/* forward references */

local int	openqotd_capbegin(int) noex ;
local int	openqotd_capend() noex ;
local int	openqotd_open(OPENQOTD_SUB *) noex ;

local int	qotdexpire(cchar *,cchar *,int,cchar *,time_t,int) noex ;
local int	qotdexpireload(vecpstr *,char *,time_t,int) noex ;

local int	qotdfetch(cchar *,int,int,int,cchar *) noex ;

#if	CF_OPENMASK
local int	openmask(cchar *,int,mode_t) noex ;
#endif /* CF_OPENMASK */

local int	loadchown(cchar *,int) noex ;

local int	getdefmjd(time_t) noex ;

local int	mkqdname(char *,cchar *, cchar *,int,cchar *) noex ;
local int	mkqfname(char *,cchar *, cchar *,int,cchar *,int) noex ;

extern "C" {
    local void	openqotd_atforkbefore() noex ;
    local void	openqotd_atforkafter() noex ;
    local void	openqotd_exit() noex ;
}


/* local variables */

static OPENQOTD		openqotd_data ;
static bufsizevar	maxpathlen(bufsize_mp) ;


/* exported variables */


/* exported subroutines */

int openqotd_init() noex {
	OPENQOTD	*uip = &openqotd_data ;
	int		rs = 1 ;
	if (! uip->f_init) {
	    ptm *mxp = &uip->mx ;
	    uip->f_init = true ;
	    if ((rs = mxp->create) >= 0) {
		ptc *cnp = &uip->cn ;
	        if ((rs = cnp->create) >= 0) {
	    	    void_f	b = openqotd_atforkbefore ;
	    	    void_f	a = openqotd_atforkafter ;
	            if ((rs = uc_atforkrec(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(openqotd_exit)) >= 0) {
	                    rs = 0 ;
	                    uip->f_initdone = true ;
	                }
	                if (rs < 0) {
	                    uc_atforkexp(b,a,a) ;
			}
	            }
	            if (rs < 0) {
	                cnp->destroy() ;
		    }
	        } /* end if (ptc_create) */
	        if (rs < 0) {
	            mxp->destroy() ;
		}
	    } /* end if (ptm_create) */
	} else {
	    while (! uip->f_initdone) {
		msleep(1) ;
	    }
	}
	return rs ;
}
/* end subroutine (openqotd_init) */

int openqotd_fini() noex {
	OPENQOTD	*uip = &openqotd_data ;
	int		rs = SR_NXIO ;
	int		rs1 ;
	if (uip->f_initdone) {
	    uip->f_initdone = false ;
	    rs = SR_OK ;
	    {
	        void_f	b = openqotd_atforkbefore ;
	        void_f	a = openqotd_atforkafter ;
	        rs1 = uc_atforkexp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		ptc *cnp = &uip->cn ;
	        rs1 = cnp->destroy() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        ptm *mxp = &uip->mx ;
	        rs1 = mxp->destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    memclear(uip) ;
	} /* end if (was initialized) */
	return rs ;
}
/* end subroutine (openqotd_fini) */

int prqotd_open(cchar *pr,int mjd,int of,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (pr) {
	    time_t	dt = 0 ;
	    cmode	dm = 0777 ;
	    cchar	*vtmpdname = VTMPDNAME ;
	    cchar	*qcname = QCNAME ;
	    if (mjd <= 0) {
	        if (dt == 0) dt = time(nullptr) ;
	        rs = getdefmjd(dt) ;
	        mjd = rs ;
	    }
    
	    if (rs >= 0) {
	        cchar	*rnp{} ;
	        if (int rnl ; (rnl = sfbasename(pr,-1,&rnp)) > 0) {
	            cchar	*vtd = vtmpdname ;
	            cchar	*cn = qcname ;
	            if (of & O_EXCL) {
	                if (dt == 0) dt = time(nullptr) ;
	                rs = qotdexpire(vtd,rnp,rnl,cn,dt,to) ;
	            }
	            if (rs >= 0) {
			if (char *qfname ; (rs = lm_mp(&qfname)) >= 0) {
			    cauto	mk = mkqfname ;
	                    if ((rs = mk(qfname,vtd,rnp,rnl,cn,mjd)) >= 0) {
	                        {
	                            OPENQOTD_SUB	qs{} ;
	                            qs.pr = pr ;
	                            qs.qfname = qfname ;
	                            qs.vtmpdname = vtmpdname ;
	                            qs.qcname = qcname ;
	                            qs.ttl = to ;
	                            qs.of = of ;
	                            qs.dm = dm ;
	                            qs.mjd = mjd ;
	                            if ((rs = openqotd_open(&qs)) >= 0) {
	                                fd = rs ;
	                            }
	                        }
	                        if ((rs >= 0) && (of & O_NOCTTY)) {
	                            uc_unlink(qfname) ;
	                        }
	                        if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	                    } /* end if (mkqfname) */
			    rs1 = lm_free(qfname) ;
			    if (rs >= 0) rs = rs1 ;
		        } /* end if (m-a-f) */
	            } /* end if (ok) */
	        } else {
	            rs = SR_NOTDIR ;
	        }
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (prqotd_open) */


/* local subroutines */

local int openqotd_capbegin(int to) noex {
	OPENQOTD	*uip = &openqotd_data ;
	int		rs ;
	int		rs1 ;
	ptm *mxp = &uip->mx ;
	if ((rs = mxp->lockbegin(to)) >= 0) {
	    {
	        ptc *cnp = &uip->cn ;
	        uip->waiters += 1 ;
	        while ((rs >= 0) && uip->f_capture) { /* busy */
	            rs = cnp->wait(mxp,to) ;
	        } /* end while */
	        if (rs >= 0) {
	            uip->f_capture = true ;
	        }
	        uip->waiters -= 1 ;
	    } /* end block */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
} /* end subroutine (openqotd_capbegin) */

local int openqotd_capend() noex {
	OPENQOTD	*uip = &openqotd_data ;
	int		rs ;
	int		rs1 ;
	ptm *mxp = &uip->mx ;
	if ((rs = mxp->lockbegin) >= 0) {
	    {
		ptc *cnp = &uip->cn ;
	        uip->f_capture = false ;
	        if (uip->waiters > 0) {
	            rs = cnp->signal ;
	        }
	    } /* end block */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
} /* end subroutine (openqotd_capend) */

local int openqotd_open(OPENQOTD_SUB *sip) noex {
	cmode		om = 0666 ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = uc_open(sip->qfname,sip->of,om)) >= 0) {
	    fd = rs ;
	} else if (rs == SR_NOENT) {
	    cmode	dm = sip->dm ;
	    cchar	*pr = sip->pr ;
	    cchar	*qfname = sip->qfname ;
	    cchar	*vtmpdname = sip->vtmpdname ;
	    cchar	*qcname = sip->qcname ;
	    if (char *qdname ; (rs = lm_mp(&qdname)) >= 0) {
	        if ((rs = prmktmpdir(pr,qdname,vtmpdname,qcname,dm)) >= 0) {
	            cint	mjd = sip->mjd ;
	            cint	of = sip->of ;
	            cint	ttl = sip->ttl ;
	            rs = qotdfetch(pr,mjd,of,ttl,qfname) ;
	            fd = rs ;
	        } /* end if (prmktmpdir) */
		rs1 = lm_free(qdname) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (NOENT) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (openqotd_open) */

local void openqotd_atforkbefore() noex {
	OPENQOTD	*uip = &openqotd_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockbegin() ;
	}
} /* end subroutine (openqotd_atforkbefore) */

local void openqotd_atforkafter() noex {
	OPENQOTD	*uip = &openqotd_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockend() ;
	}
} /* end subroutine (openqotd_atforkafter) */

local void openqotd_exit() noex {
	cint		rs = openqotd_fini() ;
	if (rs < 0) {
	    ulogerror("openqotd",rs,"exit-fini") ;
	}
} /* end subroutine (openqotd_exit) */

local int qotdexpire(cc *vtd,cc *rnp,int rnl,cc *cn,time_t dt,int to) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (to <= 0) to = TTL_EXPIRE ;
	if (char *qdname ; (rs = lm_mp(&qdname)) >= 0) {
	    if ((rs = mkqdname(qdname,vtd,rnp,rnl,cn)) >= 0) {
	        if (ustat sb ; (rs = uc_stat(qdname,&sb)) >= 0) {
	            if (S_ISDIR(sb.st_mode)) {
	                cint	n = intconv(sb.st_size / 10) ;
	                cint	cs = intconv(sb.st_size / 4) ;
	                vecpstr	ds ;
	                if ((rs = vecpstr_start(&ds,n,cs,0)) >= 0) {
	                    if ((rs = qotdexpireload(&ds,qdname,dt,to)) > 0) {
			        cauto	vg = vecpstr_get ;
	                        cchar	*fn{} ;
	                        for (int i = 0 ; vg(&ds,i,&fn) >= 0 ; i += 1) {
	                            if (fn) {
	                                rs1 = uc_unlink(fn) ;
	                                if (rs1 >= 0) c += 1 ;
				    }
	                        } /* end for */
	                    } /* end if (qotdexpireload) */
	                    rs1 = vecpstr_finish(&ds) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (vecpstr) */
	            } else {
	                rs = SR_NOTDIR ;
		    }
		} else if (isNotAccess(rs)) {
		    rs = SR_OK ;
	        } /* end if (stat) */
	    } /* end if (mkqdname) */
	    rs1 = lm_free(qdname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (qotdexpire) */

local int qotdexpireload(vecpstr *dsp,char *qfname,time_t dt,int to) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *ebuf ; (rs = lm_mn(&ebuf)) >= 0) {
	    cint	elen = rs ;
	    if (fsdir d ; (rs = fsdir_open(&d,qfname)) >= 0) {
	        fsdir_ent	de ;
	        ustat		sb ;
	        cint		dlen = lenstr(qfname) ;
	        while ((rs = fsdir_read(&d,&de,ebuf,elen)) > 0) {
	            cint	el = rs ;
	            cchar	*ep = de.name ;
	            if (hasNotDots(ep,el)) {
	                if ((rs = pathadd(qfname,dlen,ep)) >= 0) {
	                    cint	fl = rs ;
	                    if (uc_stat(qfname,&sb) >= 0) {
	                        if (S_ISREG(sb.st_mode)) {
    
	                            if ((dt-sb.st_mtime) >= to) {
	                                c += 1 ;
	                                rs = vecpstr_add(dsp,qfname,fl) ;
	                            } /* end if (expired) */
	                        } /* end if (regular file) */
	                    } /* end if (stat) */
	                } /* end if (pathadd) */
	            } /* end if (hasNotDots) */
	            if (rs < 0) break ;
	        } /* end while (fsdir_read) */
	        qfname[dlen] = '\0' ;
	        rs1 = fsdir_close(&d) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = lm_free(ebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (qotdexpireload) */

/* ARGSUSED */
local int qotdfetch(cc *pr,int mjd,int of,int ttl,cc *qfname) noex {
	int		rs ;
	int		rs1 ;
	int		lof = of ;
	int		fd = -1 ;
	cmode		om = 0664 ;
	lof &= (~ O_ACCMODE) ;
	lof &= (~ O_TRUNC) ;
	lof &= (~ O_EXCL) ;
	lof |= (O_CREAT | O_RDWR) ;
	if ((rs = uc_open(qfname,lof,om)) >= 0) {
	    fd = rs ;
	    if ((rs = uc_fminmod(fd,om)) >= 0) {
	        if ((rs = openqotd_init()) >= 0) {
	            cint	to = utimeout[uto_busy] ;
	            if ((rs = openqotd_capbegin(to)) >= 0) {
	                cint	cmd = F_WLOCK ;
	                if ((rs = lockfile(fd,cmd,0L,0L,to)) >= 0) {
	                    if ((rs = uc_fsize(fd)) == 0) {
	                        if ((rs = prqotd_maint(pr,mjd,of,ttl)) >= 0) {
	                            cint	s = rs ;
	                            if ((rs = uc_writedesc(fd,s,-1)) >= 0) {
	                                if ((rs = uc_rewind(fd)) >= 0) {
	                                    rs = loadchown(pr,fd) ;
					}
	                            }
	                            rs1 = uc_close(s) ;
	                    	    if (rs >= 0) rs = rs1 ;
	                        } /* end if (maintqotd) */
	                    } /* end if (file-size-is-zero) */
	                    rs1 = uc_lockf(fd,F_ULOCK,0z) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (file-lock) */
	                rs1 = openqotd_capend() ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	        } /* end if (openqotd_init) */
	    } /* end if (uc_fminmod) */
	    if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	} /* end if (open-) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (qotdfetch) */

#if	CF_OPENMASK
local int openmask(cchar *qfname,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = umaskset(0)) >= 0) {
	    mode_t	pm = rs ;
	    rs = uc_open(qfname,of,om) ;
	    fd = rs ;
	    rs1 = umaskset(pm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end if (openmask) */
#endif /* CF_OPENMASK */

local int loadchown(cchar *pr,int fd) noex {
	int		rs ;
	if (ustat sb ; (rs = uc_stat(pr,&sb)) >= 0) {
	    uid_t	euid = geteuid() ;
	    if (euid != sb.st_uid) {
	        uc_fchown(fd,sb.st_uid,sb.st_gid) ;
	    }
	} /* end if (uc_stat) */
	return rs ;
} /* end subroutine (loadchown) */

local int getdefmjd(time_t dt) noex {
	TMTIME		ct ;
	int		rs ;
	if (dt == 0) dt = time(nullptr) ;
	if ((rs = tmtime_timelocal(&ct,dt)) >= 0) {
	    int	y = (ct.year + TM_YEAR_BASE) ;
	    int	m = ct.mon ;
	    int	d = ct.mday ;
	    rs = getmjd(y,m,d) ;
	} /* end if (tmtime_timelocal) */
	return rs ;
} /* end subroutine (getdefmjd) */

local int mkqdname(char *rbuf,cc *vtmpdname,cc *rnp,int rnl,cc *qcname) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	rlen = rs ;
	    if (rs >= 0) {
	        rs = storebuf_strw(rbuf,rlen,i,vtmpdname,-1) ;
	        i += rs ;
	    }
	    if ((rs >= 0) && (i > 0) && (rbuf[i-1] != '/')) {
	        rs = storebuf_chr(rbuf,rlen,i,'/') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_strw(rbuf,rlen,i,rnp,rnl) ;
	        i += rs ;
	    }
	    if ((rs >= 0) && (i > 0) && (rbuf[i-1] != '/')) {
	        rs = storebuf_chr(rbuf,rlen,i,'/') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_strw(rbuf,rlen,i,qcname,-1) ;
	        i += rs ;
	    }
	} /* end if (maxpathlen) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (mkqdname) */

local int mkqfname(char *rbuf,cc *vtdn,cc *rnp,int rnl,cc *qcn,int mjd) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	rlen = rs ;
	    if (rs >= 0) {
	        rs = mkqdname(rbuf,vtdn,rnp,rnl,qcn) ;
	        i += rs ;
	    }
	    if ((rs >= 0) && (i > 0) && (rbuf[i-1] != '/')) {
	        rs = storebuf_chr(rbuf,rlen,i,'/') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_chr(rbuf,rlen,i,'q') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_deci(rbuf,rlen,i,mjd) ;
	        i += rs ;
	    }
	} /* end if (maxpathlen) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (mkqfname) */

#ifdef	COMMENT
extern int ofWritable(int of) noex {
	int	f = false ;
	f = f || ((of & O_ACCMODE) == O_WRONLY) ;
	f = f || ((of & O_ACCMODE) == O_RDWR) ;
	return f ;
} /* end subroutine (ofWritable) */
#endif /* COMMENT */


