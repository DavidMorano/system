/* mkartfile SUPPORT */
/* lang=C++20 */

/* make an article file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkartfile

	Description:
	Make an article file.

	Synopsis:
	int mkartfile(char *rbuf,mode_t om,cc *dname,cc *prefix,int serial) noex

	Arguments:
	rbuf		result buffer (must be MAXPATHLEN+1 in size)
	on		open mode
	dname		directory
	prefix		prefix to created file
	serial		a serial number

	Returns:
	>=0	success and length of created file name
	<0	failure w/ error number

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* for |UCHAR_MAX| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<base64.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<strwcpy.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */

typedef unsigned int	ui ;


/* external subroutines */


/* external variables */


/* forward reference */

static uint64_t mkbits(uint,int,int) noex ;
static int	mktry(char *,int,cchar *,uint,int,int,mode_t) noex ;
static int	mkoutname(char *,cchar *,uint,int,int) noex ;


/* local variables */


/* exported subroutines */

int mkartfile(char *rbuf,cc *dname,cc *prefix,int serial,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (rbuf && dname && prefix) {
	    rs = SR_INVALID ;
	    if (dname[0] && prefix[0] && (serial >= 0)) {
	        const time_t	dt = time(nullptr) ;
	        const mode_t	fm = (om & S_IAMB) ;
	        if ((rs = mkpath1(rbuf,dname)) >= 0) {
	            cint	rlen = rs ;
	            for (uint ts = uint(dt) ; rs >= 0 ; ts += 1) {
			cint	nss = UCHAR_MAX ;
	                for (int ss = serial ; ss < nss ; ss += 1) {
		            for (int es = 0 ; es < 4 ; es += 1) {
			        rs = mktry(rbuf,rlen,prefix,ts,ss,es,fm) ;
		                if (rs > 0) break ;
			        if (rs == SR_EXISTS) rs = SR_OK ;
				if (rs < 0) break ;
		            } /* end for (end-stamp) */
		            if (rs != 0) break ;
	                } /* end for (serial-stamp) */
		        serial = 0 ;
		        if (rs != 0) break ;
	            } /* end for (time-stamp) */
	        } /* end if (mkpath) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkartfile) */


/* local subroutines */

static int mktry(char *rp,int rl,cc *pre,ui ts,int ss,int es,mode_t om) noex {
	cint		of = (O_CREAT|O_EXCL|O_CLOEXEC) ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*tfname{} ;
	if ((rs = malloc_mp(&tfname)) >= 0) {
	    if ((rs = mkoutname(tfname,pre,ts,ss,es)) >= 0) {
	        if ((rs = pathadd(rp,rl,tfname)) >= 0) {
		    len = rs ;
	            if ((rs = u_open(rp,of,om)) >= 0) {
		        u_close(rs) ;
		    }
	        } /* end if (path-add) */
	    } /* end if (made art-filename) */
	    rs1 = uc_free(tfname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mktry) */

static int mkoutname(char *rfname,cchar *pre,uint ts,int ss,int es) noex {
	uint64_t	bits = mkbits(ts,ss,es) ;
	char		*bp = rfname ;
/* load up to 7 prefix characters into output buffer */
	bp = strwcpy(bp,pre,7) ;
/* encode the 'bits' above into the output buffer using BASE-64 encoding */
	{
	    cint	n = 7 ; /* number of chars (7x6=42 bits) */
	    int		i ;
	    int		ch ;
	    for (i = (n-1) ; i >= 0 ; i -= 1) {
	        int	bi = int(bits & 63) ;
	        ch = base64_et[bi] ;
	        if (ch == '+') {
		    ch = '�' ;
		} else if (ch == '/') {
		    ch = '�' ;
		}
	        bp[i] = ch ;
	        bits >>= 6 ;
	    } /* end for */
	    bp += n ;
	}
/* done */
	*bp = '\0' ;
	return (bp - rfname) ;
}
/* end subroutine (mkoutname) */

/* load the 42 bits into the 'bits' variable */
static uint64_t mkbits(uint ts,int ss,int es) noex {
	uint64_t	bits = 0 ;
	    {
		bits |= (ts & UINT_MAX) ;
	    }
	    {
	        bits <<= 8 ;
	        bits |= (ss & 0xff) ;
	    }
	    {
	        bits <<= 2 ;
	        bits |= (es & 3) ;
	    }
	return bits ;
} 
/* end subroutine (mkbits) */


