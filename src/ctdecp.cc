/* ctdecp */
/* lang=C++20 */

/* subroutines to convert an integer to a decimal string /w precision */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	ctdecpi
	ctdecpl
	ctdecpll
	ctdecpui
	ctdecpul
	ctdecpull

	Description:
	These subroutines perform a conversion of an integer to its
	corresponding decimal c-string representation.

	Synopsis:
	int ctdecpX(char *rbuf,int rlen,int prec,X v) noex

	Arguments:
	X		'i' or 'ui'
	rbuf		result buffer
	rlen		length of result buffer
	prec		precision
	v		value to convert

	Returns:
	>=0		length of result string
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<stdintx.h>
#include	<ctdec.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	zerofill(char *,int,int,int) noex ;


/* local variables */


/* local subroutine-templates */

template<typename UT>
int ctdecpux(int (*ctx)(char *,int,UT),char *dp,int dl,int p,UT uv) noex {
	int		rs ;
	if (((rs = ctx(dp,dl,uv)) >= 0) && (rs < p)) {
	    rs = zerofill(dp,dl,p,rs) ;
	}
	return rs ;
}
/* end subroutine (ctdecpux) */

template<typename UT,typename T>
int ctdecpx(int (*ctx)(char *,int,int,UT),char *dp,int dl,int p,T v) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (dp) {
	    UT		uv = UT(v) ;
	    int		rl = dl ;
	    char	*rp = dp ;
	    if (v < 0) {
	        uv = -uv ;
	        rp += 1 ;
	        rl -= 1 ;
	    }
	    if ((rs = ctx(rp,rl,p,uv)) >= 0) {
	        n = rs ;
	        if (v < 0) {
	            dp[0] = '-' ;
	            n += 1 ;
	        }
	    } /* end if (ctx) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (ctdecpx) */


/* exported subroutines */

int ctdecpui(char *dp,int dl,int p,uint uv) noex {
	return ctdecpux(ctdecui,dp,dl,p,uv) ;
}

int ctdecpul(char *dp,int dl,int p,ulong uv) noex {
	return ctdecpux(ctdecul,dp,dl,p,uv) ;
}

int ctdecpull(char *dp,int dl,int p,ulonglong uv) noex {
	return ctdecpux(ctdecull,dp,dl,p,uv) ;
}

int ctdecpi(char *rbuf,int rlen,int prec,int v) noex {
	return ctdecpx(ctdecpui,rbuf,rlen,prec,v) ;
}

int ctdecpl(char *rbuf,int rlen,int prec,long v) noex {
	return ctdecpx(ctdecpul,rbuf,rlen,prec,v) ;
}

int ctdecpll(char *rbuf,int rlen,int prec,longlong v) noex {
	return ctdecpx(ctdecpull,rbuf,rlen,prec,v) ;
}


/* local subroutines */

static int zerofill(char *rbuf,int rlen,int prec,int n) noex {
	int		rs = SR_OVERFLOW ;
	cint		bi = (prec - n) ;
	if (prec <= rlen) {
	    int		i ;
	    rs = SR_OK ;
	    for (i = (n-1) ; i >= 0 ; i -= 1) {
	       rbuf[i+bi] = rbuf[i] ;
	    }
	    for (i = 0 ; i < bi ; i += 1) {
		rbuf[i] = '0' ;
	    }
	    rs = prec ;
	} /* end if (not overflow) */
	return rs ;
}
/* end subroutine (zerofill) */


