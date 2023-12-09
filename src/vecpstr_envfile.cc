/* vecpstr_envfile */
/* lang=C++20 */

/* process an environment file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecpstr_envfile

	Description:
	This subroutine will read (process) an environment file and
	put any newly encountered environment variables into the
	string list (supplied).  Variables found that already exist
	are ignored.

	Synopsis:
	int vecpstr_envfile(vecpstr *vlp,cchar *fname) noex

	Arguments:
	vlp		vecpstr-string object to accumulate results
	fname		file to process

	Returns:
	>=0		count of environment variables
	<0		bad

	- Implementation note:
	Although this function is (overall) rather simple, I put
	some slightly cleaver thought into parsing the lines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<utimeout.h>
#include	<vecpstr.h>
#include	<filebuf.h>
#include	<field.h>
#include	<bufsizevar.hh>
#include	<localmisc.h>


/* local defines */

#define	LINEBUFMULT	5		/* line-buffer size multiplier */

#define	WORDEXPORT	"export"	/* optional word to ignore */


/* local namespaces */

using std::nullptr_t ;


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	sfskipwhite(cchar *,int,cchar **) noex ;
    extern int	vstrkeycmp(cchar **,cchar **) noex ;
    typedef int (*vs_f)(cchar **,cchar **) noex ;
}

extern "C" {
    extern char	*strdcpy1w(char *,int,cchar *,int) ;
}


/* external variables */


/* local structures */

namespace {
    struct subinfo {
	vecpstr		*vlp ;
	cchar		*ft ;
	cchar		*a ;		/* allocation */
	char		*lbuf ;
	char		*ebuf ;
	int		llen ;
	int		elen ;
	subinfo(vecpstr *p,cchar *t) noex : vlp(p), ft(t) { } ;
	int start() noex ;
	int finish() noex ;
	int line(cchar *,int) noex ;
	int var(field *,cchar *,int) noex ;
    } ; /* end struct (subinfo) */
}

namespace {
    struct eword {
	cint		l ;
	static cchar	p[] ;
	constexpr eword() noex : l(sizeof(WORDEXPORT)-1) { } ;
    } ;
    constexpr char eword::p[] = WORDEXPORT ;
}


/* forward references */


/* local variables */

static bufsizevar	maxlinelen(getbufsize_ml) ;

static constexpr char	fterms[32] = {
	0x00, 0x02, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported subroutines */

int vecpstr_envfile(vecpstr *vlp,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (vlp && fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        typedef	int (*rls_f)(filebuf *,char *,int,int,int *) ;
	        subinfo		si(vlp,fterms) ;
	        if ((rs = si.start()) >= 0) {
	    	    cint	to_open = utimeout[uto_open] ;
		    if ((rs = uc_opene(fname,O_RDONLY,0666,to_open)) >= 0) {
	                filebuf		dfile, *dfp = &dfile ;
	                cint		fd = rs ;
	                if ((rs = filebuf_start(dfp,fd,0L,0,0)) >= 0) {
		            cint	to = utimeout[uto_read] ;
			    cint	llen = si.llen ;
	                    int		len ;
	                    int		cl ;
		            rls_f	rls = filebuf_readlines ;
			    char	*lbuf = si.lbuf ;
		            nullptr_t	n{} ;
	                    while ((rs = rls(dfp,lbuf,llen,to,n)) > 0) {
	                        cchar	*cp ;
	                        len = rs ;
	                        if (lbuf[len - 1] == '\n') len -= 1 ;
	                        lbuf[len] = '\0' ;
	                        if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                            if (cp[0] != '#') {
	                                rs = si.line(cp,cl) ;
	                                c += rs ;
	                            }
	                        }
	                        if (rs < 0) break ;
	                    } /* end while (reading lines) */
	                    rs1 = filebuf_finish(dfp) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (filebuf) */
	                rs1 = uc_close(fd) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (file) */
	            rs1 = si.finish() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_envfile) */


/* local subroutines */

int subinfo::start() noex {
	int		rs ;
	if ((rs = maxlinelen) >= 0) {
	    int		size = 0 ;
	    char	*bp ;
	    llen = (rs*LINEBUFMULT) ;
	    size += (2*(llen+1)) ;
	    if ((rs = uc_libmalloc(size,&bp)) >= 0) {
	        a = bp ;
	        lbuf = bp ;
	        bp += (llen+1) ;
	        ebuf = bp ;
	        elen = llen ;
	    } /* end if (m-a) */
	} /* end if (maxlinelen) */
	return rs ;
}
/* end subroutine (subinfo::start) */

int subinfo::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (a) {
	    rs1 = uc_libfree(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

int subinfo::line(cchar *lp,int ll) noex {
	static constexpr eword	ew ;
	field		fsb ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = field_start(&fsb,lp,ll)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    if ((fl = field_get(&fsb,ft,&fp)) > 0) {
	        if ((fl == ew.l) && (strncasecmp(ew.p,fp,fl) == 0)) {
	            fl = field_get(&fsb,ft,&fp) ;
	        }
	    } /* end if (variable key-name) */
	    if (fl > 0) {
	        rs = var(&fsb,fp,fl) ;
	        c = rs ;
	    }
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::line) */

int subinfo::var(field *fsp,cchar *kp,int kl) noex {
	cint		klen = (elen -1) ;
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
	    if ((rs = vecpstr_finder(vlp,kbuf,vs,nullptr)) == rsn) {
	        int	fl ;
	 	rs = SR_OK ;
	        *vp++ = '=' ;
	        *vp = '\0' ;
	        vl = (vp-vbuf) ;
	        while ((fl = field_sharg(fsp,ft,vp,(vlen-vl))) >= 0) {
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
	            rs = vecpstr_add(vlp,vbuf,vl) ;
	        }
	    } /* end if (did not have it already) */
	} /* end if (not-overflow) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::var) */


