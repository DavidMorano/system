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
	int varsub_loadfile(varsub *vsp,cchar *fname) noex

	Arguments:
	vsp		pointer to varsub object to accumulate results
	fname		file to process

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
#include	<stdfnames.h>
#include	<bfile.h>
#include	<field.h>
#include	<char.h>
#include	<rmx.h>
#include	<localmisc.h>

#include	"varsub.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */

static int	mkterms() noex ;

static bool	hasexport(cchar *,int) noex ;


/* local structures */


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

static char		fterms[termsize] ;


/* exported subroutines */

int varsub_loadfile(varsub *vsp,cchar *fname) noex {
	static cint	srs = mkterms() ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = srs) >= 0) {
	char		*lbuf ;

	if (vsp == nullptr) return SR_FAULT ;
	if (fname == nullptr) return SR_FAULT ;

	if ((fname == nullptr) || (fname[0] == '\0') || (fname[0] == '-'))
	    fname = STDFNIN ;

	if ((rs = malloc_mp(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    char	*abuf ;
	    if ((rs = malloc_mp(&abuf)) >= 0) {
		bfile		vfile, *vfp = &vfile ;
		cint	alen = rs ;
	if ((rs = bopen(vfp,fname,"r",0666)) >= 0) {
	    cint	to = -1 ;
	    int		bl ;

	    while ((rs = breadlns(vfp,lbuf,llen,to,nullptr)) > 0) {
		int		len ;
		if ((len = rmcomment(lbuf,llen)) > 0) {
		    FIELD	fsb ;
	            cchar	*cp = lbuf ;
	            int		cl = len ;
	            if ((rs = field_start(&fsb,cp,cl)) >= 0) {
	    		int	kl ;
	    		cchar	*kp ;
	                if ((kl = field_get(&fsb,fterms,&kp)) > 0) {
			    auto	fa = field_sharg ;
		            int		al = alen ;
	                    char	*bp = abuf ;
	                    if ((kl == 6) && hasexport(kp,kl)) {
	                        kl = field_get(&fsb,fterms,&kp) ;
	                    }
	                    while (al > 0) {
			        bl = fa(&fsb,fterms,bp,al) ;
			        if (bl < 0) break ;
	                        if (bl > 0) bp += bl ;
	                        al = (abuf + alen - bp) ;
	                        if (fsb.term == '#') break ;
	                    } /* end while */
	                    *bp = '\0' ;
	                    rs = varsub_add(vsp,kp,kl,abuf,(bp - abuf)) ;
			    if (rs < INT_MAX) c += 1 ;
	                } /* end if (have a variable keyname) */
	                rs1 = field_finish(&fsb) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (field) */
		} /* end if (comment) */

	        if (rs < 0) break ;
	    } /* end while (reading lines) */

	    rs1 = bclose(vfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open-file) */
		rs1 = uc_free(abuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */

	} /* end if (mkterms) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (varsub_loadfile) */


/* local subroutines */

static int mkterms() noex {
	return fieldterms(fterms,false,' ','#','=') ;
}
/* end subroutine (mkterms) */

static bool hasexport(cchar *sp,int sl) noex {
	return (strncasecmp(sp,"export",sl) == 0) ;
}
/* end subroutine (hasexport) */


