/* varsub_loadfile SUPPORT */
/* lang=C++20 */

/* process a file with variable substitutions */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	varsub_loadfile

	Description:
	This subroutine will read a file of variable substitutions
	and put them into the passed varsub object.

	Synopsis:
	int varsub_loadfile(varsub *op,cchar *fn) noex

	Arguments:
	op		pointer to varsub object to accumulate results
	fn		file to process

	Returns:
	>=0		count of environment variables loaded
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<climits>		/* for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstdlib>
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<stdfnames.h>		/* |STDFNIN| */
#include	<bfile.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<char.h>
#include	<rmx.h>
#include	<localmisc.h>

#include	"varsub.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sub_loadfile {
	varsub	*op ;
	char	*lbuf ;
	char	*abuf ;
	int	llen ;
	int	alen ;
	sub_loadfile(varsub *p,char *lb,int ll,char *ab,int al) noex {
	    op = p ;
	    lbuf = lb ;
	    abuf = ab ;
	    llen = ll ;
	    alen = al ;
	} ;
	int operator () (cchar *) noex ;
    } ; /* end struct (sub_loadfile) */
}


/* forward references */

static int	mkterms() noex ;

static bool	hasexport(cchar *,int) noex ;


/* local variables */

static char		fterms[fieldterms_termsize] ;


/* exported variables */


/* exported subroutines */

int varsub_loadfile(varsub *op,cchar *fn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((fn == nullptr) || (fn[0] == '\0') || (fn[0] == '-')) {
	    fn = STDFNIN ; /* standard-input */
	}
	if (op) {
	     static cint	rst = mkterms() ;
	     if ((rs = rst) >= 0) {
	        if (char *lbuf{} ; (rs = malloc_mp(&lbuf)) >= 0) {
	            cint	llen = rs ;
	            if (char *abuf{} ; (rs = malloc_mp(&abuf)) >= 0) {
		        sub_loadfile	lo(op,lbuf,llen,abuf,rs) ;
		        {
		            rs = lo(fn) ;
		            c = rs ;
		        }
		        rs1 = uc_free(abuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	            rs1 = uc_free(lbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (mkterms) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (varsub_loadfile) */


/* local subroutines */

int sub_loadfile::operator () (cchar *fn) noex {
	bfile		vfile, *vfp = &vfile ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = bopen(vfp,fn,"r",0666)) >= 0) {
	    cint	to = -1 ;
	    int		bl ;
	    while ((rs = breadlns(vfp,lbuf,llen,to,nullptr)) > 0) {
		if (int len ; (len = rmcomment(lbuf,llen)) > 0) {
		    field	fsb ;
	            cchar	*cp = lbuf ;
	            int		cl = len ;
	            if ((rs = fsb.start(cp,cl)) >= 0) {
	    		cchar	*kp{} ;
	                if (int kl ; (kl = fsb.get(fterms,&kp)) > 0) {
		            int		al = alen ;
	                    char	*bp = abuf ;
	                    if ((kl == 6) && hasexport(kp,kl)) {
	                        kl = fsb.get(fterms,&kp) ;
	                    }
	                    while (al > 0) {
			        bl = fsb.sharg(fterms,bp,al) ;
			        if (bl < 0) break ;
	                        if (bl > 0) bp += bl ;
	                        al = (abuf + alen - bp) ;
	                        if (fsb.term == '#') break ;
	                    } /* end while */
	                    *bp = '\0' ;
			    al = (bp - abuf) ;
	                    rs = varsub_add(op,kp,kl,abuf,al) ;
			    if (rs < INT_MAX) c += 1 ;
	                } /* end if (have a variable keyname) */
	                rs1 = fsb.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (field) */
		} /* end if (comment) */
	        if (rs < 0) break ;
	    } /* end while (reading lines) */
	    rs1 = bclose(vfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open-file) */
	return (rs >= 0) ? c : rs ;
}
/* end method (sub_loadfile::operator) */

static int mkterms() noex {
	return fieldterms(fterms,false,' ','#','=') ;
}
/* end subroutine (mkterms) */

static bool hasexport(cchar *sp,int sl) noex {
	return (strncasecmp(sp,"export",sl) == 0) ;
}
/* end subroutine (hasexport) */


