/* tse SUPPORT (Time-Stamp Entry) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* time-stamp entry marshalling */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-06-25, David A­D­ Morano
	This is being writen to support shared (and permanent)
	timestamps for key-names.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	tse

	Description:
	These little module implements the messages (reads and
	writes) to the TS (Time-Stamp) database file.  These
	subroutines actually do the argument marshalling for the
	file reads and writes.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<serialbuf.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<intceil.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"tse.hh"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int tse::wr(cchar *abuf,int alen) noex {
    	cbool		frd = false ;
	char		*wbuf = cast_const<charp>(abuf) ;
    	return all(frd,wbuf,alen) ;
} /* end method (tse::wr) */

int tse::rd(char *abuf,int alen) noex {
    	cbool		frd = true ;
    	return all(frd,abuf,alen) ;
} /* end method */

int tse::wru(cchar *abuf,int alen) noex {
    	cbool		frd = false ;
    	char		*wbuf = cast_const<charp>(abuf) ;
    	return update(frd,wbuf,alen) ;
} /* end method */

int tse::rdu(char *abuf,int alen) noex {
    	cbool		frd = true ;
    	return update(frd,abuf,alen) ;
} /* end method */

int tse::loadkey(cchar *sp,int sl) noex {
	return sncpyw(keynam,keylen,sp,sl) ;
} /* end method */


/* local subroutines */

int tse::istart() noex {
    	int		rs = SR_OK ;
	int		esz = 0 ;
	esz += szof(count) ;
	esz += szof(utime) ;
	esz += szof(ctime) ;
	esz += szof(hash) ;
	esz += szof(keynam) ;
	entsz = iceil(esz,szof(int)) ;
    	return (rs >= 0) ? entsz : rs ;
} /* end method */

int tse::ifinish() noex {
    	return SR_OK ;
} /* end method */

int tse::all(bool frd,char *abuf,int alen) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (alen < 0) alen = entsz ;
	if (abuf) ylikely {
	    rs = SR_INVALID ;
	    if (alen > 0) {
	        if (serialbuf sb ; (rs = sb.start(abuf,alen)) >= 0) ylikely {
		    if (frd) {
	                sb << count ;
	                sb << utime ;
	                sb << ctime ;
	                sb << hash ;
	                sb.wstrn(keynam,TSE_LKEYNAME) ;
		    } else {
	                sb >> count ;
	                sb >> utime ;
	                sb >> ctime ;
	                sb >> hash ;
	                sb.rstrn(keynam,TSE_LKEYNAME) ;
	            } /* end if */
	            rs1 = sb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (serialbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tse::all) */

int tse::update(bool frd,char *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		usz = 0 ;
	if (alen < 0) alen = entsz ;
	if (abuf) ylikely {
	    rs = SR_INVALID ;
	    if (alen > 0) {
		usz += szof(count) ;
		usz += szof(utime) ;
	        if (serialbuf sb ; (rs = sb.start(abuf,alen)) >= 0) ylikely {
	            if (frd) {
	                sb << count ;
	                sb << utime ;
	            } else {
	                sb >> count ;
	                sb >> utime ;
	            } /* end if */
	            rs1 = sb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (serialbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? usz : rs ;
} /* end subroutine (tse::update) */

void tse::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("tse",rs,"fini-finish") ;
	}
} /* end emthod */

tse::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (entsz > 0) {
	    rs = entsz ;
	}
	return rs ;
} /* end emthod */

tse_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case tsemem_start:
	        rs = op->istart() ;
	        break ;
	    case tsemem_finish:
	        rs = op->finish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (tse_co::operator) */


