/* usys_nprocs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_nprocs

	Description:
	I think that there is no standard way (at least not in the
	POSIX® standard) to retrieve the number of processes currently
	on the system (either computing, sleeping or otherwise).
	So I procide something for some operating systems.

	Synopsis:
	int usys_nprocs(int w) noex

	Arguments:
	w		which types:
				0	all
				1	system-only
				2	current-user
				3	current-session

	Returns:
	>=0		number of processes on the system
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® |pid_t| */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"usys_getprocuid.h"
#include	"usys_nprocs.h"

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif

cint		maxsysuid	= SYSUID_MAX ;
cbool		f_debug		= CF_DEBUG ;

#if	defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0) 

#include	<libproc.h>		/* LIBPROC */

cint	nents	= 512 ; /* starting value */

namespace {
    struct cmdmaper {
	int	v[unprocscmd_overlast] ;
	consteval cmdmaper() noex {
	    v[unprocscmd_all]		= PROC_ALL_PIDS ;
	    v[unprocscmd_system]	= PROC_ALL_PIDS ;
	    v[unprocscmd_user]		= PROC_UID_ONLY ;
	    v[unprocscmd_session]	= PROC_UID_ONLY ;
	} ; /* end ctor */
    } ; /* end struct */
    struct lister {
	int w ;
	int type	= 0 ;
	int info	= 0 ;
	int matsid	= 0 ;
	int operator () (int) noex ;
	int listpids() noex ;
	int listpids(void *,int) noex ;
	int listcount(con pid_t *,int,int) noex ;
	int loop() noex ;
    } ; /* end struct (lister) */
} /* end namespace */

constexpr cmdmaper	cmdmap ;

int lister::operator () (int aw) noex {
    	int		rs = SR_OK ;
	w = aw ;
	switch (w) {
	case unprocscmd_session:
	    {
		con pid_t	pid = getpid() ;
		{
		    con pid_t spid = getsid(pid) ;
		    matsid = intconv(spid) ;
		}
	    }
	    break ;
	} /* end switch */
	switch ((type = cmdmap.v[w])) {
	case PROC_UID_ONLY:
	    {
		uid_t uid = getuid() ;
		info = intconv(uid) ;
	    }
	    break ;
	} /* end switch */
	if (rs >= 0) {
	    rs = loop() ;
	} /* end if (ok) */
	return rs ;
} /* end method (listern::operator) */

int lister::listpids(void *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	DPRINTF("ent\n") ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (type >= 0) {
	        if ((rs = proc_listpids(type,info,rbuf,rlen)) < 0) {
		    rs = (neg errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (lister::listpids) */

int lister::listcount(con pid_t *pidbuf,int pidlen,int rsz) noex {
	cint	rn = (rsz / szof(pid_t)) ;
	int	rs = SR_OVERFLOW ;
	int	c = 0 ; /* return-value */
	DPRINTF("ent pidlen=%d rn=%d\n",pidlen,rn) ;
	if (rsz < pidlen) {
	    rs = SR_OK ;
	    if (w == unprocscmd_system) {
	        for (int i = 0 ; i < rn ; i += 1) {
		    if (con pid_t pid = pid_t(pidbuf[i]) ; pid) {
		        if ((rs = getprocuid(pid)) >= 0) {
			    c += (rs <= maxsysuid) ;
		        } /* end if (getprocuid) */
		    } /* end if (non-empty slot) */
	        } /* end for */
	    } else if (w == unprocscmd_session) {
	        for (int i = 0 ; i < rn ; i += 1) {
		    if (con pid_t pid = pid_t(pidbuf[i]) ; pid) {
			con pid_t spid = getsid(pid) ;
			if (int sid = intconv(spid) ; sid == matsid) {
			    c += 1 ;
			} /* end if (session ID match) */
		    } /* end if (non-empty slot) */
	        } /* end for */
	    } else {
	        for (int i = 0 ; i < rn ; i += 1) {
		    c += (pidbuf[i] != 0) ;
	        } /* end for */
	    } /* end if */
	} /* end if (no overflow) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (lister::listcount) */

int lister::loop() noex {
	cnothrow	nt{} ;
	cint		rso = SR_OVERFLOW ;
	int		rs = SR_INVALID ;
	DPRINTF("ent\n") ;
	if (w >= 0) {
            int n = nents ;
	    bool fdone = false ;
            repeat {
	        DPRINTF("repeat n=%d\n",n) ;
	        rs = SR_NOMEM ;
	        if (pid_t *pidbuf = new(nt) pid_t [n] ; pidbuf) {
	            cint pidlen = n * szof(pid_t) ;
	            if ((rs = listpids(pidbuf,pidlen)) >= 0) {
		        if ((rs = listcount(pidbuf,pidlen,rs)) >= 0) {
			    fdone = true ;
			} else if (rs == rso) {
			    rs = SR_OK ;
		        }
	            } /* end if (listpids) */
		    n *= 2 ;
	            delete [] pidbuf ;
	        } /* end if (new-pid_t) */
	        DPRINTF("until rs=%d fdone=%u\n",rs,uint(fdone)) ;
            } until ((rs < 0) || fdone) ;
	} /* end if (valid) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (lister::loop) */

namespace usys {
    int usys_nprocs(int w) noex {
    	int		rs = SR_INVALID ;
	if ((w >= 0) && (w < unprocscmd_overlast)) {
	    lister lo ;
	    rs = lo(w) ;
	} /* end if (valid) */
	return rs ;
    } /* end subroutine (usys_nprocs) */
} /* end namespace (usys) */

#else /* defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0) */

namespace usys {
    int usys_nprocs(int w) noex {
	int		rs = SR_INVALID ;
	if (w >= 0) {
	    rs = SR_NOSYS ;
	} /* end if (valid) */
	return rs ;
    } /* end subroutine (usys_nprocs) */
} /* end namespace (usys) */

#endif /* defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0) */


