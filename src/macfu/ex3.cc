/* main (haveprogram) */
/* lang=C++20 */

/* determine if a given name is an executable program */
/* version %I% last-modified %G% */


/* revision history:

	= 1989-03-01, David A.D. Morano
	This subroutine was originally written.

*/

/* Copyright © 1989 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	pathenum
	pathto
	haveprogram

	Description:
	Print out the home directory for the given user.

	Synopsis:
	$ pathenum
	$ pathto
	$ haveprogram

	Returns:
	0		for normal operation success
	126		haveprogram: some program found but not executable
	127		haveprogram: program not found

	Implementation-note:
	The whole purpose of this program (in all three forms) is
	to be able to be built without using any of the more elaborate
	system services that are normally provided by the libraries
	|libdam|, |libuc|, and |libu|.  Several minimalistic services
	are required from |libuc| and at least one (|u_stat(3u)|)
	from |libu|.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<string>
#include	<string_view>
#include	<vector>
#include	<iostream>
#include	<usystem.h>
#include	<varnames.hh>
#include	<strn.h>
#include	<sfx.h>
#include	<matstr.h>
#include	<strwcpy.h>
#include	<varnames.hh>
#include	<strnul.hh>
#include	<sncpyx.h>
#include	<isnot.h>
#include	<mapex.h>
#include	<getourenv.h>
#include	<exitcodes.h>
#include	<localmisc.h>


/* local defines */

#ifndef	MAXPATH
#define	MAXPATH		4096
#endif


/* local namespaces */

using std::nullptr_t ;
using std::nothrow ;
using std::cout ;
using std::clog ;
using std::string ;
using std::string_view ;
using std::vector ;


/* local typedefs */

typedef cpccharp	mainv ;
typedef string_view	strview ;


/* external subroutines */

extern "C" {
    extern int u_stat(cchar *,USTAT *) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct pathent {
	string		ps ;
	dev_t		dev ;
	ino_t		ino ;
	pathent() noex = default ;
	pathent(dev_t d,ino_t i,cchar *pp,int pl = -1) noex {
	    if (pl < 0) pl = strlen(pp) ;
	    strview	s(pp,pl) ;
	    try {
	        ps = s ;
	        dev = d ;
	        ino = i ;
	    } catch (...) {
		dev = 0 ;
		ino = 0 ;
	    }
	} ; /* end if (ctor) */
    } ; /* end struct (pathent) */
    enum proginfomems {
	proginfomem_start,
	proginfomem_finish,
	proginfomem_pathend,
	proginfomem_overlast
    } ;
    struct proginfo ;
    struct proginfo_co {
	proginfo	*op = nullptr ;
	int		w = -1 ;
	void operator () (proginfo *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0) noex ;
	operator int () noex {
	    return operator () (0) ;
	} ;
    } ; /* end struct (proginfo_co) */
    struct proginfo_pabeg {
	proginfo	*op = nullptr ;
	void init(proginfo *p) noex {
	    op = p ;
	} ;
	int operator () (cchar * = nullptr) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
    } ; /* end struct (proginfo_pabeg) */
    struct proginfo {
	typedef vector<pathent>::iterator	plit_t ;
	friend proginfo_co ;
	friend proginfo_pabeg ;
	proginfo_co	start ;
	proginfo_co	finish ;
	proginfo_co	pathend ;
	proginfo_pabeg	pathbegin ;
	mainv		argv ;
	mainv		envv ;
	cchar		*pn = nullptr ;
	vector<pathent>	pl ;
	int		argc ;
	int		pm ;
	bool		ffound = false ;
	proginfo(int c,mainv a,mainv e) noex : argc(c), argv(a), envv(e) { 
	    start(this,proginfomem_start) ;
	    finish(this,proginfomem_finish) ;
	    pathend(this,proginfomem_pathend) ;
	    pathbegin.init(this) ;
	} ;
	proginfo() noex : proginfo(0,nullptr,nullptr) { } ;
	void operator () (int c,mainv a,mainv e) noex {
	    argc = c ;
	    argv = a ;
	    envv = e ;
	} ;
	int pathcandidate(cchar *,int) noex ;
	int pathalready(dev_t,ino_t) noex ;
	int pathenum() noex ;
	int pathto() noex ;
    private:
	int istart() noexcept ;
	int ifinish() noexcept ;
	int ipathbegin(cchar *) noexcept ;
	int ipathend() noexcept ;
	int pathenumone(cchar *) noex ;
    } ; /* end struct (proginfo) */
}


/* forward references */


/* local variables */

