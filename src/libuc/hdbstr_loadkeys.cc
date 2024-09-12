/* hdbstr_loadkeys SUPPORT */
/* lang=C++20 */

/* load key c-strings from a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

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
#include	<unistd.h>
#include	<climits>		/* <- for |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<hdbstr.h>
#include	<bfile.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<localmisc.h>


/* local defines */

#define	NLINES		4


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
		        cint	llen = ((rs+1)*NLINES) ;
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
	bfile		loadfile, *lfp = &loadfile ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = bopen(lfp,fn,"r",0666)) >= 0) {
	    while ((rs = breadln(lfp,lbuf,llen)) > 0) {
	        field	fsb ;
	        int	len = rs ;
	        if (lbuf[len - 1] == '\n') len -= 1 ;
	        if ((rs = field_start(&fsb,lbuf,len)) >= 0) {
		    cchar	*fp{} ;
		    int		fl ;
	            while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {
	                if (fl > 0) {
	                    if ((rs = hdbstr_add(op,fp,fl,np,0)) >= 0) {
			        c += 1 ;
			    }
	                } /* end if (got one) */
	                if (fsb.term == '#') break ;
		        if (rs < 0) break ;
	            } /* end while */
	            rs1 = field_finish(&fsb) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (fields) */
	    } /* end while (reading lines) */
	    rs1 = bclose(lfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hdbstr_load) */

static int mkterms() noex {
	return fieldterms(fterms,false,',','#') ;
}
/* end subroutine (mkterms) */


