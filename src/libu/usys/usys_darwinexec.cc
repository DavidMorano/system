/* usys_darwinexec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrieve the full path of the current process executable file */
/* version %I% last-modified %G% */

#define	CF_PR		1		/* use environment program-root */
#define	CF_PATH		1		/* use environment PATH */

/* revision history:

	= 1998-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

	= 2011-03-19, David A-D- Morano
	This was updated to replace preprocessor defines with
	C++11 'if_constexpr'.  You have got to love those 
	preprocessor defines. Yes?

*/

/* Copyright © 1998,2011 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	darwin_execname

	Description:
	This subroutine retrieves (or tries to) the full path-name
	of the executable file of the current process.

	Synopsis:
	int darwin_execname(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>0		exec-file found and this is its length in result buffer
	==0		exec-file not found
	<0		error (system-return)

	Notes:
	1. Speed and execution efficiency under all execution paths
	is not of paramount importance.  This subroutine is likely
	only executed once in the whole program lifetime (and that
	at the start-up image-load time of the program).
	2. No surpise here (there is plenty of this everywehre),
	but I just wanted to mention that the Apple-Darwin operating
	system is especially messed up in that it is missing many
	of the features that we have come to expect from UNIX®-like
	operating systems.  OK, Darwin® is pretty much POSIX®
	compliant; but that is a very low bar now-a-days.  After
	some very extensive investigation of the Apple-Darwin source
	code, there appears to be only one semi-secret kernel
	subroutine interface that can supply the full path-name of
	the executable process, and that is the (relatively secret)
	|proc_info(2)| kernel call.  I use the undocumented
	|libproc(3c)| library to call |proc_info(2)| on my behalf
	(see the used subroutine |usys_pathpid(3usys)| below).
	3. Take a look at the amount of code below -- and the various
	subroutines called residing in other modules.  Ya, it is
	no joke how hard some things can be when there is not a
	rull and rich set of interfaces that we mear mortals have
	come to use and love.
	3. This subroutine is both Thread-Safe and Fork-Safe.  This
	was easy since the subroutine is a pure-function.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

/* USYS_DARWINEXEC start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#if	(! defined(SYSHAS_GETEXECNAME)) || (SYSHAS_GETEXECNAME == 0)

#include	<sys/types.h>		/* system types */
#include	<unistd.h>		/* |getpid(2)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| + |getprogname(3c)| */
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>		/* |libu::sncpy(3u)| */
#include	<localmisc.h>

#include	"usys_darwin.h"
#include	"usys_pathpid.h"	/* |usys_pathpid(3usys)| */

#if	defined(SYSHAS_LIBPROC) && (SYSHAS_LIBPROC > 0)
#define	F_LIBPROC	1		/* if you have |libproc| available */
#else
#define	F_LIBPROC	0		/* no |libproc| is available */
#endif

#ifndef	CF_PR
#define	CF_PR		1		/* use environment program-root */
#endif
#ifndef	CF_PATH
#define	CF_PATH		1		/* use environment PATH */
#endif

#define	VARUNDER	"_"
#define	VARPR		"PR"
#define	VARPATH		"PATH"
#define	MAXPATH		(4 * 1024)

import libutil ;			/* |lenstr(3u)| */
import umisc ;				/* |strbasename(3u)| + |mknpath(3u)| */
import usysbasic ;

using libu::sncpy ;			/* subroutine (usupport) */
using usys::usys_pathpid ;		/* subroutine (usys) */
using usys::darwin_getargz ;		/* subroutine (usys) */

cbool		f_libproc	= bool(F_LIBPROC) ;
cbool		f_pr		= bool(CF_PR) ;
cbool		f_path		= bool(CF_PATH) ;

constexpr int		rsnot[] = {
	SR_NOENT,
    	SR_ACCESS,
	SR_PERM,
	SR_STALE
} ; /* end array (rsnot) */

static cchar *strunder(cchar *sp) noex {
    	cnullptr	np{} ;
    	cchar		*rp ;
	constexpr cint	sch{'*'} ;
	if ((rp = strchr(sp,sch)) != np) {
	    if (*++rp) {
		if ((rp = strchr(rp,sch)) != np) {
		    if (*++rp == '\0') {
			rp = nullptr ;
		    }
		}
	    } else {
		rp = nullptr ;
	    }
	}
	return rp ;
} /* end subroutine (strunder) */

static bool isnotpresent(int rs) noex {
    	bool	f = false ;
	for (cauto &v : rsnot) {
	    if ((f = (rs == v))) break ;
	}
	return f ;
} /* end subroutine (isnotpesent) */

namespace {
    struct namer {
	cchar	*az ;
	char	*rbuf ;
	int	rlen ;
	namer(char *rb,int rl) noex : rbuf(rb), rlen(rl) { } ;
	operator int () noex ;
	int	abs() noex ;
	int	env() noex ;
	int	proc() noex ;
	int	pwd() noex ;
	int	pr() noex ;
	int	path() noex ;
    private:
	int	prmk(cc *,cc *,cc *) noex ;
	int	pathmk(cc *,int) noex ;
	int	verify(int = -1) noex ;
    } ; /* end struct (namer) */
    typedef int (namer::*namer_m)() ;
} /* end namespace */

constexpr namer_m	mems[] = {
    	&namer::abs,
    	&namer::env,
	&namer::proc,
	&namer::pwd,
	&namer::pr,
	&namer::path
} ;