enum progmodes {
	progmode_pathenum,
	progmode_pe,
	progmode_pathto,
	progmode_pt,
	progmode_haveprogram,
	progmode_overlast
} ;

static constexpr cpcchar	prognames[] = {
	"pathenum",
	"pe",
	"pathto",
	"pt",
	"haveprogram",
	nullptr
} ;

static constexpr MAPEX	mapexs[] = {
	{ SR_NOENT,	EX_NOUSER },
	{ SR_AGAIN,	EX_TEMPFAIL },
	{ SR_DEADLK,	EX_TEMPFAIL },
	{ SR_NOLCK,	EX_TEMPFAIL },
	{ SR_TXTBSY,	EX_TEMPFAIL },
	{ SR_ACCESS,	EX_NOPERM },
	{ SR_REMOTE,	EX_PROTOCOL },
	{ SR_NOSPC,	EX_TEMPFAIL },
	{ SR_INTR,	EX_INTR },
	{ SR_EXIT,	EX_TERM },
	{ SR_NOMSG,	EX_OSERR },
	{ SR_NOSYS,	EX_OSFILE },
	{ 0, 0 }
} ;

constexpr int		maxpathlen = MAXPATH ;


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) noexcept {
	proginfo	pi(argc,argv,envv) ;
	int		ex = EX_OK ;
	int		rs ;
	int		rs1 ;
	if ((rs = pi.start) >= 0) {
            switch (pi.pm) {
            case progmode_pathenum:
            case progmode_pe:
                rs = pi.pathenum() ;
                break ;
            case progmode_pt:
                pi.pm = progmode_pathto ;
		fallthrough ;
                /* FALLTHROUGH */
            case progmode_pathto:
                rs = pi.pathto() ;
                break ;
            case progmode_haveprogram:
                if ((rs = pi.pathto()) == 0) {
                    ex = (pi.ffound) ? EX_NOEXEC : EX_NOPROG ;
                } else if (rs < 0) {
                    switch (rs) {
                    case SR_NOTFOUND:
                        ex = EX_NOPROG ;
                        break ;
                    case SR_ACCESS:
                        ex = EX_NOEXEC ;
                        break ;
                    } /* end switch */
                }
                break ;
            } /* end switch */
	    rs1 = pi.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (proginfo) */
	if ((rs < 0) && (ex == EX_OK)) ex = mapex(mapexs,rs) ;
	return ex ;
}
/* end subroutine (main) */


/* local subroutines */

int proginfo::istart() noexcept {
	int		rs = SR_FAULT ;
	if (argv) {
	    rs = SR_NOMSG ;
	    if ((argc > 0) && argv[0]) {
	        int	bl ;
	        cchar	*bp{} ;
		rs = SR_NOSYS ;
	        if ((bl = sfprogname(argv[0],-1,&bp)) > 0) {
	            if ((pm = matstr(prognames,bp,bl)) >= 0) {
			pn = prognames[pm] ;
		        rs = SR_OK ;
	            }
		} /* end if (have base-name) */
	    } /* end if (have first argument) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (proginfo::istart) */

int proginfo::ifinish() noex {
	return SR_OK ;
}
/* end method (proginfo::ifinish) */

int proginfo::ipathbegin(cchar *vn) noex {
	int		rs = SR_FAULT ;
	if (vn) {
	    rs = SR_INVALID ;
	    if (vn[0]) {
	        cchar	*valp ;
		rs = SR_OK ;
	        if ((valp = getourenv(envv,vn)) != nullptr) {
	            cchar	*sp = valp ;
	            char	*tp ;
	            int		sl = strlen(sp) ;
	            while ((tp = strnchr(sp,sl,':')) != nullptr) {
	 	        rs = pathcandidate(sp,(tp-sp)) ;
		        sl -= ((tp+1) - sp) ;
		        sp = (tp+1) ;
		        if (rs < 0) break ;
	            } /* end while */
	            if ((rs >= 0) && (sl > 0)) {
	 	        rs = pathcandidate(sp,sl) ;
	            }
	        } /* end if (got value) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (proginfo::ipathbegin) */

int proginfo::pathcandidate(cchar *sp,int sl) noex {
	USTAT		sb ;
	strnul		s(sp,sl) ;
	int		rs ;
	if ((rs = u_stat(s,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        const dev_t	d = sb.st_dev ;
	        const ino_t	i = sb.st_ino ;
	        if ((rs = pathalready(d,i)) == 0) {
		    pathent	e(d,i,sp,sl) ;
		    try {
			if ((e.dev != 0) && (e.ino != 0)) {
		            pl.push_back(e) ;
			} else {
			    rs = SR_NOMEM ;
			}
		    } catch (...) {
			rs = SR_NOMEM ;
		    }
	        } /* end if (not-already) */
	    } /* end if (is-dir) */
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end method (proginfo::pathcandidate) */

int proginfo::ipathend() noex {
	pl.clear() ;
	return SR_OK ;
}
/* end method (proginfo::ipathend) */

int proginfo::pathalready(dev_t d,ino_t i) noex {
	int		rs = SR_OK ;
	bool		f = false ;
	for (auto const &e : pl) {
	    f = ((e.dev == d) && (e.ino == i)) ;
	    if (f) break ;
	} /* end for */
	return (rs >= 0) ? f : rs ;
}
/* end method (proginfo::pathalready) */

int proginfo::pathenum() noex {
	int		rs ;
	if (argc > 1) {
	    rs = SR_OK ;
	    for (int ai = 1 ; (rs >= 0) && (ai < argc) && argv[ai] ; ai += 1) {
	        cchar	*vn = argv[ai] ;
		if (vn[0]) {
	            rs = pathenumone(vn) ;
		}
	    } /* end for (looping over program arguments) */
	} else {
	    cchar	*vn = varname.path ;
	    rs = pathenumone(vn) ;
	} /* end if (program arguments) */
	return rs ;
}
/* end subroutine (proginfo::pathenum) */

int proginfo::pathenumone(cchar *vn) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pathbegin(vn)) >= 0) {
	    for (auto const &e : pl) {
		cout << e.ps << eol ;
	    } /* end for */
	    rs1 = pathend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (proginfo::path) */
	return rs ;
}
/* end subroutine (proginfo::pathenumone) */

int proginfo::pathto() noex {
	int		rs ;
	int		rs1 ;
	int		ctotal = 0 ;
	if ((rs = pathbegin) >= 0) {
	    if ((rs = maxpathlen) >= 0) {
	        cint	pm_pt = progmode_pathto ;
	        cint	pm_hp = progmode_haveprogram ;
	        cint	plen = rs ;
	        char	*pbuf ;
	        rs = SR_NOMEM ;
	        if ((pbuf = new(nothrow) char[plen+1]) != nullptr) {
		    rs = SR_OK ;
	            for (int ai = 1 ; (ai < argc) && argv[ai] ; ai += 1) {
	                cchar	*ap = argv[ai] ;
		        int	c = 0 ;
	                if (ap[0]) {
			    ffound = false ;
	                    for (auto const &e : pl) {
			        cchar	*pp = e.ps.c_str() ; /* <- throw? */
			        if ((rs = sncpy(pbuf,plen,pp,"/",ap)) >= 0) {
				    USTAT	sb ;
			 	    if ((rs = u_stat(pbuf,&sb)) >= 0) {
				        cmode	fm = sb.st_mode ;
				        ffound = true ;
				        if (S_ISREG(fm)) {
					    cmode	xxx = 0111 ;
					    if ((fm & xxx) == xxx) {
					        c += 1 ;
					        if (pm == pm_pt) {
	                                            cout << pbuf << eol ;
					        }
					    } /* end if is-exec) */
				        } /* end if (is-reg) */
				    } else if (isNotAccess(rs)) {
				        rs = SR_OK ;
				    } /* end if (u_stat) */
			        } /* end if (sncpyx) */
			        if ((pm == pm_hp) && (c > 0)) break ;
			        if (rs < 0) break ;
	                    } /* end for (paths) */
			    if ((rs >= 0) && (pm == pm_hp) && (c == 0)) {
			        rs = (ffound) ? SR_ACCESS : SR_NOTFOUND ;
			    } /* end if */
	                } /* end if (non-empty) */
		        ctotal += c ;
		        if (rs < 0) break ;
	            } /* end for (args) */
	            delete [] pbuf ;
	        } /* end if (m-a-f) */
	    } /* end if (maxpathlen) */
	    rs1 = pathend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (proginfo::path) */
	return (rs >= 0) ? ctotal : rs ;
}
/* end subroutine (proginfo::pathto) */

int proginfo_co::operator () (int) noex {
	int	rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case proginfomem_start:
	        rs = op->istart() ;
	        break ;
	    case proginfomem_finish:
	        rs = op->ifinish() ;
	        break ;
	    case proginfomem_pathend:
	        rs = op->ipathend() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (proginfo_co::operator) */

int proginfo_pabeg::operator () (cchar *vn) noex {
	int	rs = SR_BUGCHECK ;
	if (op) {
	    if (vn == nullptr) vn = varname.path ;
	    rs = op->ipathbegin(vn) ;
	}
	return rs ;
}
/* end method (proginfo_pabeg::operator) */


