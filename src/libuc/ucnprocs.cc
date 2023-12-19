/* ucnprocs */
/* lang=C++20 */

/* get the number of processes on the system */
/* version %I% last-modified %G% */


/* revision history:

	= 1997-11-18, David A-D­ Morano
	This little subroutine was put together to get the current
	number of processes executing on the system.

	= 2019-01-14, David A-D- Morano
	Enhanced (after all this time) to ignore special entries
	(now common on newer OSes).

*/

/* Copyright © 1998,2019 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_nprocs

	Description:
	We basically cheat and count the number of "files" in '/proc'
	that start with a leading digit character. The filename may
	have to return a good |stat(2)| also. Of course, the whole
	file-system might be unmounted also, in which case we return
	with failure.

	Synopsis:
	int uc_nprocs(int w) noex

	Arguments:
	w		which processes to count:
				0=all
				1=system-only
				2=current-user
				3=current-session

	Returns:
	<0		error
	>=0		number of processes on the machine

	Notes:
	Amazingly, the '/proc' file-system with file entries in it
	that look like process IDs is probably the most portable
	way invented so far to enumerate process IDs on the system.
	I think we can all thank AT&T for this, for inventing the
	PROC FS back in the mid-1980s or so. It is amazing how much
	stuff that AT&T invented in the 1980s for UNIX® that has
	passed the test of time. The only real problem with the
	PROC FS is that it is possible that orifinally it could be
	unmounted (not mounted), in which case everyone is left
	with nothing. But some news OSes do not allow

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<libmallocxx.h>
#include	<mkchar.h>
#include	<fsdir.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<cfdec.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"ucproctypes.h"


/* local defines */

#ifndef	PROCDNAME
#define	PROCDNAME	"/proc"
#endif

#ifndef	UIDSYS
#define	UIDSYS		100		/* common but unofficial value */
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct procer {
	char		*nbuf{} ;
	uid_t		uid ;
	pid_t		sid ;
	pid_t		pgid ;
	int		nlen ;
	int		w = 0 ;
	fsdir		d ;
	fsdir_ent	de ;
	procer(int aw) noex : w(aw) { } ;
	int fsdent() noex ;
	int selection() noex ;
	int setid() noex ;
	operator int () noex ;
    } ; /* end struct (procer) */
}


/* forward references */


/* local variables */

constexpr cchar		dn[] = PROCDNAME ;


/* exported subroutines */

int uc_nprocs(int w) noex {
	procer		po(w) ;
	return po ;
}
/* end subroutine (uc_nprocs) */


/* local subroutines */

procer::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = setid()) >= 0) {
	    if ((rs = libmalloc_mn(&nbuf)) >= 0) {
	        nlen = rs ;
		{
	            rs = fsdent() ;
		    c = rs ;
		}
		rs1 = uc_libfree(nbuf) ;
		if (rs >= 0) rs = rs1 ;
		nbuf = nullptr ;
	    } /* end if (lib-allocation-free) */
	} /* end if (valid) */
	return (rs >= 0) ? c : rs ;
}
/* end method (procer::operator) */


/* local subroutines */

int procer::setid() noex {
	int		rs = SR_INVALID ;
	if ((w >= 0) && (w < ucproctype_overlast)) {
	    rs = SR_OK ;
	    switch (w) {
	    case ucproctype_user:
		uid = getuid() ;
		break ;
	    case ucproctype_session:
		sid = getsid(0) ;
		break ;
	    case ucproctype_group:
		pgid = getpgrp() ;
		break ;
	    } /* end switch */
	} /* end if (valid) */
	return rs ;
}
/* end method (procer::setid) */

int procer::fsdent() noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
        if ((rs = fsdir_open(&d,dn)) >= 0) {
                switch (w) {
                case ucproctype_all: /* all processes */
                    {
                        while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
                            cint    ch = mkchar(de.name[0]) ;
                            if (isdigitlatin(ch)) {
                                n += 1 ;
                            }
                        } /* end while */
                    } /* end block */
                    break ;
                case ucproctype_sys: /* system processes */
                case ucproctype_user: /* all-user processes */
                case ucproctype_session: /* session processes */
                case ucproctype_group: /* group processes */
		    rs = selection() ;
		    n = rs ;
		    break ;
                default:
                    rs = SR_NOSYS ;
                    break ;
                } /* end switch */
            rs1 = fsdir_close(&d) ;
            if (rs >= 0) rs = rs1 ;
	} /* end if (fsdir) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (procer::fsdent) */

int procer::selection() noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	char		*pbuf{} ;
	if ((rs = libmalloc_mp(&pbuf)) >= 0) {
	    if ((rs = mkpath1(pbuf,dn)) >= 0) {
		cint    pl = rs ;
		while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
		    cint        ch = mkchar(de.name[0]) ;
		    if (isdigitlatin(ch)) {
		        cchar   *np = de.name ;
                        if ((rs = pathadd(pbuf,pl,np)) >= 0) {
                            USTAT       sb ;
                            if ((rs = u_stat(pbuf,&sb)) >= 0) {
				switch (w) {
				case ucproctype_all:
				    n += 1 ;
				    break ;
				case ucproctype_user:
				    if (sb.st_uid == uid) n += 1 ;
				    break ;
				case ucproctype_sys:
				    if (sb.st_uid < UIDSYS) n += 1 ;
				    break ;
				case ucproctype_session:
				case ucproctype_group:
				    uint	uv{} ;
				    if ((rs = cfdecui(pbuf,-1,&uv)) >= 0) {
					switch (w) {
					case ucproctype_session:
					    rs = u_getsid(uv) ;
				    	    if (rs == sid) n += 1 ;
					    break ;
					case ucproctype_group:
					    rs = u_getpgid(uv) ;
				    	    if (rs == pgid) n += 1 ;
					    break ;
					} /* end switch */
					if (rs == SR_SRCH) rs = SR_OK ;
				    } /* end if (cfdec) */
				    break ;
				} /* end switch */
                            } else if (isNotAccess(rs)) {
				rs = SR_OK ;
			    }
                        } /* end if (pathadd) */
		    } /* end if (is-digit) */
		    if (rs < 0) break ;
		} /* end while */
	    } /* end if (mkpath) */
	    rs1 = uc_libfree(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end method (procer::selection) */


