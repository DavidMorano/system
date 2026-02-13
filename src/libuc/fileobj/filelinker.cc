/* filelinker SUPPORT */
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
	filelinker

	Names:
	filelinker_start
	filelinker_link
	filelinker_count
	filelinker_finish

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
#include	<usysbase.h>
#include	<ulogerror.h>
#include	<uclibmem.h>
#include	<filerec.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"filelinker.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |resumelife(3u)| + |getlen(3u)| */

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef filerec *	filerecp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int filelinker_magic(filelinker *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == FILELINKER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (filelinker_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int filelinker::start(mainv tlist) noex {
    	cnothrow	nt{} ;
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (tlist) {
	    rs = SR_INVALID ;
	    tarlist = nullptr ;
	    frp = nullptr ;
	    magic = 0 ;
	    if ((tll = lenstrarr(tlist)) > 0) {
	        rs = SR_NOMEM ;
	        tlp = tlist ;
	        if (filerec *p ; (p = new(nt) filerec) != np) ylikely {
		    frp = filerecp(p) ;
		    if ((rs = p->start) >= 0) {
	                magic = FILELINKER_MAGIC ;
		    }
		    if (rs < 0) {
			delete p ;
			frp = nullptr ;
		    } /* end if (error) */
	        } /* end if (new-filerec) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (filelinker_start) */

int filelinker_finish(filelinker *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	if ((rs = filelinker_magic(op)) >= 0) ylikely {
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) ylikely {
	        delete qvp ;
	        op->frp = nullptr ;
	    } else {
	        rs = SR_BUGCHECK ;
	    } /* end if (non-null) */
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filelinker_finish) */

int filelinker_ins(filelinker *op,short v) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = filelinker_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) ylikely {
		if ((rs = qvp->ins(v)) >= 0) ylikely {
		    rs = qvp->count() ;
		    c = rs ;
		}
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (filelinker_ins) */

int filelinker_rem(filelinker *op,short *rp) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = filelinker_magic(op,rp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) ylikely {
		if ((rs = qvp->rem(rp)) >= 0) ylikely {
		    rs = qvp->count() ;
		    c = rs ;
		} /* end if (rem) */
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (filelinker_rem) */

int filelinker_remread(filelinker *op,short *rbuf,int rlen) noex {
	cnullptr	np{} ;
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = filelinker_magic(op,rbuf)) >= 0) ylikely {
	    if (rlen > 0) ylikely {
		int	ml ;
	        if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) {
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
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (positive) */
	    rbuf[i] = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (filelinker_remread) */

int filelinker_remall(filelinker *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = filelinker_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) ylikely {
		rs = qvp->remall() ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (filelinker_remall) */

int filelinker_get(filelinker *op,int ei) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rv = 0 ; /* return-value */
	if ((rs = filelinker_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) ylikely {
		rs = qvp->get(ei) ;
		rv = rs ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (filelinker_get) */

int filelinker_readat(filelinker *op,int ei,short *rbuf,int rlen) noex {
    	cnullptr	np{} ;
    	int		rs ;
	int		rl = 0 ; /* return-value */
	if ((rs = filelinker_magic(op,rbuf)) >= 0) ylikely {
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) ylikely {
		if ((rs = qvp->count()) > 0) {
		    cint cnt = rs ;
		    if ((ei >= 0) && (ei < cnt)) {
			while (rlen-- && (rl < (cnt - ei))) {
			    if ((rs = qvp->get(ei++)) >= 0) {
				rbuf[rl++] = short(rs) ;
			    }
			    if (rs < 0) break ;
			} /* end while */
		    } else {
		        rs = SR_INVALID ;
		    } /* end if (valid) */
		} /* end if (count - non-zero positive) */
	    } else {
	        rs = SR_BUGCHECK ;
	    } /* end if (non-null) */
	    rbuf[rl] = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (filelinker_readat) */

int filelinker_read(filelinker *op,short *rbuf,int rlen) noex {
    	return filelinker_readat(op,0,rbuf,rlen) ;
} /* end subroutine (filelinker_read) */

int filelinker_size(filelinker *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = filelinker_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) ylikely {
		rs = qvp->size() ;
		c = rs ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (filelinker_size) */

int filelinker_count(filelinker *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = filelinker_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) ylikely {
		rs = qvp->count() ;
		c = rs ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (filelinker_count) */

int filelinker_load(filelinker *op,short *sp,int µsl) noex {
	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = filelinker_magic(op,sp)) >= 0) ylikely {
	    if (int sl ; (sl = getlen(sp,µsl)) >= 0) ylikely {
	        if (bmgr *qvp ; (qvp = resumelife<bmgr>(op->frp)) != np) {
	            while ((rs >= 0) && (sl-- > 0)) {
			rs = qvp->ins(sp[c++]) ;
	           } /* end while */
		} else {
		    rs = SR_BUGCHECK ;
		} /* end if (non-null) */
	    } /* end if (getlen) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (filelinker_load) */


/* private subroutines */

void filelinker::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("filelinker",rs,"fini-finish") ;
	}
} /* end method (filelinker::dtor) */

int filelinker::operator [] (int ei) noex {
    return filelinker_get(this,ei) ;
}

filelinker::operator int () noex {
    	return filelinker_count(this) ;
}

int filelinker_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case filelinkermem_count:
	        rs = filelinker_count(op) ;
	        break ;
	    case filelinkermem_finish:
	        rs = filelinker_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (filelinker_co::operator) */


