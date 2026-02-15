/* modproc1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load module names from a C++ source code file */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-03-21, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	modprocload

	Description:
	I parse a C++ source code file and find all of the module
	names that I can that are being imported to that source file.

	Synopsis:
	modprocload(vecstr *op,cchar *fname) noex

	Arguments:
	op		ureserve::vecstr
	fname		file-name to parse

	Returns:
	>=0		number of imported modules found
	<0		error (system-return)

	Notes:
	1. The handling of line continuations is quite poor (if I have
	to say it myself).  I should have used a C++ |string(3c++|
	object rather than the fixed (although quite large) buffer
	that I am using.  In the end laziness about not changing
	existing code rules the day (which practice I usually quite 
	despised back in the 1980s).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>		/* |O_{xx}| */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<strnul.hh>
#include	<langparse.h>
#include	<ascii.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<six.h>			/* |sispanwht(3uc)| */
#include	<strmgr.h>
#include	<strop.h>
#include	<strwcmp.h>
#include	<ascii.h>		/* |CH_NL| */
#include	<mkchar.h>
#include	<hasx.h>		/* |headlead(3uc)| */
#include	<localmisc.h>
#include	<debprintf.h>		/* |DEBPRINTF(3u)| */

#pragma		GCC dependency		"mod/modproc.ccm"
#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

module modproc ;

import libutil ;			/* |getlenstr(3u)| */
import ulibvals ;			/* |ulibval(3u)| + |pagesz| */

/* local defines */

#define	ALTLNMULT	5		/* multplier to pagesz for alt-line */

#define CDEBPR(FMT, ...) \
	if_constexpr (f_debug) \
    	debprintf(__func__, FMT __VA_OPT__(,) __VA_ARGS__)


/* imported namespaces */

using libu::um ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct modmgr {
	vecstr		*vop ;
	char		*lbuf{} ;
	short		*sbuf{} ;
	langparse	procer ;
	strmgr		lnmgr ;
	int		llen = 0 ;
	int		slen = 0 ;
	int		li = 0 ;
	bool		flnalt = false ;
	modmgr(vecstr *p) noex : vop(p) { } ;
	int operator () (cchar *) noex ;
	int start() noex ;
	int finish() noex ;
	int fileload(cchar *) noex ;
	int fileproc() noex ;
	int filter(char *,int,int) noex ;
	int liner(int,size_t) noex ;
	int checker(int,cchar *,int) noex ;
	int procln(int,cchar *,int) noex ;
    private:
	int lnbegin() noex ;
	int lnend() noex ;
	int lnready() noex ;
	int lnadd(cchar *,int) noex ;
	int lncheck(int) noex ;
	int swbegin() noex ;
	int swend() noex ;
    } ; /* end struct (modmgr) */
} /* end namespace */


/* forward references */

local bool haspound(cchar *sp,int sl) noex {
    	return haslead(sp,sl,'#') ;
} /* end subroutine (haspound) */


/* local variables */

cchar			istr[] = "import" ;


/* exported variables */


/* exported subroutines */

