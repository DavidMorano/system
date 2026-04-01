/* filesyner SUPPORT */
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
	filesyner

	Names:
	filesyner_start
	filesyner_link
	filesyner_count
	filesyner_finish

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
#include	<usyscalls.h>
#include	<umem.hh>
#include	<ulogerror.h>
#include	<strnul.hh>
#include	<localmisc.h>
#include	<dprintf.hh>

#include	"filesyner.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"
#pragma		GCC dependency		"mod/umisc.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"
#pragma		GCC dependency		"mod/tardir.ccm"
#pragma		GCC dependency		"mod/filerec.ccm"

import libutil ;			/* |resumelife(3u)| + |getlen(3u)| */
import ulibvals ;			/* |ulibval(3u)| */
import umisc ;				/* |mknpathxw(3u)| */
import ureserve ;			/* |isNot{x}(3u)| + |isOneOf(3u)| */
import tardir ;
import filerec ;

/* local defines */

#define	FL		filesyner

#define	US		ustat
#define	CUS		custat

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libu::umem ;			/* variable */


/* local typedefs */

typedef tardir	*	tardirp ;
typedef filerec *	filerecp ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpath ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

const uint	filesyner_magicval	= FILELINKER_MAGIC ;

template<typename ... Args>
local int filesyner_ctor(filesyner *op,Args ... args) noex {
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
} /* end subroutine (filesyner_ctor) */

local int filesyner_dtor(filesyner *op) noex {
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
} /* end subroutine (filesyner_dtor) */

template<typename ... Args> 
int filesyner_ma::operator () (Args ... args) noex {
    	int		rs = SR_FAULT ;
	if ((... && args)) {
	    rs = (op->magval == filesyner_magicval) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end method (filesyner_ma::operator) */

filesyner_ma::operator int () noex {
	return (op->magval == filesyner_magicval) ? SR_OK : SR_NOTOPEN ;
} /* end method (filesyner_ma::operator) */

local bool isnotsame(custat *s1p,custat *s2p) noex {
    	bool	f = false ;
	f = f || (s1p->st_ino != s2p->st_ino) ;
	f = f || (s1p->st_dev != s2p->st_dev) ;
	return f ;
} /* end subroutine (isnotsame) */

local bool isNotDir(int) noex ;


/* local variables */

static const int	maxpathlen = ulibval.maxpathlen ;

static vars		var ;

cbool			f_debug = CF_DEBUG ;

constexpr int		rsnotdir[] = {
    	SR_NOTDIR,
	SR_NOENT,
	0
} ; /* end array (rsnotdir) */


/* exported variables */


/* exported subroutines */

int filesyner::istart(mainv tlist) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = filesyner_ctor(this)) >= 0) {
	    if ((rs = maxpathlen) >= 0) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
		    cint sz = ((var.maxpath + 1) * 2) ;
		    int ai = 0 ;
		    plen = var.maxpath ;
		    dlen = var.maxpath ;
		    if ((rs = umem.mall(sz,&a)) >= 0) {
			pbuf = (a + ((var.maxpath + 1) * ai++)) ;
			dbuf = (a + ((var.maxpath + 1) * ai++)) ;
	                if ((rs = istarter()) >= 0) {
		            if (tlist) {
		                rs = load(tlist) ;
		                c = rs ;
		            }
	                } /* end if (istarter) */
		        if (rs < 0) {
			    umem.free(pbuf) ;
			    plen = 0 ;
		        }
		    } /* end if (memory-allocation) */
		} /* end if (vars) */
	    } /* end if (maxpathlen) */
	    if (rs < 0) {
		filesyner_dtor(this) ;
	    } /* end if (error) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end method (filesyner::istart) */

int filesyner::istarter() noex {
	tardir		*dlp = tardirp(dirp) ;
	int		rs ;
	if ((rs = dlp->start) >= 0) {
	    filerec *rlp = filerecp(recp) ;
	    if ((rs = rlp->start) >= 0) {
		magval = filesyner_magicval ;
	    }
	    if (rs < 0) {
		dlp->finish() ;
	    } /* end if (error) */
	} /* end if (tardir_start) */
	return rs ;
} /* end method (filesyner::istarter) */

