/* vecstrx_svcargs SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* load service arguments */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A�D� Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	vecpstr_svcargs

	Description:
	This subroutine loads service arguments (arguments which
	accompany a service string) into a VECSTR object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* for |CHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<strn.h>
#include	<strwcmp.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"vecstrx.hh"


/* local defines */


/* external subroutines */


/* externals variables */


/* forward references */

static int	vecstrx_arger(vecstrx *,int *,cchar *) noex ;
static int	mkterms() noex ;
static int	hasLong(cchar *,int) noex ;


/* local structures */


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

static char		terms[termsize] ;


/* exported variables */


/* exported subroutines */

int vecstrx::svcargs(int *fp,cchar *abuf) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (fp && abuf) {
            rs = SR_INVALID ;
	    if (abuf[0]) {
	        static cint	rst = mkterms() ;
	        if ((rs = rst) >= 0) {
		    rs = vecstrx_arger(this,fp,abuf) ;
		    c = rs ;
		} /* end if (terms) */
	    } /* end if (valid) */
	} /* end if (non-null non-zero) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_svcargs) */


/* local subroutines */

static int vecstrx_arger(vecstrx *vsp,int *fp,cchar *abuf) noex {
    	cnullptr	np{} ;
	cint		alen = strlen(abuf) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *fbuf{} ; (rs = uc_malloc((alen+1),&fbuf)) >= 0) {
	    cint	flen = alen ;
	    if (field fsb ; (rs = fsb.start(abuf,alen)) >= 0) {
	        int	fl ;
		int	f = false ;
	        while ((fl = fsb.sharg(terms,fbuf,flen)) >= 0) {
		    if (c == 0) {
			if (cc *tp ; (tp = strnchr(fbuf,fl,'/')) != np) {
			     fl = (tp-fbuf) ;
			     if (((fbuf+fl)-tp) >= 2) {
				cint	sch = mkchar(tp[1]) ;
				f = (tolc(sch) == 'w') ;
			    }
			}
	                rs = vsp->add(fbuf,fl) ;
			c += 1 ;
		    } else {
			if ((fbuf[0] == '/') && hasLong(fbuf,fl)) {
			    f = true ;
			} else {
	                    rs = vsp->add(fbuf,fl) ;
			    c += 1 ;
			}
		    }
	            if (rs < 0) break ;
	        } /* end while */
		if (fp) *fp = f ;
	        rs1 = fsb.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	    rs1 = uc_free(fbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_arger) */

static int mkterms() noex {
	return fieldterms(terms,false,'\t',' ') ;
}
/* end subroutine (mkterms) */

static int hasLong(cchar *sp,int sl) noex {
	int		f = false ;
	if (sp[0] == '/') {
	    if (sl < 0) sl = strlen(sp) ;
	    if (sl >= 2) {
		cint	sch = mkchar(sp[1]) ;
		f = (tolc(sch) == 'w') ;
	    }
	}
	return f ;
}
/* end subroutine (hasLong) */


