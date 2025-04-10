/* tse SUPPORT (Time-Stamp Entry) */
/* encoding=ISO8859-1 */
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
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<serialbuf.h>
#include	<intceil.h>
#include	<localmisc.h>

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
}
/* end subroutine (tse::wr) */

int tse::rd(char *abuf,int alen) noex {
    	cbool		frd = true ;
    	return all(frd,abuf,alen) ;
}

int tse::wru(cchar *abuf,int alen) noex {
    	cbool		frd = false ;
    	char		*wbuf = cast_const<charp>(abuf) ;
    	return update(frd,wbuf,alen) ;
}

int tse::rdu(char *abuf,int alen) noex {
    	cbool		frd = true ;
    	return update(frd,abuf,alen) ;
}


/* local subroutines */

int tse::istart() noex {
    	int		rs = SR_OK ;
	int		esz = 0 ;
	esz += szof(count) ;
	esz += szof(utime) ;
	esz += szof(ctime) ;
	esz += szof(hash) ;
	esz += szof(keyname) ;
	entsz = iceil(esz,szof(int)) ;
    	return (rs >= 0) ? entsz : rs ;
}

int tse::ifinish() noex {
    	return SR_OK ;
}

int tse::all(bool frd,char *abuf,int alen) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (alen < 0) alen = INT_MAX ;
	if (abuf) {
	    rs = SR_INVALID ;
	    if (alen > 0) {
	        if (serialbuf sb ; (rs = sb.start(abuf,alen)) >= 0) {
		    if (frd) {
	                sb << count ;
	                sb << utime ;
	                sb << ctime ;
	                sb << hash ;
	                sb.wstrn(keyname,TSE_LKEYNAME) ;
		    } else {
	                sb >> count ;
	                sb >> utime ;
	                sb >> ctime ;
	                sb >> hash ;
	                sb.rstrn(keyname,TSE_LKEYNAME) ;
	            }
	            rs1 = sb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (serialbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tse::all) */

int tse::update(bool frd,char *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (abuf) {
	    rs = SR_INVALID ;
	    if (alen > 0) {
	        /* go to where this data is in the message buffer */
	        abuf += TSE_OCOUNT ;
	        alen -= TSE_OCOUNT ;
	        /* proceed as normal (?) :-) */
	        if (serialbuf sb ; (rs = sb.start(abuf,alen)) >= 0) {
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
	        if (rs >= 0) {
	            rs += TSE_OCOUNT ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tse::update) */

void tse::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("tse",rs,"fini-finish") ;
	}
}

tse::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (entsz > 0) {
	    rs = entsz ;
	}
	return rs ;
}

tse_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
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
}
/* end method (tse_co::operator) */