namer::operator int () noex {
    	cnullptr	np{} ;
    	int		rs = SR_NOSYS ;
	if ((az = darwin_getargz()) != np) {
	    for (cauto &m : mems) {
	        rs = (this->*m)() ;
	        if (rs != 0) break ;
	    } /* end for */
	} /* end if (getprogname) */
	return rs ;
} /* end method (namer::operator) */

int namer::abs() noex {
    	int		rs = SR_OK ;
	if (az[0] == '/') {
	    if ((rs = sncpy(rbuf,rlen,az)) >= 0) {
		rs = verify(rs) ;
	    }
	}
	return rs ;
} /* end method (namer::abs) */

int namer::env() noex {
    	static cchar	*valp = getenv(VARUNDER) ;
	cnullptr	np{} ;
    	int		rs = SR_OK ;
	if (valp) {
	    if (valp[0] == '*') {
		if (cchar *rp ; (rp = strunder(valp)) != np) {
	            if ((rs = sncpy(rbuf,rlen,rp)) > 0) {
			rs = verify(rs) ;
		    }
		}
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (namer::env) */

int namer::proc() noex {
    	int		rs = SR_OK ;
	if_constexpr (f_libproc) {
	    const pid_t		pid = getpid() ;
	    rs = usys_pathpid(rbuf,rlen,pid) ;
	} /* end if_constexpr (f_libproc) */
	return rs ;
} /* end method (namer::proc) */

int namer::pwd() noex {
    	int		rs = SR_OK ;
	if (az[0] != '/') {
	    if ((rs = ugetcwd(rbuf,rlen)) >= 0) {
		int	rl = rs ;
		if (cint pl = lenstr(az) ; pl <= (rlen - (rl + 1))) {
		    rbuf[rl++] = '/' ;
		    strcpy((rbuf + rl),az) ;
		    rs = verify(rl + pl) ;
		}
	    }
	}
	return rs ;
} /* end method (namer::pwd) */

int namer::verify(int len) noex {
    	int		rs ;
	if (ustat sb ; (rs = ustatfile(rbuf,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode) && (sb.st_mode & 0111)) {
		rs = (len >= 0) ? len : lenstr(rbuf) ;
	    } else {
		rs = SR_OK ;
	    }
	} else if (isnotpresent(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end method (namer::verify) */

namespace usys {
    sysret_t darwin_execname(char *obuf,int olen) noex {
	int		rs = SR_FAULT ;
	if (obuf) {
	    rs = SR_INVALID ;
	    if (olen > 0) {
		namer	geto(obuf,olen) ;
		rs = geto ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (darwin_execname) */
} /* end namespace (usys) */

constexpr cpcchar	bins[] = {
    "bin",
    "sbin",
    "libexec"
} ; /* end array (bins) */

int namer::pr() noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	if_constexpr (f_pr) {
	    if (strchr(az,'/') == np) {
	        static cc	*valp = getenv(VARPR) ;
		if (valp) {
	            cint	am = (R_OK|X_OK) ;
	            if ((rs = udiraccess(valp,am)) >= 0) {
		        for (cauto &bin : bins) {
			    rs = prmk(valp,bin,az) ;
			    if (rs != 0) break ;
		        } /* end for */
	            } else if (isnotpresent(rs)) {
		        rs = SR_OK ;
	            }
		} /* end if (valp) */
	    } /* end if (have a bare prog-name) */
	} /* end if_constexpr (f_pr) */
    	return rs ;
} /* end method (namer::pr) */

int namer::prmk(cc *pr,cc *bin,cc *an) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = mknpath(rbuf,rlen,pr,bin,an)) >= 0) {
	    rs = verify(rs) ;
	    len = rs ;
	} /* end if (mknpath) */
    	return (rs >= 0) ? len : rs ;
} /* end method (namer::prmk) */

int namer::path() noex {
    	cnullptr	np{} ;
    	int		rs = SR_OK ;
	int		len = 0 ; /* return-path */
	if_constexpr (f_path) {
	    if (strchr(az,'/') == np) {
	        static cchar	*valp = getenv(VARPATH) ;
	        if (valp) {
		    cchar	*paths = valp ;
		    for (char *pp ; (pp = strchr(paths,'/')) != np ; ) {
			if (cint pl = intconv(pp - paths) ; pl > 0) {
			    rs = pathmk(pp,pl) ;
			    len = rs ;
			}
			paths = (pp + 1) ;
			if (rs != 0) break ;
		    } /* end for */
		    if ((rs == 0) && (len == 0) && paths[0]) {
			cint pl = lenstr(paths) ;
			rs = pathmk(paths,pl) ;
			len = rs ;
		    } /* end if (remainder) */
	        } /* end if (path) */
	    } /* end if (have a bare prog-name) */
	} /* end if_constexpr (f_path) */
	return (rs >= 0) ? len : rs ;
} /* end method (namer::path) */

int namer::pathmk(cc *pp,int pl) noex {
    	int		rs = SR_OK ;
	if (pl > 0) {
	    if ((rs = mknpathw(rbuf,rlen,pp,pl)) >= 0) {
		rs = verify(rs) ;
	    } /* end if (mknpathw) */
	} /* end if (non-zero positive) */
	return rs ;
} /* end method (namer::pathmk) */

#endif /* (! defined(SYSHAS_GETEXECNAME)) || (SYSHAS_GETEXECNAME == 0) */
#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYS_DARWINEXEC finish */


