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
#include	<cinttypes>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"tse.hh"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	tse_update(tse *,int,char *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int tse::wr(cchar *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (alen < 0) alen = INT_MAX ;
	if (abuf) {
	    char	*wbuf = cast_const<charp>(abuf) ;
	    if (serialbuf sb ; (rs = sb.start(wbuf,alen)) >= 0) {
		{
	            sb.rui(&count) ;
	            sb.rui(&utime) ;
	            sb.rui(&ctime) ;
	            sb.rui(&hash) ;
	            sb.rstrn(keyname,TSE_LKEYNAME) ;
	        }
	        rs1 = sb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (serialbuf) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (tse::wr) */

int tse::rd(char *abuf,int alen) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (alen < 0) alen = INT_MAX ;
	if (abuf) {
	    if (serialbuf sb ; (rs = sb.start(abuf,alen)) >= 0) {
		{
	            sb.wui(count) ;
	            sb.wui(utime) ;
	            sb.wui(ctime) ;
	            sb.wui(hash) ;
	            sb.wstrn(keyname,TSE_LKEYNAME) ;
	        }
	        rs1 = sb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (serialbuf) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (tse::rd) */

int tse::wru(cchar *abuf,int alen) noex {
    	cint		frd = false ;
    	char		*wbuf = cast_const<charp>(abuf) ;
    	return tse_update(this,frd,wbuf,alen) ;
}

int tse::rdu(char *abuf,int alen) noex {
    	cint		frd = true ;
    	return tse_update(this,frd,abuf,alen) ;
}


/* local subroutines */

static int tse_update(tse *ep,int f_read,char *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep && abuf) {
	    /* go to where this data is in the message buffer */
	    abuf += TSE_OCOUNT ;
	    alen -= TSE_OCOUNT ;
	    /* proceed as normal (?) :-) */
	    if (serialbuf sb ; (rs = sb.start(abuf,alen)) >= 0) {
	        if (f_read) {
	            sb.wui(ep->count) ;
	            sb.wui(ep->utime) ;
	        } else {
	            sb.rui(&ep->count) ;
	            sb.rui(&ep->utime) ;
	        } /* end if */
	        rs1 = sb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (serialbuf) */
	    if (rs >= 0) {
	        rs += TSE_OCOUNT ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (tse_update) */


