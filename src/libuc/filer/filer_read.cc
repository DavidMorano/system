/* filer_read SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support low-overhead file bufferring requirements */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-07-09, David A­D­ Morano
	This code was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	filer

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX |SEEK_{xx}| */
#include	<fcntl.h>		/* POSIX */
#include	<poll.h>		/* POSIX */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD |memcpy(3c)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<funcodes.h>		/* LIBU |FM_{x}(3uc)| */
#include	<intfloor.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<sysval.hh>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<snflags.h>		/* LIBUC */
#include	<fmtstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"filer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memcopy(3u)| */
import filer_mag ;

/* local defines */

#define	MEMCPYLEN	100

#ifndef	CF_DEBUG
#define	CF_DEBUG	1		/* debugging */
#endif


/* imported namespaces */

using std::min ;		/* subroutine-template */
using std::max ;		/* subroutine-template */
using libuc::libmem ;		/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct reader {
	static constexpr int	fmo = FM_TIMED ;
	filer	*op ;
	char	*rbuf ;
	int	rlen ;
	int	to ;
	int	feof ;
	bool	fto ;
	reader(filer *o,void *b,int l,int t) noex : op(o), rlen(l), to(t) { 
	    rbuf = charp(b) ;
	    feof = false ;
	    fto = false ;
	} ; /* end ctor */
	operator int () noex ;
    private:
	int bufcopy(int) noex ;
	int bufread(int) noex ;
    } ; /* end struct (reader) */
} /* end namespace */


/* forward references */

local int	memover(char *dp,cchar *sp,int sl) noex {
    	if (sl < 0) sl = lenstr(sp) ;
    	if (csize msize = size_t(sl) ; msize > 0) {
	    memcpy(dp,sp,msize) ;
	}
	return sl ;
} /* end subroutine (memover) */


/* local variables */

cint			netrc = FILER_RCNET ;
cbool			f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int filer_read(filer *op,void *vbuf,int rlen,int to) noex {
	int		rs ;
	DPRINTF("ent rlen=%d\n",rlen) ;
	if ((rs = filer_magic(op,vbuf)) >= 0) ylikely {
	    if (reader ro(op,vbuf,rlen,to) ; (rs = ro) > 0) {
		op->off += rs ;
	    }
	} /* end if (magic) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (filer_read) */

reader::operator int () noex {
    	int		rs = SR_OK ;
	int		tlen = 0 ; /* return-value */
	while ((rs >= 0) && (! fto) && (! feof) && (tlen < rlen)) {
	    if ((rs = bufcopy(tlen)) >= 0) {
		tlen += rs ;
		rs = bufread(tlen) ;
	    } /* end if (copy-out from buffer) */
	} /* end while */
	return (rs >= 0) ? tlen : rs ;
} /* end method (reader::operator) */

int reader::bufcopy(int tlen) noex {
    	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
    	if (cint nlen = (rlen - tlen) ; nlen > 0) {
	    if (op->len > 0) {
	        cint mlen = min(nlen,op->len) ;
	        if ((rs = memover((rbuf + tlen),op->bp,mlen)) > 0) {
		    op->bp += rs ;
		    op->len -= rs ;
		    len += rs ;
		} /* end if (memcopy) */
	    } /* end if (have some data) */
	} /* end if (need data) */
	return (rs >= 0) ? len : rs ;
} /* end method (reader::bufcopy) */

int reader::bufread(int tlen) noex {
    	cint		nlen = (rlen - tlen) ;
    	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	if ((nlen > 0) && (op->len == 0)) {
	    cint	fd = op->fd ;
	    cint	dlen = op->dlen ;
	    int		rc = (op->fl.net) ? netrc : 1 ;
	    char	*dbuf = op->dbuf ;
	    for ( ; (op->len == 0) && (rc > 0) ; rc -= 1) {
                op->bp = op->dbuf ;
                if (to >= 0) {
                    rs = uc_reade(fd,dbuf,dlen,to,fmo) ;
                    len = rs ;
                } else {
                    rs = uc_read(fd,dbuf,dlen) ;
                    len = rs ;
                }
                if ((rs == SR_TIMEDOUT) && (tlen > 0)) {
                    fto = true ;
                    rs = SR_OK ;
                    break ;
                } /* end if (time-out) */
                if (rs < 0) break ;
                op->len = len ;
	    } /* end for (refill) */
	    if ((rs >= 0) && (len == 0) && (rc == 0)) {
	        feof = true ;
	    } /* end if (EOF calculation) */
	} /* end if (need refilling) */
	return (rs >= 0) ? len : rs ;
} /* end method (reader:bufread) */


