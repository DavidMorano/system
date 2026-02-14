/* filelinker SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* short-word queue */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */

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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ulogerror.h>
#include	<uclibmem.h>
#include	<strnul.hh>
#include	<localmisc.h>
#include	<dprintf.h>

#include	"filelinker.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/tardir.ccm"
#pragma		GCC dependency		"mod/filerec.ccm"

import libutil ;			/* |resumelife(3u)| + |getlen(3u)| */
import tardir ;
import filerec ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef tardir	*	tardirp ;
typedef filerec *	filerecp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

const uint	filelinker_magicval	= 0x83655680 ;

template<typename ... Args>
local int filelinker_ctor(filelinker *op,Args ... args) noex {
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    op->dirp = nullptr ;
	    op->recp = nullptr ;
	    op->magval = 0 ;
	    op->tll = 0 ;
	    rs = SR_NOMEM ;
	    if (tardir *dlp ; (dlp = new(nt) tardir) != np) ylikely {
		op->dirp = dlp ;
	        if (filerec *rlp ; (rlp = new(nt) filerec) != np) ylikely {
		    op->recp = rlp ;
		    rs = SR_OK ;
	        } /* end if (new-filerec) */
		if (rs < 0) {
		    delete dlp ;
		    op->dirp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-tardir) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (filelinker_ctor) */

local int filelinker_dtor(filelinker *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->recp) {
		filerec *rlp = filerecp(op->recp) ;
		delete rlp ;
		op->recp = nullptr ;
	    }
	    if (op->dirp) {
		tardir *dlp = tardirp(op->dirp) ;
		delete dlp ;
		op->dirp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (filelinker_dtor) */

template<typename ... Args> 
int filelinker_ma::operator () (Args ... args) noex {
    	int		rs = SR_FAULT ;
	if ((... && args)) {
	    rs = (op->magval == filelinker_magicval) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end method (filelinker_ma::operator) */

inline filelinker_ma::operator int () noex {
	return (op->magval == filelinker_magicval) ? SR_OK : SR_NOTOPEN ;
} /* end method (filelinker_ma::operator) */


/* local variables */

cbool		f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int filelinker::istart(mainv tlist) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = filelinker_ctor(this)) >= 0) {
	    if ((rs = istarter()) >= 0) {
		if (tlist) {
		    rs = load(tlist) ;
		    c = rs ;
		}
	    } /* end if (istarter) */
	    if (rs < 0) {
		filelinker_dtor(this) ;
	    } /* end if (error) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end method (filelinker::istart) */

int filelinker::istarter() noex {
	tardir		*dlp = tardirp(dirp) ;
	int		rs ;
	if ((rs = dlp->start) >= 0) {
	    filerec *rlp = filerecp(recp) ;
	    if ((rs = rlp->start) >= 0) {
		magval = filelinker_magicval ;
	    }
	    if (rs < 0) {
		dlp->finish() ;
	    } /* end if (error) */
	} /* end if (tardir_start) */
	return rs ;
} /* end method (filelinker::istarter) */

int filelinker::ifinish() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = magic) >= 0) ylikely {
	    if (recp) {
		filerec *rlp = filerecp(recp) ;
		rs1 = rlp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (dirp) {
		tardir *dlp = tardirp(dirp) ;
		rs1 = dlp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs = filelinker_dtor(this) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (non-null) */
	    magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end method (filelinker::ifinish) */

int filelinker::load(mainv tlist) noex {
    	int		rs = SR_FAULT ;
	int		c = 0 ; /* return-value */
	if ((tll = lenstrarr(tlist)) >= 0) {
	    rs = SR_OK ;
	    if (tll > 0) {
	        if ((rs = magic) >= 0) ylikely {
	            tardir *dlp = tardirp(dirp) ;
	            for (int i = 0 ; (rs >= 0) && tlist[i] ; i += 1) {
		        if (cchar *dn = tlist[i] ; dn[0]) {
		            rs = dlp->add(dn) ;
			    c += rs ;
		        }
	            } /* end for */
	        } /* end if (vecent_magic) */
	    } /* end if (non-zero positive) */
	} /* end if (lenstrarr) */
	return (rs >= 0) ? c : rs ;
} /* end method (filelinker::load) */

int filelinker::add(cchar *sp,int µsl) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = magic(sp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	        tardir *dlp = tardirp(dirp) ;
		rs = dlp->add(sp,sl) ;
		c = rs ;
	    } /* end if (getlenstr) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end method (filelinker::add) */

int filelinker::icount() noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = magic) >= 0) ylikely {
	    tardir *dlp = tardirp(dirp) ;
	    rs = dlp->count ;
	    c = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end method (filelinker::icount) */

int filelinker::link(ustat *sbp,cchar *sp,int sl) noex {
	int		rs ;
	int		rv = 0 ; /* return-value */
	{
	    strnul s(sp,sl) ;
	    DPRINTF("ent s=%s\n",ccp(s)) ;
        }
	if ((rs = magic(sbp,sp)) >= 0) ylikely {
	    tardir *dlp = tardirp(dirp) ;
	    ustat sb ;
	    (void) sl ;
	    cchar *dp ;
	    for (int i = 0 ; dlp->get(i,&sb,&dp) >= 0 ; i += 1) {
	        DPRINTF("get() dp=%s\n",dp) ;
	    } /* end for */
	} /* end if (magic) */
	DPRINTF("ret rs=%d rv=%d\n",rs,rv) ;
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (filelinker::link) */


/* private subroutines */

void filelinker::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("filelinker",rs,"fini-finish") ;
	}
} /* end method (filelinker::dtor) */

filelinker::operator int () noex {
    	return icount() ;
}

int filelinker_st::operator () (mainv arr) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
    	    rs = op->istart(arr) ;
	}
	return rs ;
} /* end method (filelinker_st::operator) */

filelinker_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case filelinkermem_count:
	        rs = op->icount() ;
	        break ;
	    case filelinkermem_finish:
	        rs = op->ifinish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (filelinker_co::operator) */


