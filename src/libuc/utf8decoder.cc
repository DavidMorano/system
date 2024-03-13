/* utf8decoder SUPPORT */
/* lang=C++98 */

/* UTF-8 decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was really made from scratch.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        We facilitate the decoding of UTF-8 byte streams into UNICODE®
        characters.

		utf8decoder_start
		utf8decoder_load
		utf8decoder_read
		utf8decoder_finish

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstring>
#include	<vector>
#include	<new>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"utf8decoder.h"


/* local defines */


/* imported namespaces */

using std::min ;		/* subroutine-template */
using std::max ;		/* subroutine-template */
using std::nothrow ;		/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

class widebuf {
	std::vector<int>	b ;
	int			oi ;		/* output index */
public:
	widebuf() noex : oi(0) { 
	} ;
	widebuf(cchar *sbuf) noex : oi(0) { 
	    int	i ;
	    for (i = 0 ; sbuf[i] ; i += 1) {
		b.push_back(sbuf[i]) ;
	    }
	} ;
	widebuf(cchar *sbuf,int slen) noex : oi(0) {
	    int	i ;
	    if (slen < 0) slen = strlen(sbuf) ;
	    for (i = 0 ; sbuf[i] ; i += 1) {
		b.push_back(sbuf[i]) ;
	    }
	} ;
	int operator [] (int i) const noex {
	    cint	n = b.size() ;
	    int		rch = 0 ;
	    if ((oi+i) < n) rch = b[oi+i] ;
	    return rch ;
	} ;
	widebuf &operator += (int ch) noex {
	    b.push_back(ch) ;
	    return *this ;
	} ;
	int add(int ch) noex {
	    b.push_back(ch) ;
	    return (b.size() - oi) ;
	} ;
	int add(cchar *sp,int sl = -1) noex {
	    if (sl < 0) sl = strlen(sp) ;
	    for (int i = 0 ; i < sl ; i += 1) {
		b.push_back(sp[i]) ;
	    }
	    return (b.size() - oi) ;
	} ;
	int count() const noex {
	    return (b.size() - oi) ;
	} ;
	int len() const noex {
	    return (b.size() - oi) ;
	} ;
	int at(int i) const noex {
	    cint	n = b.size() ;
	    int		rch = 0 ;
	    if ((oi+i) < n) rch = b[oi+i] ;
	    return rch ;
	} ;
	int adv(int al) noex ;
} ; /* end structure (widebuf) */

typedef widebuf *	widebufp ;


/* forward references */

template<typename ... Args>
static inline int utf8decoder_magic(utf8decoder *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == UTF8DECODER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (utf8decoder_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int utf8decoder_start(utf8decoder *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    widebuf	*wbp ;
	    rs = SR_NOMEM ;
	    op->outbuf = nullptr ;
	    op->magic = 0 ;
	    op->code = 0 ;
	    op->rem = 0 ;
	    if ((wbp = new(nothrow) widebuf) != nullptr) {
	        op->outbuf = voidp(wbp) ;
	        op->magic = UTF8DECODER_MAGIC ;
	        rs = SR_OK ;
	    } /* end if (new-widebuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utf8decoder_start) */

int utf8decoder_finish(utf8decoder *op) noex {
	int		rs ;
	if ((rs = utf8decoder_magic(op)) >= 0) {
	    if (op->outbuf) {
	        widebuf *wbp = widebufp(op->outbuf) ;
	        delete wbp ;
	        op->outbuf = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (utf8decoder_finish) */

int utf8decoder_load(utf8decoder *op,cchar *sp,int sl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = utf8decoder_magic(op,sp)) >= 0) {
	    cnullptr	np{} ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (widebuf *wbp ; (wbp = widebufp(op->outbuf)) != np) {
	        while (sl-- > 0) {
		    const uint	uch = *sp++ ;
		    if ((uch & 0x80) == 0) {
	                wbp->add(uch) ;
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
			        op->code |= ((uch & 0x3F) << (op->rem*6)) ;
			        if (op->rem == 0) {
	            	            wbp->add(op->code) ;
		    	            c += 1 ;
		                }
		            } /* end if (process continuation portion) */
		        } /* end if (multi type) */
		    } /* end if (single or multi) */
	       } /* end while */
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (utf8decoder_load) */

int utf8decoder_read(utf8decoder *op,wchar_t *rbuf,int rlen) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = utf8decoder_magic(op,rbuf)) >= 0) {
	    cnullptr	np{} ;
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
		int	ml ;
	        if (widebuf *wbp ; (wbp = widebufp(op->outbuf)) != np) {
	            cint	len = wbp->len() ;
	            ml = min(len,rlen) ;
	            for (i = 0 ; i < ml ; i += 1) {
		        rbuf[i] = wbp->at(i) ;
	            }
	            rbuf[i] = '\0' ;
	            rs = wbp->adv(i) ;
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (positive) */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (utf8decoder_read) */


/* private subroutines */

int widebuf::adv(int al) noex {
	cint		sl = b.size() ;
	int		rl = 0 ;
	if (al > 0) {
	    if (sl > (oi+al)) {
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
	    if (sl > oi) {
		rl = (sl - oi) ;
		oi += rl ;
	    } else {
		b.clear() ;
		oi = 0 ;
	    }
	} /* end if */
	return rl ;
}
/* end subroutine (widebuf::adv) */


