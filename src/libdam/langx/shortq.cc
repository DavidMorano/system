/* shortq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* short-word queue */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was made from scratch, although it is (very) similar to
	many others that I have made like it.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	shortq

	Names:
	shortq_start
	shortq_ins
	shortq_rem
	shortq_size
	shortq_count
	shortq_finish

	Description:
	This object implements a queue of short-words.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<deque>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ulogerror.h>
#include	<uclibmem.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"shortq.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |resumelife(3u)| */

/* local defines */


/* imported namespaces */

using std::deque ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    class bmgr {
	deque<short>	b ;
    public:
	bmgr() = delete ;
	bmgr(int a) noex { 
	    (void) a ;
	} ; /* end ctor */
	int ins(short v) noex {
	    int		rs = SR_OK ;
	    try {
	        b.push_back(v) ;
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	    return rs ;
	} ; /* end method (ins) */
	int ins(cshort *sp,int sl) noex {
	    int		rs = SR_FAULT ;
	    if (sp) {
	        rs = SR_INVALID ;
	        if (sl >= 0) {
	            try {
		        rs = SR_OK ;
	                for (int i = 0 ; i < sl ; i += 1) {
		            b.push_back(*sp) ;
	                }
	            } catch (...) {
		        rs = SR_NOMEM ;
	            }
	        } /* end if */
	    } /* end if (non-null) */
	    return rs ;
	} ; /* end method (ins) */
	int rem(short *rp) noex {
	    int		rs = SR_FAULT ;
	    if (rp) {
		if (! b.empty()) {
	            cshort v = b.back() ;
		    rs = SR_OK ;
	            *rp = v ;
		    b.pop_back() ;
		} else {
		    rs = SR_EMPTY ;
		}
	    } /* end if (non-null) */
	    return rs ;
	} ; /* end method (rem) */
	int remall() noex {
	    int		rs = SR_OK ;
	    if (! b.empty()) {
		b.clear() ;
	    }
	    return rs ;
	} ; /* end method (remall) */
	int size() const noex {
	    const int	cnt = intsat(b.max_size()) ;
	    return cnt ;
	} ; /* end method (size) */
	int count() const noex {
	    csize	bsize = b.size() ;
	    int		rs ;
	    {
		rs = intconv(bsize) ;
	    }
	    return rs ;
	} ; /* end method (count) */
    } ; /* end structure (bmgr) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int shortq_magic(shortq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SHORTQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (shortq_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int shortq_start(shortq *op,int a) noex {
    	cnothrow	nt{} ;
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    op->qvp = nullptr ;
	    op->magic = 0 ;
	    if (a >= 0) {
	        rs = SR_NOMEM ;
	        if (bmgr *qvp ; (qvp = new(nt) bmgr(a)) != np) {
	            op->qvp = qvp ;
	            op->magic = SHORTQ_MAGIC ;
	            rs = SR_OK ;
	        } /* end if (new-bmgr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (shortq_start) */

int shortq_finish(shortq *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	if ((rs = shortq_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->qvp)) != np) {
	        delete qvp ;
	        op->qvp = nullptr ;
	    } /* end if (non-null) */
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (shortq_finish) */

int shortq_ins(shortq *op,short v) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = shortq_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->qvp)) != np) {
		if ((rs = qvp->ins(v)) >= 0) {
		    rs = qvp->count() ;
		    c = rs ;
		}
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (shortq_ins) */

int shortq_rem(shortq *op,short *rp) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = shortq_magic(op,rp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->qvp)) != np) {
		if ((rs = qvp->rem(rp)) >= 0) {
		    rs = qvp->count() ;
		    c = rs ;
		}
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (shortq_rem) */

int shortq_remall(shortq *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = shortq_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->qvp)) != np) {
		rs = qvp->remall() ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (shortq_remall) */

int shortq_size(shortq *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = shortq_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->qvp)) != np) {
		rs = qvp->size() ;
		c = rs ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (shortq_size) */

int shortq_count(shortq *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = shortq_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->qvp)) != np) {
		rs = qvp->count() ;
		c = rs ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (shortq_count) */

int shortq_load(shortq *op,cchar *sp,int µsl) noex {
	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = shortq_magic(op,sp)) >= 0) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
		rs = SR_BUGCHECK ;
	        if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->qvp)) != np) {
	            while ((rs >= 0) && (sl-- > 0)) {
			rs = SR_OK ;
			c += 1 ;
	           } /* end while */
		} /* end if (non-null) */
	    } /* end if (getlenstr) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (shortq_load) */

int shortq_read(shortq *op,short *rbuf,int rlen) noex {
	cnullptr	np{} ;
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = shortq_magic(op,rbuf)) >= 0) {
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
		int	ml ;
	        if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->qvp)) != np) {
	            cint	len = qvp->count() ;
	            ml = min(len,rlen) ;
	            for (i = 0 ; (rs >= 0) && (i < ml) ; i += 1) {
			if (short v ; (rs = qvp->rem(&v)) >= 0) {
			    rbuf[i] = v ;
			} else if (rs == SR_EMPTY) {
			    rs = SR_OK ;
			    break ;
			}
	            } /* end for */
	            rbuf[i] = 0 ;
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (positive) */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (shortq_read) */


/* private subroutines */

int shortq::ins(short v) noex {
    return shortq_ins(this,v) ;
}

int shortq::rem(short *rp) noex {
    return shortq_rem(this,rp) ;
}

void shortq::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("shortq",rs,"fini-finish") ;
	}
} /* end method (shortq::dtor) */

shortq::operator int () noex {
    	return shortq_count(this) ;
}

int shortq_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case shortqmem_start:
	        rs = shortq_start(op,a) ;
	        break ;
	    case shortqmem_remall:
	        rs = shortq_remall(op) ;
	        break ;
	    case shortqmem_size:
	        rs = shortq_size(op) ;
	        break ;
	    case shortqmem_count:
	        rs = shortq_count(op) ;
	        break ;
	    case shortqmem_len:
	        rs = shortq_count(op) ;
	        break ;
	    case shortqmem_finish:
	        rs = shortq_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (shortq_co::operator) */