int modprocload(vecstr *op,cchar *fname) noex {
    	int		rs = SR_FAULT ;
	CDEBPR("ent fn=%s\n",fname) ;
	if (op && fname) ylikely {
    	    modmgr	mgr(op) ;
	    rs = mgr(fname) ;
	} /* end if (non-null) */
	CDEBPR("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (modprocload) */


/* local subroutines */

int modmgr::operator () (cchar *fname) noex {
    	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	CDEBPR("ent fn=%s\n",fname) ;
	if ((rs = start()) >= 0) {
	    if ((rs = procer.start) >= 0) {
	        if ((rs = fileload(fname)) >= 0) {
		    rs = fileproc() ;
		    rv = rs ;
	            CDEBPR("fileproc() rs=%d\n",rs) ;
	        } /* end if (fileload) */
	        rs1 = procer.finish ;
	        if (rs >= 0) rs = rs1 ;
	        CDEBPR("procer.finish() rs=%d\n",rs) ;
	    } /* end if (langproc) */
	    rs1 = finish() ;
	    if (rs >= 0) rs = rs1 ;
	    CDEBPR("finish() rs=%d\n",rs) ;
	} /* end if (start-finish) */
	CDEBPR("ret rs=%d rc=%d\n",rs,rv) ;
    	return (rs >= 0) ? rv : rs ;
} /* end method (modmgr::operator) */

int modmgr::start() noex {
	return swbegin() ;
} /* end method (modmgr::start) */

int modmgr::finish() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (lbuf) {
    	    rs1 = lnend() ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (sbuf) {
	    rs1 = swend() ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end method (modmgr::finish) */

int modmgr::fileload(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		nmods = 0 ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		cint	of = O_RDONLY ;
		if ((rs = u_open(fname,of,0)) >= 0) ylikely {
		    cint	fd = rs ;
		    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
			csize	fsize = size_t(sb.st_size) ;
			rs = SR_NOTSUP ;
		        if (S_ISREG(sb.st_mode)) ylikely {
			    rs = SR_OK ;
			    if (fsize > 0) {
			        rs = liner(fd,fsize) ;
			        nmods = rs ;
			    }
			} /* end if (regular file) */
		    } /* end if (stat) */
		    rs1 = u_close(fd) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? nmods : rs ;
} /* end method (modmgr::fileload) */

int modmgr::liner(int fd,csize ms) noex {
	cnullptr	np{} ;
	cint		mp = PROT_READ ;
	cint		mf = MAP_SHARED ;
	int		rs ;
	int		rs1 ;
	int		nmods = 0 ; /* return-value */
	CDEBPR("ent\n") ;
	if (void *md ; (rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	    cint	cmd = MADV_SEQUENTIAL ;
	    if ((rs = u_madvise(md,ms,cmd)) >= 0) ylikely {
		size_t	ll = ms ;
		int	ln = 1 ;
		cchar	*lp = charp(md) ;
		for (cchar *tp ; (tp = charp(memchr(lp,'\n',ll))) != np ; ) {
		    csize	si = ((tp + 1) - lp) ;
		    if (int sl = intconv(si - 1) ; sl > 0) {
			cchar *sp = lp ;
	    		if (sp[sl - 1] == '\r') {
			    sl -= 1 ;
	    		}
	    		if (sl > 0) {
			    if (sp[sl - 1] == CH_BSLASH) {
			        rs = lnadd(sp,(sl - 1)) ;
			    } else {
			        if (flnalt) {
			            if ((rs = lnadd(sp,sl)) >= 0) {
				        rs = lncheck(ln++) ;
		                        nmods += rs ;
				    }
			        } else {
		                    rs = checker(ln++,sp,sl) ;
		                    nmods += rs ;
			        }
			    } /* end if (continuation or regular) */
			} /* end if (still not-empty) */
		    } /* end if (non-empty) */
		    ll -= si ;
		    lp += si ;
		    if (rs < 0) break ;
		} /* end for */
		if ((rs >= 0) && (ll > 0)) {
		    cint	sl = intconv(ll) ;
		    rs = checker(ln++,lp,sl) ;
		    nmods += rs ;
		} /* end if (trailing line */
	    } /* end if (memory-advise) */
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (map-file) */
	CDEBPR("ret rs=%d c=%d\n",rs,nmods) ;
	return (rs >= 0) ? nmods : rs ;
} /* end method (modmgr::liner) */

int modmgr::checker(int ln,cchar *lp,int ll) noex {
    	int		rs = SR_OK ;
	if_constexpr (f_debug) {
	    strnul s(lp,ll) ;
	    CDEBPR("ent ln=%d l=>%s<\n",ln,ccp(s)) ;
	}
	if ((ll > 0) && (! haspound(lp,ll))) {
	    if ((ll > 0) && (lp[ll - 1] == '\r')) {
		ll -= 1 ;
	    }
	    if (ll > 0) {
	        rs = procer.load(lp,ll) ;
	        CDEBPR("procer.proc() rs=%d\n",rs) ;
	    }
	} /* end if (not preprocessor directive) */
	CDEBPR("ret rs=%d\n",rs) ;
    	return rs ; 
} /* end method (modmgr::checker) */

int modmgr::fileproc() noex {
    	cnullptr	np{} ;
    	cint		chnl = CH_NL ;
    	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	CDEBPR("ent\n") ;
	if (char *tbuf ; (rs = um.mall((slen + 1),&tbuf)) >= 0) {
	    cint tlen = rs ;
	    for (int ln = 1 ; (rs = procer.remread(sbuf,slen)) > 0 ; ) {
		if ((rs = filter(tbuf,tlen,rs)) >= 0) {
		    int		ll = rs ;
		    cchar	*lp = tbuf ;
		    for (cchar *tp ; (tp = strnchr(lp,ll,chnl)) != np ; ) {
			cint tl = intconv(tp - lp) ;
			CDEBPR("bef tl=%d\n",tl) ;
	                rs = procln(ln++,lp,tl) ;
	                rv += rs ;
	                if (rs < 0) break ;
			ll -= intconv((tp + 1) - lp) ;
			lp = (tp + 1) ;
		    } /* end for */
		    if ((rs >= 0) && (ll > 0)) {
			CDEBPR("aft ll=%d\n",ll) ;
	                rs = procln(ln++,lp,ll) ;
	                rv += rs ;
		    } /* end if (reminder) */
		} /* end if (filter) */
	    } /* end for */
	    rs1 = um.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	CDEBPR("ret rs=%d rv=%d\n",rs,rv) ;
    	return (rs >= 0) ? rv : rs ;
} /* end method (modmgr::fileproc) */

int modmgr::filter(char *tbuf,int tlen,int sl) noex {
    	cauto		&lm = langparsem ;
    	int		rs = SR_OK ;
	int		rl = 0 ;
	for (int i = 0 ; (i < sl) && (i < tlen) ; i += 1) {
	    if (cshort cw = sbuf[i] ; (! (cw & lm.comment))) {
		tbuf[rl++] = char(cw) ;
	    }
	} /* end for */
	return (rs >= 0) ? rl : rs ;
} /* end method (modmgr::filter) */

int modmgr::procln(int ln,cchar *lp,int ll) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value -- number of names found */
	{
	    strnul st(lp,ll) ;
	    CDEBPR("ent ln=>%s<\n",ccp(st)) ;
	}
	(void) ln ;
    	if (strop s ; (rs = s.start(lp,ll)) >= 0) ylikely {
	    cchar	*ip ;
	    if (int il ; (il = s.fieldwht(&ip)) > 0) {
		if_constexpr (f_debug) {
	    	    strnul st(ip,il) ;
	    	    CDEBPR("piece=>%s<\n",ccp(st)) ;
		}
		if (strwcmp(istr,ip,il) == 0) {
		    cint	ch_s = CH_SEMI ;
		    cchar	*mp ;
		    CDEBPR("cmp\n") ;
		    if (int ml ; (ml = s.fieldchr(ch_s,&mp)) > 0) {
			if_constexpr (f_debug) {
	    	    	    strnul st(mp,ml) ;
	    	    	    CDEBPR("m=>%s<\n",ccp(st)) ;
			}
			if (hasmodname(mp,ml)) {
			    rs = vop->adduniq(mp,ml) ;
			    c = (rs < INT_MAX) ;
			}
		    } /* end if (strop_fieldchr) */
	        } /* end if (got "import") */
	    } /* end if (got field) */
	    rs1 = s.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (strop) */
	CDEBPR("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (modmgr::procln) */

int modmgr::lnbegin() noex {
    	int		rs = SR_OK ;
	if (lbuf == nullptr) {
	    if ((rs = ulibval.pagesz) >= 0) {
		cint	sz = (ALTLNMULT * rs) ;
		if (char *bp ; (rs = um.mall(sz,&bp)) >= 0) {
		    lbuf = bp ;
		    llen = sz ;
		    rs = lnmgr.start(lbuf,llen) ;
		    if (rs < 0) {
			um.free(lbuf) ;
			lbuf = nullptr ;
			llen = 0 ;
		    } /* end if (error) */
		} /* end if (memory-allocation) */
	    } /* end if (ulibvals) */
	} /* end if (required) */
	return rs ;
} /* end method (modmgr::lnbegin) */

int modmgr::lnend() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (lbuf) {
	    {
		rs1 = lnmgr.finish ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = um.free(lbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        lbuf = nullptr ;
	        llen = 0 ;
	    }
	    li = 0 ;
	} /* end if (was initialized) */
	return rs ;
} /* end method (modmgr::lnend) */

int modmgr::lnready() noex {
    	int		rs = SR_OK ;
	if (lbuf == nullptr) {
	    rs = lnbegin() ;
	}
	return rs ;
} /* end method (modmgr::lnready) */

int modmgr::lnadd(cchar *sp,int sl) noex {
    	int		rs ;
	if ((rs = lnready()) >= 0) {
	    if ((rs = lnmgr.str(sp,sl)) > 0) {
	        flnalt = true ;
	    } /* end if (lnadd) */
	} /* end if (lnready) */
	return rs ;
} /* end method (modmgr::lnadd) */

int modmgr::lncheck(int ln) noex {
    	int		rs ;
	if (cchar *cp ; (rs = lnmgr.get(&cp)) > 0) {
	    if ((rs = checker(ln,cp,rs)) >= 0) {
	        if ((rs = lnmgr.reset) >= 0) {
	            flnalt = false ;
	        }
	    } /* endif (checker) */
	} /* end if (lnmgr.get) */
	return rs ;
} /* end method (modmgr::lncheck) */

int modmgr::swbegin() noex {
    	int		rs = SR_OK ;
	if (sbuf == nullptr) {
	    if ((rs = ulibval.pagesz) >= 0) {
		cint	sz = (ALTLNMULT * rs) * szof(short) ;
		if (void *bp ; (rs = um.mall(sz,&bp)) >= 0) {
		    sbuf = shortp(bp) ;
		    slen = (ALTLNMULT * rs) ;
		} /* end if (memory-allocation) */
	    } /* end if (ulibvals) */
	} /* end if (required) */
	return rs ;
} /* end method (modmgr::swbegin) */

int modmgr::swend() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (sbuf) {
	    {
	        rs1 = um.free(sbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        sbuf = nullptr ;
	        slen = 0 ;
	    }
	} /* end if (was initialized) */
	return rs ;
} /* end method (modmgr::swend) */


