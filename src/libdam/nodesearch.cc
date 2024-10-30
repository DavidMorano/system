/* NS SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<cstring>
#include	<usystem.h>
#include	<hdbstr.h>
#include	<mallocstuff.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"nodesfile.h"
#include	"nodesearch.h"


/* local defines */

#define	NS		nodesearch
#define	NS_CUR		nodesearch_cur

#define	TO_CHECK	4
#define	TO_HOLD		2		/* file hold time */

#define	BUFLEN		(2 * MAXPATHLEN)
#define	DATABUFLEN	30


/* external subroutines */

extern "C" {
     extern int	hdbstr_loadfile1(hdnstr *,cchar *) noex ;
}


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */

template<typename ... Args>
static int nodesearch_ctor(lfm *op,Args ... args) noex {
    	NODESEARCH	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nodesearch_ctor) */

static int nodesearch_dtor(lfm *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nodesearch_dtor) */

template<typename ... Args>
static inline int nodesearch_magic(lfm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == NODESEARCH_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (nodesearch_magic) */

static int	nodesearch_filechanged(NS *,time_t) noex ;

#ifdef	COMMENT
static int	hdbstr_search(hdnstr *,cchar *,int) noex ;
#endif

static int	hdbstr_release(hdnstr *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int nodesearch_open(NS *op,cc *fname,int fsize,int n) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	if ((rs = nodesearch_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		rs = SR_NOMEM ;
	        if ((op->fi.fname = mallocstr(fname)) != np) {
		    if (USTAT sb ; (rs = u_stat(op->fi.fname,&sb)) >= 0) {
	    		rs = SR_ISDIR ;
			if (! S_ISDIR(sb.st_mode)) {

	rs = nodesfile_open(&op->a,fname,fsize,O_RDONLY) ;

	if ((rs == SR_PROTO) || (rs == SR_TOOBIG)) {
	    op->fl.sw = TRUE ;
	    if ((rs = hdbstr_start(&op->b,n)) >= 0) {
	        if ((rs = hdbstr_loadfile1(&op->b,fname)) >= 0) {
	            time_t	daytime = time(nullptr) ;
	            op->ti_load = daytime ;
	            if ((rs = u_stat(fname,&sb)) >= 0) {
	                op->fi.mtime = sb.st_mtime ;
	                op->fi.ino = sb.st_ino ;
	                op->fi.dev = sb.st_dev ;
	            }
	        }
	    }
	} /* end if (wrong type of file) */

ret0:
	return rs ;

/* bad stuff */
bad1:
	if (op->fi.fname != nullptr) {
	    uc_free(op->fi.fname) ;
	    op->fi.fname = nullptr ;
	}

bad0:
	return rs ;
}
/* end subroutine (nodesearch_open) */

int nodesearch_close(NS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = nodesearch_magic(op)) >= 0) {
	    if (! op->fl.sw) {
	        rs1 = nodesfile_close(&op->a) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
	        rs1 = hdbstr_finish(&op->b) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fi.fname) {
	        rs1 = uc_free(op->fi.fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fi.fname = nullptr ;
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

int nodesearch_check(NS *op,time_t daytime) noex {
	int		rs = SR_OK ;

	if ((daytime - op->ti_check) < TO_CHECK)
	    return SR_OK ;

	op->ti_check = daytime ;
	if (! op->fl.sw) {

	    rs = nodesfile_check(&op->a,daytime) ;

	} else {

	    rs = nodesearch_filechanged(op,daytime) ;

	    if (rs > 0) {

	        hdbstr_release(&op->b) ;

	        rs = hdbstr_loadfile1(&op->b,op->fi.fname) ;
	        if (rs >= 0)
	            op->ti_load = daytime ;

	    }

	} /* end if */

	return rs ;
}
/* end subroutine (nodesearch_check) */

int nodesearch_search(NS *op,cc *nodename,int nl) noex {
	int		rs = SR_NOANODE ;
	int		sw ;
	int		f_found ;
	cchar		*cp ;

	if (op == nullptr) return SR_FAULT ;

	if (nl < 0)
	    nl = strlen(nodename) ;

	sw = op->fl.sw ;
	switch (sw) {
	case 0:
	    rs = nodesfile_search(&op->a,nodename,nl) ;
	    f_found = (rs >= 0) ;
	    break ;
	case 1:
	    rs = hdbstr_fetch(&op->b,(char *) nodename,nl,nullptr,&cp) ;
	    f_found = (rs >= 0) ;
	    break ;
	} /* end switch */

	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (nodesearch_search) */

int nodesearch_curbegin(NS *op,NS_CUR *curp) noex {
	int		rs = SR_NOANODE ;
	int		sw ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	sw = op->fl.sw ;
	switch (sw) {
	case 0:
	    rs = nodesfile_curbegin(&op->a,&curp->c1) ;
	    break ;
	case 1:
	    rs = hdbstr_curbegin(&op->b,&curp->c2) ;
	    break ;
	} /* end switch */

	return rs ;
}
/* end subroutine (nodesearch_curbegin) */

int nodesearch_curend(NS *op,NS_CUR *curp) noex {
	int		rs = SR_NOANODE ;
	int		sw ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	sw = op->fl.sw ;
	switch (sw) {
	case 0:
	    rs = nodesfile_curend(&op->a,&curp->c1) ;
	    break ;
	case 1:
	    rs = hdbstr_curend(&op->b,&curp->c2) ;
	    break ;
	} /* end switch */

	return rs ;
}
/* end subroutine (nodesearch_curend) */

int nodesearch_enum(NS *op,NS_CUR *curp,char *np,int nl) noex {
	int		rs = SR_NOANODE ;
	int		sw ;
	int		kl, vl ;
	cchar		*kp, *vp ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (np == nullptr) return SR_FAULT ;

	if (nl < 0)
	    nl = strlen(nodename) ;

	sw = op->fl.sw ;
	switch (sw) {
	case 0:
	    rs = nodesfile_enum(&op->a,&curp->c1,np,nl) ;
	    break ;
	case 1:
	    if ((rs = hdbstr_curenum(&op->b,&curp->c2,&kp,&vp,&vl)) >= 0) {
	        kl = (nl >= 0) ? MIN(rs,nl) : rs ;
	        rs = snwcpy(np,nl,kp,kl) ;
	    }
	    break ;
	} /* end switch */

	return rs ;
}
/* end subroutine (nodesearch_enum) */


/* private subroutines */

static int nodesearch_filechanged(NS *op,time_t daytime) noex {
	int		rs ;
	int		f = false ;
	if (USTAT sb ; (rs = u_stat(op->fi.fname,&sb)) >= 0) {
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

#ifdef	COMMENT

static int hdbstr_search(hdnstr *hsp,cc *nodename,int nl) noex {
    	cnullptr	np{} ;
	int		rs = SR_BUGCHECK ;
	int		f_found = false ;
	if (hsp) {
	    if (nl < 0) nl = strlen(nodename) ;
	    if ((rs = hdbstr_fetch(hsp,(char *) nodename,nl,np,np)) >= 0) {
	        f_found = true ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (hdbstr_search) */

#endif /* COMMENT */

static int hdbstr_release(hdnstr *hsp) noex {
    	cnullptr	np{} ;
	int		rs = SR_BUGCHECK ;
	if (hsp) {
	    hdbstr_cur	cur ;
	    if ((rs = hdbstr_curbegin(hsp,&cur)) >= 0) {
	        while (hdbstr_curenum(hsp,&cur,np,np,np) >= 0) {
		    hdbstr_delcur(hsp,&cur,0) ;
	        }
	        hdbstr_curend(hsp,&cur) ;
	    } /* end if (hdbstr-cur) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_release) */


