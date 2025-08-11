/* hdbstr_loadkeys SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load key c-strings from a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hdbstr_loadkeys

	Description:
	This subroutine will read (process) a file and put all of the
	strings found into the string (supplied) list, consisting of
	an HDBSTR object.  All strings on all lines (except that
	comment characters are respected) are loaded as keys (without
	values) into the container object.

	Synopsis:
	int hdbstr_loadkeys(hdbstr *op,cchar *fname) noex

	Arguments:
	op		object pointer
	fname		file-name to extract keys from

	Returns:
	>=0		number of keys loaded
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<ccfile.hh>
#include	<field.h>
#include	<fieldterms.h>
#include	<localmisc.h>

#include	"hdbstr.h"

/* local defines */

#define	NLINES		4		/* multiplier for line-length */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	hdbstr_load(hdbstr *,char *,int,cchar *) noex ;

static int	mkterms() noex ;


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

static bufsizevar	maxlinelen(getbufsize_ml) ;

static char		fterms[termsize] ;


/* exported variables */


/* exported subroutines */

int hdbstr_loadkeys(hdbstr *op,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op && fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		static cint	srs = mkterms() ;
	        if ((rs = srs) >= 0) {
		    if ((rs = maxlinelen) >= 0) {
		        cint	llen = (rs * NLINES) ;
		        char	*lbuf{} ;
		        if ((rs = uc_malloc((llen+1),&lbuf)) >= 0) {
			    {
			        rs = hdbstr_load(op,lbuf,llen,fname) ;
			        c = rs ;
			    }
			    rs1 = uc_free(lbuf) ;
			    if (rs >= 0) rs = rs1 ;
		        } /* end if (m-a-f) */
		    } /* end if (maxlinelen) */
		} /* end if (mkterms) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end suberoutine (hdbstr_loadkeys) */


/* local subroutines */

static int hdbstr_load(hdbstr *op,char *lbuf,int llen,cchar *fn) noex {
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (ccfile kf ; (rs = kf.open(fn)) >= 0) {
	    while ((rs = kf.readln(lbuf,llen)) > 0) {
	        if (field fsb ; (rs = fsb.start(lbuf,rs)) >= 0) {
		    cchar	*fp{} ;
	            for (int fl ; (fl = fsb.get(fterms,&fp)) >= 0 ; ) {
	                if (fl > 0) {
	                    if ((rs = hdbstr_add(op,fp,fl,np,0)) >= 0) {
			        c += 1 ;
			    }
	                } /* end if (got one) */
	                if (fsb.term == '#') break ;
		        if (rs < 0) break ;
	            } /* end for */
	            rs1 = fsb.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (field) */
	    } /* end while (reading lines) */
	    rs1 = kf.close ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ccfile) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hdbstr_load) */

static int mkterms() noex {
	return fieldterms(fterms,false,',','#') ;
}
/* end subroutine (mkterms) */


