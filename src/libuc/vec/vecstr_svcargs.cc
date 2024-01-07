/* vecstr_svcargs SUUPORT */
/* lang=C++20 */

/* load service arguments */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A�D� Morano
	This program was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine loads service arguments (arguments which
	accompany a service string) into a VECSTR object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<climits>		/* for |CHAR_MAX| + |CHAR_BIT| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<field.h>
#include	<vecstr.h>
#include	<strn.h>
#include	<strwcmp.h>
#include	<mkchar.h>
#include	<toxc.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* externals variables */


/* forward references */

static int	vecstr_arger(vecstr *,int *,cchar *) noex ;
static int	mkterms() noex ;
static int	hasLong(cchar *,int) noex ;


/* local structures */


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

static char		terms[termsize] ;


/* exported subroutines */

int vecstr_svcargs(vecstr *op,int *fp,cchar *abuf) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && fp && abuf) {
            rs = SR_INVALID ;
	    if (abuf[0]) {
	        static cint	srs = mkterms() ;
	        if ((rs = srs) >= 0) {
		    rs = vecstr_arger(op,fp,abuf) ;
		    c = rs ;
		} /* end if (terms) */
	    } /* end if (valid) */
	} /* end if (non-null non-zero) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_svcargs) */


/* local subroutines */

static int vecstr_arger(vecstr *op,int *fp,cchar *abuf) noex {
	cint		alen = strlen(abuf) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*fbuf{} ;
	if ((rs = uc_malloc((alen+1),&fbuf)) >= 0) {
	    field	fsb ;
	    cint	flen = alen ;
	    if ((rs = field_start(&fsb,abuf,alen)) >= 0) {
	        int	fl ;
		int	f = false ;
	        while ((fl = field_sharg(&fsb,terms,fbuf,flen)) >= 0) {
		    if (c == 0) {
			cchar	*tp ;
			if ((tp = strnchr(fbuf,fl,'/')) != nullptr) {
			     fl = (tp-fbuf) ;
			     if (((fbuf+fl)-tp) >= 2) {
				cint	sch = mkchar(tp[1]) ;
				f = (tolc(sch) == 'w') ;
			    }
			}
	                rs = vecstr_add(op,fbuf,fl) ;
			c += 1 ;
		    } else {
			if ((fbuf[0] == '/') && hasLong(fbuf,fl)) {
			    f = true ;
			} else {
	                    rs = vecstr_add(op,fbuf,fl) ;
			    c += 1 ;
			}
		    }
	            if (rs < 0) break ;
	        } /* end while */
		if (fp) *fp = f ;
	        rs1 = field_finish(&fsb) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	    rs1 = uc_free(fbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_arger) */

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


