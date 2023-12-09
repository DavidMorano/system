/* mklogid */
/* lang=C20 */

/* make a log ID */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-11-01, David A­D­ Morano
	This was originally written but modeled after a version of
	this sort of subroutine that I previously wrote.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mklogid

	Description:
	Make a log ID (like for use in logging).

	Synopsis:
	int mklogid(char *rbuf,int rlen,cchar *sp,int sl,int v) noex

	Arguments:
	rbuf		buffer to hold result
	rlen		length of supplied result buffer
	sp		string to use for leading part of ID
	sl		length of string to use
	v		number for trailing part of ID
	
	Returns:
	>=0		length of created ID string
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<usystem.h>
#include	<sbuf.h>
#include	<ctdec.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern int	ndigits(int,int) noex ;
extern int	ipow(int,int) noex ;


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

int mklogid(char *rbuf,int rlen,cchar *sp,int sl,int v) noex {
	cint		maxdigs = ndigits(PID_MAX,10) ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		maxstrlen ;

	if ((rbuf == NULL) || (sp == NULL))
	    return SR_FAULT ;

	if (rlen > maxdigs) {
	    maxstrlen = (rlen - maxdigs) ;
	} else if (rlen < 0) {
	    maxstrlen = (LOGIDLEN - maxdigs) ;
	    rlen = INT_MAX ;
	} else
	    rs = SR_OVERFLOW ;

	if (rs >= 0) {
	    const int	dlen = DIGBUFLEN ;
	    int		len ;
	    int		ml ;
	    int		modval ;
	    char	dbuf[DIGBUFLEN + 1] ;

/* get lengths */

	    sl = strnlen(sp,sl) ;

	    modval = (maxdigs < 10) ? ipow(10,maxdigs) : INT_MAX ;
	    v = abs(v) ;
	    v = (v % modval) ; /* limits the decimal part to maxdigs */

/* prepare value string */

	    if ((rs = ctdeci(dbuf,dlen,v)) >= 0) {
	        SBUF		b ;
	        const int	dl = rs ;

	        len = (sl + dl) ;
	        ml = (len > rlen) ? (len - rlen) : 0 ;

/* prepare the receiving buffer */

	        if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {

	            if (ml == 0) {
	                sbuf_strw(&b,sp,sl) ;
	                sbuf_strw(&b,dbuf,dl) ;
	            } else if (ml <= 2) {
	                sbuf_strw(&b,(sp + 2),(sl - 2)) ;
	                sbuf_strw(&b,dbuf,dl) ;
	            } else {
	                ml -= 2 ;
	                sp += 2 ;
	                sl -= 2 ;
	                if (sl <= maxstrlen) {
	                    sbuf_strw(&b,sp,sl) ;
	                    sbuf_strw(&b,dbuf,dl) ;
	                } else {
	                    len = MAX(sl,maxstrlen) + dl ;
	                    ml = (len <= rlen) ? len : (len - rlen) ;
	                    sbuf_strw(&b,sp,MIN(sl,maxstrlen)) ;
	                    sbuf_strw(&b,(dbuf + ml),(dl - ml)) ;
	                } /* end if */
	            } /* end if */

	            rs1 = sbuf_finish(&b) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */

	    } /* end if (cfdec) */

	} /* end if (ok) */

	return rs ;
}
/* end subroutine (mklogid) */