int filesyner::ifinish() noex {
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
	    if (a) {
		rs1 = umem.free(a) ;
	        if (rs >= 0) rs = rs1 ;
		a = nullptr ;
		pbuf = nullptr ;
		dbuf = nullptr ;
		plen = 0 ;
		dlen = 0 ;
	    }
	    {
		rs = filesyner_dtor(this) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (non-null) */
	    magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end method (filesyner::ifinish) */

int filesyner::load(mainv tlist) noex {
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
} /* end method (filesyner::load) */

int filesyner::add(cchar *sp,int µsl) noex {
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
} /* end method (filesyner::add) */

int filesyner::icount() noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = magic) >= 0) ylikely {
	    tardir *dlp = tardirp(dirp) ;
	    rs = dlp->count ;
	    c = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end method (filesyner::icount) */

local int linkover(cchar *pbuf,custat *sbp,cchar *sp,int sl) noex {
	strnul		sfn(sp,sl) ;
	int		rs ;
	int		c = 0 ;
	if (S_ISDIR(sbp->st_mode)) {
	    cmode dm = (sbp->st_mode | S_IRWXU) ;
	    rs = u_mkdirs(pbuf,dm) ;
	    c = rs ;
	} else {
	    rs = u_linkfile(sfn,pbuf) ;
	    c = 1 ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (linkover) */

local int rmother(custat *sbp,cchar *pbuf) noex {
    	int		rs ;
	if (S_ISDIR(sbp->st_mode)) {
	    rs = u_rmdirs(pbuf) ;
	} else {
	    rs = u_unlink(pbuf) ;
	}
	return rs ;
} /* end subroutine (rmother) */

int filesyner::idir(custat *fsbp,custat *sbp,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (! S_ISDIR(fsbp->st_mode)) {
	    if ((rs = rmother(fsbp,pbuf)) >= 0) {
		rs = linkover(pbuf,sbp,sp,sl) ;
		c += 1 ;
	    } /* end if (rmother) */
	} /* end if (target-file not a directory) */
	return (rs >= 0) ? c : rs ;
} /* end method (filesyner::idir) */

int filesyner::ione(custat *tsb,cc *tdp,custat *sbp,cc *sp,int sl) noex {
    	int		rs ;
	int		c = 0 ; /* return-value */
	(void) tsb ;
	if ((rs = mknpathw(pbuf,plen,tdp,sp,sl)) >= 0) {
	    if (ustat fsb ; (rs = u_lstat(pbuf,&fsb)) >= 0) {
		if (isnotsame(&fsb,sbp)) {
		    if (S_ISDIR(sbp->st_mode)) {
		        rs = idir(&fsb,sbp,sp,sl) ;
		        c = rs ;
		    } else {
		        if ((rs = rmother(&fsb,pbuf)) >= 0) {
			    rs = linkover(pbuf,sbp,sp,sl) ;
			    c += 1 ;
		        } /* end if (rmother) */
		    } /* end if */
		} /* end if (file was different) */
	    } else if (isNotPresent(rs)) {
		if ((rs = linkover(pbuf,sbp,sp,sl)) >= 0) {
		    c += 1 ;
		} else if (isNotDir(rs)) {
	    	    cmode dm = (sbp->st_mode | S_IRWXU) ;
		    if ((rs = mkpdirs(pbuf,dm)) >= 0) {
			c += rs ;
			rs = linkover(pbuf,sbp,sp,sl) ;
			c += rs ;
		    } /* end if (mkpdirs) */
		} /* end if */
	    } /* end if (file present or not) */
	} /* end if (mknpathw) */
	return (rs >= 0) ? c : rs ;
} /* end method (filesyner::ione) */

local int filesyner_linker(FL *op,custat *sbp,cchar *sp,int sl) noex {
	tardir *dlp = tardirp(op->dirp) ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	ustat tsb ;
	cchar *tdp ;
	for (int i = 0 ; (rs1 = dlp->get(i,&tsb,&tdp)) >= 0 ; i += 1) {
	    DPRINTF("get() tdp=%s\n",tdp) ;
	    rs = op->ione(&tsb,tdp,sbp,sp,sl) ;
	    c += rs ;
	} /* end for */
	DPRINTF("for-out rs1=%d\n",rs1) ;
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (filesyner_linker) */

int filesyner::link(custat *sbp,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ; /* return-value */
	if (int sl ; sbp && ((sl = getlenstr(sp,µsl)) >= 0)) {
	    rs = SR_INVALID ;
	    if (sl > 0) {
	        {
	            strnul s(sp,sl) ;
	            DPRINTF("ent s=%s\n",ccp(s)) ;
                }
	        if ((rs = magic) >= 0) ylikely {
		    rs = filesyner_linker(this,sbp,sp,sl) ;
		    c = rs ;
	        } /* end if (magic) */
	    } /* end if (valid) */
	} /* end if (getlenstr) */
	DPRINTF("ret rs=%d rv=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (filesyner::link) */


/* private subroutines */

int filesyner::mkpdirs(cchar *tarfname,mode_t dm) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*dp ;
	if (int dl ; (dl = sfdirname(tarfname,-1,&dp)) > 0) {
	    if ((rs = mknpath1w(dbuf,dlen,dp,dl)) >= 0) {
		if (ustat dsb ; (rs = u_stat(dbuf,&dsb)) >= 0) {
		    rs = SR_OK ;
		} else if (isNotPresent(rs)) {
	            rs = u_mkdirs(dbuf,dm) ;
		    c = rs ;
		} /* end if (u_stat) */
	    } /* end if (mknpath1w) */
	} /* end if (sfdirname) */
	return (rs >= 0) ? c : rs ;
} /* end method (filesyner::mkpdirs) */

void filesyner::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("filesyner",rs,"fini-finish") ;
	}
} /* end method (filesyner::dtor) */

filesyner::operator int () noex {
    	return icount() ;
} /* end method (filesyner::operator) */

int filesyner_st::operator () (mainv arr) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
    	    rs = op->istart(arr) ;
	}
	return rs ;
} /* end method (filesyner_st::operator) */

filesyner_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case filesynermem_count:
	        rs = op->icount() ;
	        break ;
	    case filesynermem_finish:
	        rs = op->ifinish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (filesyner_co::operator) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = maxpathlen) >= 0) {
	    maxpath = rs ;
	}
    	return rs ;
} /* end method (vars::operator) */

local bool isNotDir(int rs) noex {
    	return isOneOf(rsnotdir,rs) ;
} /* end subroutine (isNotDir) */


