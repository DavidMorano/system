/* progencode SUPPORT (base64) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* encode a file (encoded in BASE64) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:

	Description:
	This subroutine reads the given input file and encodes the
	data there in BASE64 and then outputs it to the output file.
	Optional text mode processing is available.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<ucmem.h>
#include	<bfile.h>
#include	<base64.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG */

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	PI
#define	PI	proginfo
#endif

#define	BASE64LINELEN	76
#define	BASE64BUFLEN	((BASE64LINELEN / 4) * 3)

#undef	BUFLEN
#define	BUFLEN		(100 * BASE64BUFLEN)


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* external variables */


/* local structures */

struct outbuf {
	char		*buf ;
	int		i ;
} ; /* end struct (outbuf) */


/* forward references */

local int	outbase64(PI *,bfile *,cchar *,int) noex ;
local int	putout(PI *,bfile *,outbuf *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progencode(PI *pip,bfile *ofp,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		olen = 0 ; /* return-value */
	if (pip && ofp && name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        cint	sz = (BUFLEN + 4) ;
		if (char *buf ; (rs = mem.vall(sz,&buf)) >= 0) {
	            if (pip->fl.text) {
	                outbuf      ob(buf,0) ;
	                bfile       infile, *ifp = &infile ;
	                if (name[0] == '-') name = BFILE_STDIN ;
	                if ((rs = bopen(ifp,name,"r",0666)) >= 0) {
	                    for (int ch ; (ch = bgetc(ifp)) >= 0 ; ) {
	                        if (ch == '\n') {
	                            if (rs >= 0) {
	                                rs = putout(pip,ofp,&ob,'\r') ;
	                                olen += rs ;
	                            }
	                            if (rs >= 0) {
	                                rs = putout(pip,ofp,&ob,'\n') ;
	                                olen += rs ;
	                            }
	                        } else {
	                            rs = putout(pip,ofp,&ob,ch) ;
	                            olen += rs ;
	                        }
	                        if (rs < 0) break ;
	                    } /* end for */
	                    if ((rs >= 0) && (ob.i > 0)) {
	                        rs = outbase64(pip,ofp,buf,ob.i) ;
	                        olen += rs ;
	                    } /* end if */
	                    rs1 = bclose(ifp) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (opnened input file) */
	            } else {
	                int         ifd ;
	                if (name[0] != '-') {
	                    rs = uc_open(name,O_RDONLY,0666) ;
	                    ifd = rs ;
	                } else {
	                    ifd = FD_STDIN ;
	                }
	                if (rs >= 0) {
	                    while ((rs = u_read(ifd,buf,BUFLEN)) > 0) {
	                        cint len = rs ;
	                        rs = outbase64(pip,ofp,buf,len) ;
	                        olen += rs ;
	                        if (rs < 0) break ;
	                    } /* end while */
	                    rs1 = u_close(ifd) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (ok) */
	            } /* end if (straight or text-mode) */
	            rs1 = mem.free(buf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? olen : rs ;
}
/* end subroutine (progencode) */


/* local subroutines */

local int putout(PI *pip,bfile *ofp,outbuf *bp,int ch) noex {
	int		rs = SR_OK ;
	bp->buf[bp->i++] = ch ;
	if (bp->i == BUFLEN) {
	    rs = outbase64(pip,ofp,bp->buf,BUFLEN) ;
	    bp->i = 0 ;
	}
	return rs ;
}
/* end subroutine (putout) */

/* write out in BASE64! */
local int outbase64(PI *pip,bfile *ofp,char *sbuf,int slen) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	char		linebuf[BASE64LINELEN + 4] ;
	for (int rlen = slen, i = 0 ; (rs >= 0) && (rlen > 0) ; ) {
	    cint	mlen = MIN(BASE64BUFLEN,rlen) ;
	    int		len ;
	    len = base64_e(sbuf + i,mlen,linebuf) ;
	    if (pip->fl.test) {
	        rs = 0 ;
	        if (len > 0) {
	            int	c = 1 ;
	            int	ol = 0 ;
	            for (int m, j = 0 ; (rs >= 0) && (j < len) ; ) {
	                m = MIN(c,(len - j)) ;
	                rs = bprintf(ofp,"%r\n",(linebuf + j),m) ;
	                ol += rs ;
	                j += m ;
	                c += 1 ;
	            } /* end for */
	            if (rs >= 0) {
	                rs = ol ;
		    }
	        } /* end if */
	        wlen += rs ;
	    } else {
	        rs = bwrite(ofp,linebuf,len) ;
	        wlen += rs ;
	    }
	    if (rs >= 0) {
	        rs = bputc(ofp,'\n') ;
	        wlen += rs ;
	    }
	    rlen -= mlen ;
	    i += mlen ;
	    if (rs < 0) break ;
	} /* end while */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outbase64) */


