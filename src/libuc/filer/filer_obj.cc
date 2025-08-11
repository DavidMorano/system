/* filer_obj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extra methods for the FILER object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	filer

	Description:
	These are the subroutines (methods) to support the OO
	flavored operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"filer.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int filer::start(int afd,off_t soff,int bsz,int to) noex {
	return filer_start(this,afd,soff,bsz,to) ;
}

int filer::read(void *rbuf,int rlen,int to) noex {
	return filer_read(this,rbuf,rlen,to) ;
}

int filer::readp(void *rbuf,int rlen,off_t poff,int to) noex {
	return filer_readp(this,rbuf,rlen,poff,to) ;
}

int filer::readln(char *rbuf,int rlen,int to) noex {
	return filer_readln(this,rbuf,rlen,to) ;
}

int filer::readlns(char *rbuf,int rlen,int to,int *nlinep) noex {
	return filer_readlns(this,rbuf,rlen,to,nlinep) ;
}

int filer::write(cvoid *rbuf,int rlen) noex {
	return filer_write(this,rbuf,rlen) ;
}

int filer::writeto(cvoid *rbuf,int rlen,int to) noex {
	return filer_writeto(this,rbuf,rlen,to) ;
}

int filer::println(cchar *rbuf,int rlen) noex {
	return filer_println(this,rbuf,rlen) ;
}

int filer::printf(cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (fmt) {
	    va_begin(ap,fmt) ;
	    rs = filer_vprintf(this,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
} /* end method (filer::printf) */

int filer::vprintf(cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	if (fmt && ap) {
	    rs = filer_vprintf(this,fmt,ap) ;
	}
	return rs ;
}

int filer::seek(off_t soff,int w) noex {
	return filer_seek(this,soff,w) ;
}

int filer::writefill(cchar *wbuf,int wlen) noex {
	return filer_writefill(this,wbuf,wlen) ;
}

int filer::writefd(char *bufp,int bufl,int mfd,int alen) noex {
	return filer_writefd(this,bufp,bufl,mfd,alen) ;
}

int filer::stat(ustat *sbp) noex {
	return filer_stat(this,sbp) ;
}

void filer::dtor() noex {
	cint		rs = filer_finish(this) ;
	if (rs < 0) {
	    ulogerror("filer",rs,"fini-finish") ;
	}
} /* end method (filer::dtor) */

int filer_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case filermem_reserve:
	        rs = filer_reserve(op,a) ;
	        break ;
	    case filermem_invalidate:
	        rs = filer_invalidate(op) ;
	        break ;
	    case filermem_flush:
	        rs = filer_flush(op) ;
	        break ;
	    case filermem_adv:
	        rs = filer_adv(op,a) ;
	        break ;
	    case filermem_poll:
	        rs = filer_poll(op,a) ;
	        break ;
	    case filermem_lockend:
	        rs = filer_lockend(op) ;
	        break ;
	    case filermem_writeblanks:
	        rs = filer_writeblanks(op,a) ;
	        break ;
	    case filermem_writealign:
	        rs = filer_writealign(op,a) ;
	        break ;
	    case filermem_writezero:
	        rs = filer_writezero(op,a) ;
	        break ;
	    case filermem_finish:
	        rs = filer_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (filer_co::operator) */


