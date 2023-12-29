/* main (fu) */
/* lang=C++20 */

/* filter filenames */
/* version %I% last-modified %G% */


/* revision history:

	= 1989-03-01, David A.D. Morano
	This subroutine was originally written.

*/

/* Copyright © 1989 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	fu

	Description:
	This program filters filenames keeping only unique files.

	Synopsis:
	$ fu [<file(s)>] [-]

	Arguments:
	<file(s)>	file(s) to process
	-		read standard-input for file(s) to process

	Returns:
	0		for normal operation success
	126		haveprogram: some program found but not executable
	127		haveprogram: program not found

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
#include	<getourenv.h>
#include	<mapex.h>
#include	<mapblock.hh>
#include	<readln.hh>
#include	<exitcodes.h>
#include	<localmisc.h>


/* local defines */

#ifndef	MAXPATH
#define	MAXPATH		4096
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */
using std::cin;				/* variable */
using std::cout ;			/* variable */
using std::string ;			/* type */
using std::string_view ;		/* type */
using std::unordered_map ;		/* type */
using std::vector ;			/* type */


/* local typedefs */

typedef string_view	strview ;


/* external subroutines */

extern "C" {
    extern int u_stat(cchar *,USTAT *) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct devino {
	dev_t		dev ;
	ino_t		ino ;
	devino(dev_t d,ino_t i) noex : dev(d), ino(i) { } ;
	size_t hashval() const noex {
	    return size_t(dev | ino) ;
	} ;
    } ; /* end struct (devino) */
    struct filenode {
	string		fn ;
	int		rc = 0 ;
	filenode() noex = default ;
	filenode(cchar *pp,int pl = -1) noex {
	    if (pl < 0) pl = strlen(pp) ;
	    strview	s(pp,pl) ;
	    try {
	        fn = s ;
		rc = 1 ;
	    } catch (...) {
		rc = 0 ;
	    }
	} ; /* end if (ctor) */
    } ; /* end struct (filenode) */
    enum proginfomems {
	proginfomem_start,
	proginfomem_finish,
	proginfomem_flistbegin,
	proginfomem_flistend,
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
    struct proginfo {
	typedef mapblock<devino,filenode>		nodedb ;
	typedef mapblock<devino,filenode>::iterator	plit_t ;
	friend proginfo_co ;
	proginfo_co	start ;
	proginfo_co	finish ;
	proginfo_co	flistbegin ;
	proginfo_co	flistend ;
	mainv		argv ;
	mainv		envv ;
	cchar		*pn = nullptr ;
	nodedb		flist ;
	int		argc ;
	int		pm = 0 ;
	bool		ffound = false ;
	proginfo(int c,mainv a,mainv e) noex : argc(c), argv(a), envv(e) { 
	    start(this,proginfomem_start) ;
	    finish(this,proginfomem_finish) ;
	    flistbegin(this,proginfomem_flistbegin) ;
	    flistend(this,proginfomem_flistend) ;
	} ;
	proginfo() noex : proginfo(0,nullptr,nullptr) { } ;
	void operator () (int c,mainv a,mainv e) noex {
	    argc = c ;
	    argv = a ;
	    envv = e ;
	} ;
	int filecandidate(cchar *,int = -1) noex ;
	int filealready(dev_t,ino_t) noex ;
    private:
	int istart() noex ;
	int ifinish() noex ;
	int getpn(mainv) noex ;
	int iflistbegin() noex ;
	int iflistend() noex ;
	int readin() noex ;
    } ; /* end struct (proginfo) */
    template<> struct std::hash<devino> {
	size_t operator() (const devino &di) const noex {
	    return di.hashval() ;
	} ;
    } ; /* end struct-template (hash<devino>) */
}


/* forward references */


/* local variables */

enum progmodes {
	progmode_fileuniq,
	progmode_fu,
	progmode_overlast
} ;

static constexpr cpcchar	prognames[] = {
	"fileuniq",
	"fu",
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

int main(int argc,mainv argv,mainv envv) noex {
	proginfo	pi(argc,argv,envv) ;
	int		ex = EX_OK ;
	int		rs ;
	int		rs1 ;
	if ((rs = pi.start) >= 0) {
	    if ((rs = pi.flistbegin()) >= 0) {
                switch (pi.pm) {
                case progmode_fileuniq:
                case progmode_fu:
                    rs = pi.output() ;
                    break ;
                } /* end switch */
		rs1 = pi.flistend() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (flist-) */
	    rs1 = pi.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (proginfo) */
	if ((rs < 0) && (ex == EX_OK)) ex = mapex(mapexs,rs) ;
	return ex ;
}
/* end subroutine (main) */


/* local subroutines */

int proginfo::istart() noex {
	int		rs ;
	if ((rs = getpn(prognames)) >= 0) {
	    rs = 0 ;
	} /* end if (proginfo::getpn) */
	return rs ;
}
/* end method (proginfo::istart) */

int proginfo::ifinish() noex {
	return SR_OK ;
}
/* end method (proginfo::ifinish) */

int proginfo::getpn(mainv names) noex {
	int		rs = SR_FAULT ;
	if (argv) {
	    rs = SR_NOMSG ;
	    if ((argc > 0) && argv[0]) {
	        int	bl ;
		cchar	*arg0 = argv[0] ;
	        cchar	*bp{} ;
	        if ((bl = sfbasename(argv0,-1,&bp)) > 0) {
		    int		pl = rmchr(bp,bl,'.') ;
		    cchar	*pp = bp ;
		    if (pl > 0) {
	                if ((pm = matstr(names,pp,pl)) >= 0) {
			    pn = names[pm] ;
		            rs = pm ;
	                }
		    } /* end if (non-zero positive progname) */
		} /* end if (have base-name) */
	    } /* end if (have first argument) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (proginfo::getpn) */

int proginfo::iflistbegin() noex {
	int		rs = SR_OK ;
	if (argc > 1) {
	    for (int ai = 1 ; (rs >= 0) && (ai < argc) ; ai += 1) {
		cchar	*fn = argv[ai] ;
		if (fn[0]) {
		    if ((fn[0] == '-') && (fn[1] == '\0')) {
			rs = readin() ;
		    } else {
			rs = filecandidate(fn) ;
		    }
		} /* end if */
	    } /* end for */
	} else {
	    rs = readin() ;
	}
	return rs ;
}
/* end method (proginfo::iflistbegin) */

int proginfo::readin() noex {
	cint		llen = MAXPATH ;
	int		rs = SR_NOMEM ;
	char		*lbuf ;
	if ((lbuf = new(nothrow) char[llen+1]) != nullptr) {
	    while ((rs = readln(cin,lbuf,llen)) >= 0) {
		int	ll = rs ;
		if ((ll > 0) && (lbuf[ll - 1] == eol)) ll -= 1 ;
		if (ll > 0) {
		    rs = flistenter(lbuf,ll) ;
		}
		if (rs < 0) break ;
	    } /* end if (reading lines) */
	    delete [] lbuf ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end method (proginfo::readin) */

int proginfo::filecandidate(cchar *sp,int sl) noex {
	USTAT		sb ;
	strnul		s(sp,sl) ;
	int		rs ;
	if ((rs = u_stat(s,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        const dev_t	d = sb.st_dev ;
	        const ino_t	i = sb.st_ino ;
	        if ((rs = filealready(d,i)) == 0) {
		    devino	di(d,i) ;
		    filenode	e(sp,sl) ;
		    rs = flist.ins(di,e) ;
	        } /* end if (not-already) */
	    } /* end if (is-dir) */
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end method (proginfo::filecandidate) */

int proginfo::iflistend() noex {
	flist.clear() ;
	return SR_OK ;
}
/* end method (proginfo::iflistend) */

int proginfo::filealready(dev_t d,ino_t i) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	bool		f = false ;
	devino		di(d,i) ;
	if ((rs = flist.present(di)) == rsn) {
	    rs = SR_OK ;
	    f = true ;
	} /* end if (mapblock::present) */
	return (rs >= 0) ? f : rs ;
}
/* end method (proginfo::filealready) */

int proginfo::output() noex {
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
/* end subroutine (proginfo::output) */

int proginfo::pathenumone(cchar *vn) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = flistbegin(vn)) >= 0) {
	    for (auto const &e : pl) {
		cout << e.ps << eol ;
	    } /* end for */
	    rs1 = flistend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (proginfo::path) */
	return rs ;
}
/* end subroutine (proginfo::pathenumone) */

int proginfo::pathto() noex {
	int		rs ;
	int		rs1 ;
	int		ctotal = 0 ;
	if ((rs = flistbegin) >= 0) {
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
	    rs1 = flistend ;
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
	    case proginfomem_flistbegin:
	        rs = op->iflistend() ;
	        break ;
	    case proginfomem_flistend:
	        rs = op->iflistend() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (proginfo_co::operator) */


