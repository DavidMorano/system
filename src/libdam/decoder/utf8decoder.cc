/* utf8decoder SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UTF-8 decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was made from scratch, although it is (very) similar to
	many others that I have made like it.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	utf8decoder

	Names:
	utf8decoder_start
	utf8decoder_load
	utf8decoder_read
	utf8decoder_finish

	Description:
	This object facilitates the decoding of UTF-8 byte streams
	into UNICODE® characters.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<memory>		/* C++STD |destroy_at(3c++)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<vector>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"utf8decoder.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::vector ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    class widebuf {
	vector<wchar_t>	b ;
	int		oi ;		/* output index */
    public:
	widebuf() noex : oi(0) { } ;
	wchar_t operator [] (int i) const noex {
	    wchar_t	rch = 0 ;
	    csize	bsize = b.size() ;
	    {
	        cint	n = intconv(bsize) ;
	        if ((oi+i) < n) {
		    rch = b[oi+i] ;
	        }
	    }
	    return rch ;
	} ; /* end method (operator) */
	int add(int ch) noex {
	    int		rs ;
	    try {
	        b.push_back(ch) ;
		{
		    csize	bsize = b.size() ;
		    int		sz ;
		    {
		        sz = intconv(bsize) ;
		        rs = (sz - oi) ;
		    }
		}
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	    return rs ;
	} ;
	int add(cchar *sp,int µsl = -1) noex {
	    int		rs ;
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        try {
	            for (int i = 0 ; i < sl ; i += 1) {
		        b.push_back(sp[i]) ;
	            }
		    {
		        csize	bsize = b.size() ;
		        {
			    cint	sz = intconv(bsize) ;
	        	    rs = (sz - oi) ;
		        }
		    }
	        } catch (...) {
		    rs = SR_NOMEM ;
	        }
	    } /* end if (getlenstr) */
	    return rs ;
	} ; /* end method (add) */
	int count() const noex {
	    csize	bsize = b.size() ;
	    int		rs ;
	    {
		cint	sz = intconv(bsize) ;
	    	rs = (sz - oi) ;
	    }
	    return rs ;
	} ;
	int len() const noex {
	    return count() ;
	} ;
	wchar_t at(int i) const noex {
	    csize	bsize = b.size() ;
	    int		rch = 0 ;
	    {
	       cint	n = intconv(bsize) ;
	       if ((oi+i) < n) {
		   rch = b[oi+i] ;
	       }
	    }
	    return rch ;
	} ;
	int adv(int al) noex ;
    } ; /* end structure (widebuf) */
} /* end namespace */

typedef widebuf *	widebufp ;


/* forward references */

template<typename ... Args>
local inline int utf8decoder_magic(utf8decoder *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == UTF8DECODER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (utf8decoder_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int utf8decoder_start(utf8decoder *op) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOMEM ;
	    op->outbuf = nullptr ;
	    op->magval = 0 ;
	    op->code = 0 ;
	    op->rem = 0 ;
	    if (widebuf *wbp = new(nt) widebuf ; wbp) ylikely {
	        op->outbuf = wbp ;
	        op->magval = UTF8DECODER_MAGIC ;
	        rs = SR_OK ;
	    } /* end if (new-widebuf) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (utf8decoder_start) */

int utf8decoder_finish(utf8decoder *op) noex {
	int		rs ;
	if ((rs = utf8decoder_magic(op)) >= 0) ylikely {
	    if (op->outbuf) {
	        widebuf *wbp = widebufp(op->outbuf) ;
		{
	            delete wbp ;
	            op->outbuf = nullptr ;
		}
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (utf8decoder_finish) */

int utf8decoder_load(utf8decoder *op,cchar *sp,int µsl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = utf8decoder_magic(op,sp)) >= 0) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	        if (widebuf *wbp = widebufp(op->outbuf) ; wbp) ylikely {
	            while ((rs >= 0) && (sl-- > 0)) {
		        const wchar_t	uch = mkchar(*sp++) ;
		        if ((uch & 0x80) == 0) {
	                    rs = wbp->add(uch) ;
		            c += 1 ;
		        } else {
		            if ((uch & 0xE0) == 0xC0) {
		                op->rem = 1 ;
		                op->code = ((uch & 0x1F) << 6) ;
		            } else if ((uch & 0xF0) == 0xE0) {
		                op->rem = 2 ;
		                op->code = ((uch & 0x0F) << 12) ;
		            } else if ((uch & 0xF8) == 0xF0) {
		                op->rem = 3 ;
		                op->code = ((uch & 0x07) << 18) ;
		            } else if ((uch & 0xC0) == 0x80) {
		                if (op->rem > 0) {
			            op->rem -= 1 ;
				    cint &rem = op->rem ;
			            op->code |= ((uch & 0x3F) << (rem * 6)) ;
			            if (op->rem == 0) {
	            	                rs = wbp->add(op->code) ;
		    	                c += 1 ;
		                    }
		                } /* end if (process continuation portion) */
		            } /* end if (multi type) */
		        } /* end if (single or multi) */
	           } /* end while */
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (utf8decoder_load) */

int utf8decoder_read(utf8decoder *op,wchar_t *rbuf,int rlen) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = utf8decoder_magic(op,rbuf)) >= 0) ylikely {
	    if (rlen > 0) {
		int	ml ;
		rs = SR_BUGCHECK ;
	        if (widebuf *wbp = widebufp(op->outbuf) ; wbp) ylikely {
	            cint	len = wbp->len() ; /* <- read-coerce */
	            ml = min(len,rlen) ;
	            for (i = 0 ; i < ml ; i += 1) {
		        rbuf[i] = wbp->at(i) ;
	            } /* end for */
	            rs = wbp->adv(i) ;
	        } /* end if (non-null) */
	    } /* end if (positive) */
	    rbuf[i] = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (utf8decoder_read) */


/* private subroutines */

int widebuf::adv(int al) noex {
    	csize		bsize = b.size() ;
	int		rl = 0 ;
	if (al > 0) {
	    cint	sl = intconv(bsize) ;
	    if (sl > (oi + al)) {
		rl = (sl - oi) ;
		oi += rl ;
	    } else {
		rl = (sl - oi) ;
		oi += rl ;
		if (rl == 0) {
		    b.clear() ;
		    oi = 0 ;
		}
	    }
	} else if (al < 0) {
	    cint	sl = intconv(bsize) ;
	    if (sl > oi) {
		rl = (sl - oi) ;
		oi += rl ;
	    } else {
		b.clear() ;
		oi = 0 ;
	    }
	} /* end if */
	return rl ;
} /* end subroutine (widebuf::adv) */


