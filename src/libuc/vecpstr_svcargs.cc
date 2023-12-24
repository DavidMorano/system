/* vecpstr_svcargs */
/* lang=C20 */

/* load service arguments */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine loads service arguments (arguments which
	accompany a service string) into a VECSTR object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<field.h>
#include	<vecpstr.h>
#include	<strn.h>
#include	<strwcmp.h>
#include	<mkchar.h>
#include	<toxc.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* externals variables */


/* forward references */

static int	hasLong(cchar *,int) noex ;


/* local structures */


/* exported subroutines */

int vecpstr_svcargs(vecpstr *op,int *fp,cchar *abuf) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (abuf && (abuf[0] != '\0')) {
	    FIELD	fsb ;
	    const int	alen = strlen(abuf) ;
	    int		f = FALSE ;
	    char	terms[32] ;
	    fieldterms(terms,0," \t") ;
	    if ((rs = field_start(&fsb,abuf,alen)) >= 0) {
	        cint	flen = alen ;
	        char	*fbuf ;
		if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	            int		fl ;
	            while ((fl = field_sharg(&fsb,terms,fbuf,flen)) >= 0) {
			if (c == 0) {
			     cchar	*tp ;
			     if ((tp = strnchr(fbuf,fl,'/')) != NULL) {
				fl = (tp-fbuf) ;
				if (((fbuf+fl)-tp) >= 2) {
				    const int	sch = MKCHAR(tp[1]) ;
				    f = (tolc(sch) == 'w') ;
				}
			     }
	                     rs = vecpstr_add(op,fbuf,fl) ;
			     c += 1 ;
			} else {
			    if ((fbuf[0] == '/') && hasLong(fbuf,fl)) {
			        f = TRUE ;
			    } else {
	                        rs = vecpstr_add(op,fbuf,fl) ;
			        c += 1 ;
			    }
			}
	                if (rs < 0) break ;
	            } /* end while */
		    uc_free(fbuf) ;
		} /* end if (m-a) */
		if (fp != NULL) *fp = f ;
	        field_finish(&fsb) ;
	    } /* end if (field) */
	} /* end if (non-null non-zero) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_svcargs) */


/* local subroutines */

static int hasLong(cchar *sp,int sl) noex {
	int		f = FALSE ;
	if (sp[0] == '/') {
	    if (sl < 0) sl = strlen(sp) ;
	    if (sl >= 2) {
		const int	sch = MKCHAR(sp[1]) ;
		f = (tolc(sch) == 'w') ;
	    }
	}
	return f ;
}
/* end subroutine (hasLong) */


