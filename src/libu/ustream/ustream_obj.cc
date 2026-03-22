/* ustream_obj SUPPORT */
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
	ustream

	Description:
	These are the subroutines (methods) to support the OO
	flavored operations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ulogerror.h>
#include	<localmisc.h>

#include	"ustream.hh"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using ustream_ns::ustream_close ;
using ustream_ns::ustream_read ;
using ustream_ns::ustream_readp ;
using ustream_ns::ustream_readln ;
using ustream_ns::ustream_readlns ;
using ustream_ns::ustream_write ;
using ustream_ns::ustream_println ;
using ustream_ns::ustream_vprintf ;
using ustream_ns::ustream_reserve ;
using ustream_ns::ustream_update ;
using ustream_ns::ustream_adv ;
using ustream_ns::ustream_seek ;
using ustream_ns::ustream_tell ;
using ustream_ns::ustream_invalidate ;
using ustream_ns::ustream_flush ;
using ustream_ns::ustream_poll ;
using ustream_ns::ustream_stat ;
using ustream_ns::ustream_lockbegin ;
using ustream_ns::ustream_lockend ;
using ustream_ns::ustream_writeblanks ;
using ustream_ns::ustream_writealign ;
using ustream_ns::ustream_writezero ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ustream::iwrite(cvoid *abuf,int alen) noex {
	return ustream_write(this,abuf,alen) ;
} /* end method (ustream::iwrite) */

int ustream::iclose() noex {
    	int		rs ;
	if ((rs = magic) >= 0) ylikely {
	    rs = ustream_close(this) ;
	} /* end if (ustream_magic) */
    	return rs ;
} /* end method (ustream::iclose) */

int ustream::read(void *rbuf,int rlen,int to) noex {
	int		rs ;
	if ((rs = magic(rbuf)) >= 0) ylikely {
	    rs = ustream_read(this,rbuf,rlen,to) ;
	}
	return rs ;
} /* end method (ustream::read) */

int ustream::readp(void *rbuf,int rlen,off_t off,int to) noex {
    	int		rs ;
	if ((rs = magic(rbuf)) >= 0) ylikely {
   	    rs = ustream_readp(this,rbuf,rlen,off,to) ;
	}
	return rs ;
} /* end method (ustream::readp) */

int ustream::readln(char *rbuf,int rlen,int to) noex {
    	int		rs ;
	if ((rs = magic(rbuf)) >= 0) ylikely {
    	    rs = ustream_readln(this,rbuf,rlen,to) ;
	}
	return rs ;
} /* end method (ustream::readln) */

int ustream::readlns(char *lbuf,int llen,int to,int *lcp) noex {
    	int		rs ;
	if ((rs = magic(lbuf)) >= 0) ylikely {
    	    rs = ustream_readlns(this,lbuf,llen,to,lcp) ;
	}
	return rs ;
} /* end method (ustream::readlns) */

int ustream::write(cvoid *abuf,int alen) noex {
	int		rs ;
	if ((rs = magic(abuf)) >= 0) ylikely {
	    rs = ustream_write(this,abuf,alen) ;
	}
	return rs ;
} /* end method (ustream::write) */

int ustream::println(cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = magic(sp)) >= 0) ylikely {
    	    rs = ustream_println(this,sp,sl) ;
	}
	return rs ;
} /* end method (ustream::prinln) */

int ustream::printf(cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = magic(fmt)) >= 0) ylikely {
	    va_begin(ap,fmt) ;
	    rs = ustream_vprintf(this,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (ustream_magic) */
	return rs ;
} /* end method (ustream::printf) */

int ustream::vprintf(cchar *fmt,va_list ap) noex {
	int		rs ;
	if ((rs = magic(fmt,ap)) >= 0) ylikely {
	    rs = ustream_vprintf(this,fmt,ap) ;
	} /* end if (ustream_magic) */
	return rs ;
} /* end method (ustream::vprintf) */

int ustream::update(off_t roff,cchar *rbuf,int rlen) noex {
    	int		rs ;
	if ((rs = magic(rbuf)) >= 0) ylikely {
    	    rs = ustream_update(this,roff,rbuf,rlen) ;
	}
	return rs ;
} /* end method (ustream::update) */

int ustream::seek(off_t woff,int w) noex {
    	int		rs ;
	if ((rs = magic) >= 0) ylikely {
    	    rs = ustream_seek(this,woff,w) ;
	}
	return rs ;
} /* end method (ustream::seek) */

int ustream::tell(off_t *offp) noex {
    	int		rs ;
	if ((rs = magic) >= 0) ylikely {
    	    rs = ustream_tell(this,offp) ;
	}
	return rs ;
} /* end method (ustream::tell) */

int ustream::stat(ustat *sbp) noex {
    	int		rs ;
	if ((rs = magic(sbp)) >= 0) ylikely {
    	    rs = ustream_stat(this,sbp) ;
	}
	return rs ;
} /* end method (ustream::stat) */

int ustream::lockbegin(int lt,int to) noex {
    	int		rs ;
	if ((rs = magic) >= 0) ylikely {
    	    rs = ustream_lockbegin(this,lt,to) ;
	}
	return rs ;
} /* end method (ustream::lockbrgin) */

void ustream::dtor() noex {
	cint		rs = ustream_close(this) ;
	if (rs < 0) {
	    ulogerror("ustream",rs,"fini-finish") ;
	}
} /* end method (ustream::dtor) */

ustream_ma::operator int () noex {
	return (op->magval == ustream_magicval) ? SR_OK : SR_NOTOPEN ;
} /* end method (ustream_ma::operator) */

int ustream_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magval == ustream_magicval) {
		rs = SR_BUGCHECK ;
	        switch (w) {
	        case ustreammem_reserve:
	            rs = ustream_reserve(op,a) ;
	            break ;
	        case ustreammem_invalidate:
	            rs = ustream_invalidate(op) ;
	            break ;
	        case ustreammem_flush:
	            rs = ustream_flush(op) ;
	            break ;
	        case ustreammem_adv:
	            rs = ustream_adv(op,a) ;
	            break ;
	        case ustreammem_poll:
	            rs = ustream_poll(op,a) ;
	            break ;
	        case ustreammem_lockend:
	            rs = ustream_lockend(op) ;
	            break ;
	        case ustreammem_writeblanks:
	            rs = ustream_writeblanks(op,a) ;
	            break ;
	        case ustreammem_writealign:
	            rs = ustream_writealign(op,a) ;
	            break ;
	        case ustreammem_writezero:
	            rs = ustream_writezero(op,a) ;
	            break ;
	        case ustreammem_close:
	            rs = op->iclose() ;
	            break ;
	        } /* end switch */
	   } /* end if (ustream_magic) */
	} /* end if (non-null) */
	return rs ;
} /* end method (ustream_co::operator) */


