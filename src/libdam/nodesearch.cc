/* nodesearch SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support searching for a node name in the cluster */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	nodesearch

	Description:
	This little object provides multiplexing of two ways to
	search the cluster 'nodes' file.  If the file is 'regular',
	the NODESFILE object is used to process it (for speed).  If
	the file is anything other than a 'regular' file, then a
	HDBSTR object is used to process it and the file is read
	line-by-line not assuming that the file may be seekable
	(suitable for pipes, FIFOs, sockets, and the like).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocstuff.h>
#include	<absfn.h>
#include	<hdbstr.h>
#include	<strwcpy.h>
#include	<nodesfile.h>
#include	<localmisc.h>

#include	"nodesearch.h"

import libutil ;

/* local defines */

#define	NS		nodesearch
#define	NS_CUR		nodesearch_cur

#define	TO_CHECK	4
#define	TO_HOLD		2		/* file hold time */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int nodesearch_ctor(NS *op,Args ... args) noex {
    	NODESEARCH	*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->nfp = new(nothrow) nodesfile) != np) {
		rs = SR_OK ;
	    } /* end if (new-nodesfile) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nodesearch_ctor) */

static int nodesearch_dtor(NS *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->nfp) {
		delete op->nfp ;
		op->nfp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nodesearch_dtor) */

template<typename ... Args>
static inline int nodesearch_magic(NS *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == NODESEARCH_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (nodesearch_magic) */

#ifdef	COMMENT
static int	nodesearch_filechanged(NS *,time_t) noex ;
#endif /* COMMENT */


/* local variables */


/* exported variables */


/* exported subroutines */

int nodesearch_open(NS *op,cc *fname,int fsz) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	if ((rs = nodesearch_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		cchar	*fn{} ;
		if (absfn af ; (rs = af.start(fname,-1,&fn)) >= 0) {
		    if (rs = SR_NOMEM ; (op->fi.fn = mallocstr(fn)) != np) {
		        if (USTAT sb ; (rs = u_stat(op->fi.fn,&sb)) >= 0) {
	    		    rs = SR_ISDIR ;
			    if (! S_ISDIR(sb.st_mode)) {
			        nodesfile	*nfp = op->nfp ;
			        if ((rs = nodesfile_open(nfp,fn,fsz)) >= 0) {
				    op->magic = NODESEARCH_MAGIC ;
			        }
			    }
		        } /* end if (stat) */
		        if (rs < 0) {
			    if (op->fi.fn) {
	    		        uc_free(op->fi.fn) ;
	    		        op->fi.fn = nullptr ;
			    }
		        } /* end if (error-handling) */
	            } /* end if (memory-allocation) */
		    rs1 = af.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (absfn) */
	    } /* end if (valid) */
	    if (rs < 0) {
		nodesearch_dtor(op) ;
	    }
	} /* end if (nodesearch_ctor) */
	return rs ;
}
/* end subroutine (nodesearch_open) */

int nodesearch_close(NS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = nodesearch_magic(op)) >= 0) {
	    if (op->nfp) {
	        rs1 = nodesfile_close(op->nfp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fi.fn) {
	        rs1 = uc_free(op->fi.fn) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fi.fn = nullptr ;
	    }
	    {
	        rs1 = nodesearch_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodesearch_close) */

int nodesearch_check(NS *op,time_t dt) noex {
	int		rs ;
	if ((rs = nodesearch_magic(op)) >= 0) {
	    if (dt == 0) dt = getustime ;
	    if ((dt - op->ti_check) >= TO_CHECK) {
		op->ti_check = dt ;
	    	rs = nodesfile_check(op->nfp,dt) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodesearch_check) */

int nodesearch_search(NS *op,cc *sp,int sl) noex {
	int		rs ;
	int		f_found = false ;
	if ((rs = nodesearch_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if ((rs = nodesfile_have(op->nfp,sp,sl)) >= 0) {
	        f_found = true ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (nodesearch_search) */

int nodesearch_curbegin(NS *op,NS_CUR *curp) noex {
	int		rs ;
	if ((rs = nodesearch_magic(op,curp)) >= 0) {
	    cint	osz = szof(nodesfile_cur) ;
	    if (void *vp{} ; (rs = uc_malloc(osz,&vp)) >= 0) {
		curp->nfcp = (nodesfile_cur *) vp ;
	        rs = nodesfile_curbegin(op->nfp,curp->nfcp) ;
	        if (rs < 0) {
		    uc_free(curp->nfcp) ;
		    curp->nfcp = nullptr ;
		}
	    } /* end if (memory-allocatiob) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodesearch_curbegin) */

int nodesearch_curend(NS *op,NS_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = nodesearch_magic(op,curp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (curp->nfcp) {
		rs = SR_OK ;
		{
	            rs1 = nodesfile_curend(op->nfp,curp->nfcp) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	        {
		    rs1 = uc_free(curp->nfcp) ;
		    if (rs >= 0) rs = rs1 ;
		    curp->nfcp = nullptr ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodesearch_curend) */

int nodesearch_curenum(NS *op,NS_CUR *curp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = nodesearch_magic(op,curp,rbuf)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (curp->nfcp) {
	        rs = nodesfile_curenum(op->nfp,curp->nfcp,rbuf,rlen) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodesearch_curenum) */


/* private subroutines */

#ifdef	COMMENT
static int nodesearch_filechanged(NS *op,time_t daytime) noex {
	int		rs ;
	int		f = false ;
	if (USTAT sb ; (rs = u_stat(op->fi.fn,&sb)) >= 0) {
	    f = (op->fi.mtime > sb.st_mtime) ;
	    f = f || (op->fi.ino != sb.st_ino) ;
	    f = f || (op->fi.dev != sb.st_dev) ;
	    if (f) {
	        f = f && ((daytime - op->fi.mtime) >= TO_HOLD) ;
	    }
	    if (f) {
	        op->fi.mtime = sb.st_mtime ;
	        op->fi.ino = sb.st_ino ;
	        op->fi.dev = sb.st_dev ;
	    }
	} /* end if (u_stat) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (nodesearch_filechanged) */
#endif /* COMMENT */


