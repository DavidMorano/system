/* ureserve9 MODULE (vecstr - implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vecstr - process an environment file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstr::addenvfile

	Description:
	This subroutine will read (process) an environment file and
	put any newly encountered environment variables into the
	string list (supplied).  Variables found that already exist
	are ignored.

	Synopsis:
	int vecstr::addenvfile(cchar *fname) noex

	Arguments:
	fname		file to process

	Returns:
	>=0		count of environment variables
	<0		bad

	- Implementation note:
	Although this function is (overall) rather simple, I put
	some slightly cleaver thought into parsing the lines.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD |strncasecmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<utimeout.h>		/* LIBU */
#include	<usysconf.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU |_SC_LINE_MAX| */
#include	<ccfile.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

module ureserve ;

import libutil ;			/* |lenstr(3u)| */
import usysconf ;			/* |usysconfval(3u)| */

/* local defines */

#define	LINEBUFMULT	5		/* line-buffer size multiplier */
#define	WORDEXPORT	"export"	/* optional word to ignore */


/* imported namespaces */

using libu::strdcpy1w ;			/* subroutine */
using libu::vstrkeycmp ;		/* subroutine */
using libu::umem ;			/* variable */


/* local typedefs */

extern "C" {
    typedef int (*vs_f)(cchar **,cchar **) noex ;
} /* end extern */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct subinfo {
	vecstr		*vsp ;
	cchar		*ft ;		/* field-terms */
	char		*a ;		/* allocation */
	char		*lbuf ;
	char		*ebuf ;
	int		llen ;
	int		elen ;
	subinfo(vecstr *p,cchar *t) noex : vsp(p), ft(t) { } ;
	int start() noex ;
	int finish() noex ;
	int line(cchar *,int) noex ;
	int parse(field *,cchar *,int) noex ;
    } ; /* end struct (subinfo) */
} /* end namespace */

namespace {
    struct eword {
	int		l ;
	static cchar	p[] ;
	constexpr eword() noex {
	    l = clenstr(WORDEXPORT) ;
	} ; /* end ctor */
    } ; /* end struct (eword) */
    constexpr char eword::p[] = WORDEXPORT ;
    struct vars {
	int		linebuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

local int	vecstr_envfiler(vecstr *,cchar *) noex ;
local int	mkinit() noex ;
local int	mkterms() noex ;


/* local variables */

static vars	var ;
static char	fterms[fieldterms_termsize] ;


/* exported variables */


/* exported subroutines */

int vecstr::addenvfile(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	DPRINTF("ent fn=%s\n",fname) ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		if (static cint rsi = mkinit() ; (rs = rsi) >= 0) ylikely {
		    rs = vecstr_envfiler(this,fname) ;
		    c = rs ;
		} /* end if (mkinit) */
		DPRINTF("mkinit() _out rs=%d\n",rs) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (vecstr::envfile) */


/* private subroutines */

int vecstr_envfiler(vecstr *op,cchar *fname) noex {
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	DPRINTF("ent fn=%s\n",fname) ;
	if (subinfo si(op,fterms) ; (rs = si.start()) >= 0) ylikely {
            cmode   om = 0666 ;
            if (ccfile cf ; (rs = cf.open(fname,np,om)) >= 0) ylikely {
                    cint    to = utimeout[uto_read] ;
                    cint    llen = si.llen ;
                    char    *lbuf = si.lbuf ;
	            DPRINTF("filer_start() rs=%d\n",rs) ;
                    while ((rs = cf.readlns(lbuf,llen,to,np)) > 0) {
			cchar	*cp{} ;
			if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
                            rs = si.line(cp,cl) ;
                            c += rs ;
                        }
                        if (rs < 0) break ;
                    } /* end while (reading lines) */
	            DPRINTF("while-out rs=%d\n",rs) ;
                rs1 = cf.close ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (ccfile) */
	   DPRINTF("u_open()-out rs=%d\n",rs) ;
            rs1 = si.finish() ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (subinfo) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstr_envfiler) */

int subinfo::start() noex {
	int		rs ;
	int		sz = 0 ;
	int		ai = 2 ;
	llen = var.linebuflen ;
	sz += (ai * (llen + 1)) ;
	if (char *bp ; (rs = umem.mall(sz,&bp)) >= 0) {
	    a = bp ;
	    lbuf = (a + (--ai * (llen + 1))) ;
	    ebuf = (a + (--ai * (llen + 1))) ;
	    elen = llen ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (subinfo::start) */

int subinfo::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (a) ylikely {
	    rs1 = umem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (subinfo_finish) */

int subinfo::line(cchar *lp,int ll) noex {
	static constexpr eword	ew ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) ylikely {
	    cchar	*fp{} ;
	    if (int fl ; (fl = fsb.get(ft,&fp)) > 0) {
	        if ((fl == ew.l) && (strncasecmp(ew.p,fp,fl) == 0)) {
	            fl = fsb.get(ft,&fp) ;
	        } /* end if ("export" word) */
	        if (fl > 0) {
	            rs = parse(&fsb,fp,fl) ;
	            c = rs ;
	        } /* end if (non-zero positive) */
	    } /* end if (field_get) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo::line) */

/****
  ebuf -- expansion result buffer pointer
  elen -- expansion result buffer length
****/
int subinfo::parse(field *fsp,cchar *kp,int kl) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	if (kl <= (elen - 1)) ylikely {
	    int		el = 0 ;
	    if ((rs = vsp->findkey(kp,kl,np)) == rsn) {
	        char	*ep = strdcpy1w(ebuf,elen,kp,kl) ;
	        int	fl ; /* used-afterwards */
	 	rs = SR_OK ;
	        *ep++ = '=' ;
	        *ep = '\0' ;
	        el = intconv(ep - ebuf) ;
	        while ((fl = fsp->sharg(ft,ep,(elen - el))) >= 0) {
	            if (fl > 0) {
	                ep += fl ;
	                el += fl ;
	            }
	            if (fsp->term == '#') break ;
	        } /* end while */
		if (fl != rsn) rs = fl ;
	        *ep = '\0' ;
	        if (rs >= 0) {
	            c += 1 ;
	            rs = vsp->add(ebuf,el) ;
	        } /* end if (ok) */
	    } /* end if (did not have it already) */
	} /* end if (not-overflow) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo::parse) */

local int mkterms() noex {
	return fieldterms(fterms,false,'\t',' ','#','=') ;
} /* end subroutine (mkterms) */

vars::operator int () noex {
    	cint		cmd = _SC_LINE_MAX ;
	int		rs ;
	if ((rs = usysconfval(cmd)) >= 0) ylikely {
	    linebuflen = (rs * LINEBUFMULT) ;
	}
	return rs ;
} /* end subroutine (vars::operator) */

local int mkinit() noex {
    	int		rs ;
	DPRINTF("ent\n") ;
	if ((rs = mkterms()) >= 0) ylikely {
	    rs = var ;
	}
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (mkinit) */


