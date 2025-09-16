/* vecstrx_envfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process an environment file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstrx_envfile

	Description:
	This subroutine will read (process) an environment file and
	put any newly encountered environment variables into the
	string list (supplied).  Variables found that already exist
	are ignored.

	Synopsis:
	int vecstrx_envfile(vecstrx *vsp,cchar *fname) noex

	Arguments:
	vsp		vecstrx-string object to accumulate results
	fname		file to process

	Returns:
	>=0		count of environment variables
	<0		bad

	- Implementation note:
	Although this function is (overall) rather simple, I put
	some slightly cleaver thought into parsing the lines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>		/* |strncasecmp(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<utimeout.h>
#include	<filer.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<sfx.h>
#include	<strcpyx.h>
#include	<strdcpyxw.h>
#include	<vstrkeycmpx.h>
#include	<localmisc.h>

#include	"vecstrx.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;
import ulibvals ;			/* |libval.maxline(3u)| */

/* local defines */

#define	LINEBUFMULT	5		/* line-buffer size multiplier */

#define	WORDEXPORT	"export"	/* optional word to ignore */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (*rls_f)(filer *,char *,int,int,int *) noex ;
    typedef int (*vs_f)(cchar **,cchar **) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct subinfo {
	vecstrx		*vsp ;
	cchar		*ft ;		/* field-terms */
	char		*a ;		/* allocation */
	char		*lbuf ;
	char		*ebuf ;
	int		llen ;
	int		elen ;
	subinfo(vecstrx *p,cchar *t) noex : vsp(p), ft(t) { } ;
	int start() noex ;
	int finish() noex ;
	int line(cchar *,int) noex ;
	int parse(field *,cchar *,int) noex ;
    } ; /* end struct (subinfo) */
}

namespace {
    struct eword {
	int		l ;
	static cchar	p[] ;
	constexpr eword() noex {
	    l = lenstr(WORDEXPORT) ;
	} ;
    } ;
    constexpr char eword::p[] = WORDEXPORT ;
    struct vars {
	int		linebuflen ;
	operator int () noex ;
    } ;
}


/* forward references */

static int	vecstrx_envfiler(vecstrx *,cchar *) noex ;
static int	mkinit() noex ;
static int	mkterms() noex ;


/* local variables */

static int		maxline = ulibval.maxline ;
static vars		var ;
static char		fterms[fieldterms_termsize] ;


/* exported variables */


/* exported subroutines */

int vecstrx::envfile(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		static cint	rsi = mkinit() ;
		if ((rs = rsi) >= 0) {
		    rs = vecstrx_envfiler(this,fname) ;
		    c = rs ;
		} /* end if (mkinit) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_envfile) */


/* local subroutines */

int vecstrx_envfiler(vecstrx *op,cchar *fname) noex {
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (subinfo si(op,fterms) ; (rs = si.start()) >= 0) {
            cmode   om = 0666 ;
            cint    of = O_RDONLY ;
            if ((rs = u_open(fname,of,om)) >= 0) {
                cint		fd = rs ;
                if (filer df ; (rs = df.start(fd,0z,0,0)) >= 0) {
                    cint    to = utimeout[uto_read] ;
                    cint    llen = si.llen ;
                    char    *lbuf = si.lbuf ;
                    while ((rs = df.readlns(lbuf,llen,to,np)) > 0) {
			cchar	*cp{} ;
			if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
                            rs = si.line(cp,cl) ;
                            c += rs ;
                        }
                        if (rs < 0) break ;
                    } /* end while (reading lines) */
                    rs1 = df.finish ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (filer) */
                rs1 = u_close(fd) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (file) */
            rs1 = si.finish() ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (subinfo) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_envfiler) */

int subinfo::start() noex {
	int		rs ;
	int		sz = 0 ;
	llen = var.linebuflen ;
	sz += (2 * (llen + 1)) ;
	if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) {
	    a = bp ;
	    lbuf = bp ;
	    bp += (llen + 1) ;
	    ebuf = bp ;
	    elen = llen ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (subinfo::start) */

int subinfo::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (a) {
	    rs1 = libmem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

int subinfo::line(cchar *lp,int ll) noex {
	static constexpr eword	ew ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    cchar	*fp{} ;
	    if (int fl ; (fl = fsb.get(ft,&fp)) > 0) {
	        if ((fl == ew.l) && (strncasecmp(ew.p,fp,fl) == 0)) {
	            fl = fsb.get(ft,&fp) ;
	        }
	        if (fl > 0) {
	            rs = parse(&fsb,fp,fl) ;
	            c = rs ;
	        }
	    } /* end if (field_get) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::line) */

int subinfo::parse(field *fsp,cchar *kp,int kl) noex {
	cint		klen = (elen - 1) ;
	int		rs = SR_OK ;
	int		c = 0 ;
	char		*kbuf = ebuf ;
	if (kl <= klen) {
	    vs_f	vs = vstrkeycmp ;
	    cint	rsn = SR_NOTFOUND ;
	    cint	vlen = elen ;
	    int		vl ;
	    char	*vbuf = ebuf ;
	    char	*vp = strdcpy1w(kbuf,klen,kp,kl) ;
	    if ((rs = vsp->finder(kbuf,vs,nullptr)) == rsn) {
	        int	fl ;
	 	rs = SR_OK ;
	        *vp++ = '=' ;
	        *vp = '\0' ;
	        vl = intconv(vp - vbuf) ;
	        while ((fl = fsp->sharg(ft,vp,(vlen-vl))) >= 0) {
	            if (fl > 0) {
	                vp += fl ;
	                vl += fl ;
	            }
	            if (fsp->term == '#') break ;
	        } /* end while */
		if (fl != rsn) rs = fl ;
	        *vp = '\0' ;
	        if (rs >= 0) {
	            c += 1 ;
	            rs = vsp->add(vbuf,vl) ;
	        }
	    } /* end if (did not have it already) */
	} /* end if (not-overflow) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::parse) */

static int mkterms() noex {
	return fieldterms(fterms,false,'\t',' ','#','=') ;
}
/* end subroutine (mkterms) */

vars::operator int () noex {
	int		rs ;
	if ((rs = maxline) >= 0) {
	    linebuflen = (rs * LINEBUFMULT) ;
	}
	return rs ;
}
/* end subroutine (vars::operator) */

static int mkinit() noex {
    	int		rs ;
	if ((rs = mkterms()) >= 0) {
	    rs = var ;
	}
	return rs ;
}
/* end subroutine (mkinit) */


