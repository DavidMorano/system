/* mkartfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make an article file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written a long time ago (like
	many of these other subroutines and objects).  This subroutine
	is used in the creation of "article-files" for the BBNEWS
	system.  But this code is now used more widely is helping
	to create files for various other facilities also.  I am
	more formerly documenting this subroutine now.

	= 2025-05-04, David A­D­ Morano
	I changed the code to not (any longer) require any dynamic
	memory allocation.  Rather, the possible choices for a
	working article file file-name are created within the
	caller-supplied result buffer itself (where it would have
	ended up anyway).  Enjoy.

*/

/* Copyright © 1998,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkartfile

	Description:
	Make an "article" file.  That is: I create the file-name
	itself (according to some procedure below) and also the
	associated file in the filesystem.  The resulting file-name
	that was created is returned.

	Synopsis:
	int mkartfile(char *rbuf,cc *dir,cc *pre,int serial,mode_t om) noex

	Arguments:
	rbuf		result buffer (must be MAXPATHLEN+1 in size)
	dir		directory
	pre		prefix to created file
	serial		a serial number
	on		open mode

	Returns:
	>=0	success and length of created file name
	<0	failure w/ error number (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>		/* |malloc_mn(3uc)| */
#include	<base64.h>		/* |base64_enc(3uc)| */
#include	<snx.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<strwcpy.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct trier {
	cchar	*dn ;			/* caller-supplied directory-name */
	cchar	*pn ;			/* caller-supplied prefix-name */
	char	*rbuf ;			/* caller-supplied result buffer */
	char	*nbuf ;			/* internal temporary (path) buffer */
	int	serial ;		/* caller-supplied serial */
	int	nlen ;
	int	rl ;			/* internal variable */
	mode_t	fm ;			/* caller-supplied file-open-mode */
	trier(char *abuf,cc *adn,cc *apn,int as,mode_t om) noex {
	    rbuf = abuf ;
	    dn = adn ;
	    pn = apn ;
	    serial = as ;
	    fm = (om & S_IAMB) ;
	} ;
	operator int () noex ;
	int mktry(uint,int,int) noex ;
    } ; /* end struct (trier) */
} /* end namespace */


/* forward reference */

static uint64_t		mkbits(uint,int,int) noex ;
static int		mkoutname(char *,int,cc *,uint,int,int) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;

cint			outnamelen = 14 ; /* (7 + 7) */


/* exported variables */


/* exported subroutines */

int mkartfile(char *rbuf,cc *dname,cc *prefix,int serial,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (rbuf && dname && prefix) {
	    rs = SR_INVALID ;
	    if (dname[0] && prefix[0] && (serial >= 0)) {
		trier	to(rbuf,dname,prefix,serial,om) ;
		rs = to ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkartfile) */


/* local subroutines */

trier::operator int () noex {
	custime		dt = getustime ;
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = maxpathlen) >= 0) {
	    cint	maxpath = rs ;
	    if ((rs = mkpath(rbuf,dn,"/")) >= 0) {
	        rl = rs ;
		nbuf = (rbuf + rs) ;
		nlen = (maxpath - rs) ;
                for (uint ts = uint(dt) ; rs >= 0 ; ts += 1) {
                    cint    nss = (UCHAR_MAX + 1) ;
                    for (int ss = serial ; ss < nss ; ss += 1) {
                        for (int es = 0 ; es < 4 ; es += 1) {
                            rs = mktry(ts,ss,es) ;
                            len = rs ;
                            if (rs != 0) break ;
                        } /* end for (end-stamp) */
                        if (rs != 0) break ;
                    } /* end for (serial-stamp) */
                    serial = 0 ; /* <- reviewed and acceptable (for now) */
                    if (rs != 0) break ;
                } /* end for (time-stamp) */
	    } /* end if (mkpath) */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? len : rs ;
}
/* end mrthod (trier::operator) */

int trier::mktry(uint ts,int ss,int es) noex {
	cint		of = (O_CREAT|O_EXCL|O_CLOEXEC) ;
	int		rs ;
	int		len = 0 ; /* return-value */
        if ((rs = mkoutname(nbuf,nlen,pn,ts,ss,es)) >= 0) {
            len = (rl + rs) ;
            if ((rs = u_open(rbuf,of,fm)) >= 0) {
                u_close(rs) ;
            } else if (rs == SR_EXISTS) {
                rs = SR_OK ;
                len = 0 ;
            }
        } /* end if (mkoutname) */
	return (rs >= 0) ? len : rs ;
}
/* end method (trier::mktry) */

/* load up to 7 prefix characters into output buffer */
/* encode the 'bits' above into the output buffer using BASE-64 encoding */
static int mkoutname(char *obuf,int olen,cc *pre,uint ts,int ss,int es) noex {
	int		rs = SR_NAMETOOLONG ;
	int		rl = 0 ; /* return-value */
	if (olen >= outnamelen) {
	    uint64_t	bits = mkbits(ts,ss,es) ;
	    char	*bp = obuf ;
	    rs = SR_OK ;
	    {
	        bp = strwcpy(bp,pre,7) ;
	    }
	    {
	        cint	n = 7 ; /* number of chars (7x6=42 bits) */
	        int	ch ;
	        for (int i = (n - 1) ; i >= 0 ; i -= 1) {
	            cint	bi = int(bits & 63) ; /* grab six bits */
	            ch = base64_enc(bi) ;
	            if (ch == '+') {
		        ch = 'Þ' ;
		    } else if (ch == '/') {
		        ch = 'þ' ;
		    }
	            bp[i] = charconv(ch) ;
	            bits >>= 6 ;
	        } /* end for */
	        bp += n ;
	        rl = intconv(bp - obuf) ;
	    } /* end block */
	    *bp = '\0' ;
	} /* end if (valid) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkoutname) */

/* load the 42 bits into the 'bits' variable (of type |uint64_t|) */
static uint64_t mkbits(uint ts,int ss,int es) noex {
	uint64_t	bits = uint64_t(ts) ;	/* load in 32 bits */
	{
	    bits <<= 8 ;
	    bits |= uint64_t(ss & UCHAR_MAX) ;	/* OR in eight (8) bits */
	}
	{
	    bits <<= 2 ;
	    bits |= uint64_t(es & 3) ;		/* OR in two (2) bits */
	}
	return bits ;
} 
/* end subroutine (mkbits) */


