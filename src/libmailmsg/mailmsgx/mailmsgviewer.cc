/* mailmsgviewer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support low-overhead file buffering requirements */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mailmsgviewer

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<vecobj.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"mailmsgviewer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	MMV		mailmsgviewer
#define	MMV_LN		mailmsgviewer_e


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_open(cchar *,int,mode_t) noex ;
    extern int uc_close(int) noex ;
}


/* external variables */


/* local typedefs */


/* local structures */

struct mailmsgviewer_e {
	cchar		*lp ;
	int		ll ;
} ; /* end struct */

typedef mailmsgviewer_e	* mlinep ;


/* forward references */

template<typename ... Args>
local int mailmsgviewer_ctor(MMV *op,Args ... args) noex {
    	MAILMSGVIEWER	*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->llp = new(nothrow) vecobj) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgviewer_ctor) */

local int mailmsgviewer_dtor(MMV *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->llp) ylikely {
		delete op->llp ;
		op->llp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgviewer_dtor) */

template<typename ... Args>
local inline int mailmsgviewer_magic(MMV *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == MAILMSGVIEWER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailmsgviewer_magic) */

local int	mailmsgviewer_mapbegin(MMV *,cc *) noex ;
local int	mailmsgviewer_mapend(MMV *) noex ;
local int	mailmsgviewer_findline(MMV *,int,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsgviewer_open(MMV *op,cchar *fname) noex {
	int		rs ;
	if ((rs = mailmsgviewer_ctor(op,fname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	        cint	esz = szof(MMV_LN) ;
	        cint	vn = 10 ;
	        cint	vo = VECOBJ_OCOMPACT ;
	        if ((rs = vecobj_start(op->llp,esz,vn,vo)) >= 0) ylikely {
		    rs = mailmsgviewer_mapbegin(op,fname) ;
	            if (rs < 0) {
	                vecobj_finish(op->llp) ;
	            }
	        } /* end if (vecobj_start) */
	    } /* end if (valid) */
	    if (rs < 0) {
		mailmsgviewer_dtor(0) ;
	    }
	} /* end if (mailmsgviewer_ctor) */
	return rs ;
}
/* end subroutine (mailmsgviewer_open) */

int mailmsgviewer_close(MMV *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailmsgviewer_magic(op)) >= 0) ylikely {
	    {
	        rs1 = mailmsgviewer_mapend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->llp) ylikely {
	        rs1 = vecobj_finish(op->llp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mailmsgviewer_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgviewer_close) */

int mailmsgviewer_getline(MMV *op,int ln,cchar **lpp) noex {
	int		rs ;
	if ((rs = mailmsgviewer_magic(op,lpp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (ln >= 0) {
		if (void *vp ; (rs = vecobj_get(op->llp,ln,&vp)) >= 0) {
	            MMV_LN	*ep = mlinep(vp) ;
	            *lpp = charp(ep->lp) ;
	            rs = ep->ll ;
	        } else if (rs == SR_NOTFOUND) {
	            rs = SR_OK ;
	            if ((! op->fl.eof) && op->mapdata) {
	                rs = mailmsgviewer_findline(op,ln,lpp) ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgviewer_getline) */

#ifdef	COMMENT

int mailmsgviewer_seek(MMV *op,off_t off,int w) noex {
	int		rs ;
	if ((rs = mailmsgviewer_magic(op)) >= 0) ylikely {
	    switch (w) {
	    case SEEK_SET:
	        break ;
	    case SEEK_END:
	        off = (op->mapsize + off) ;
	        break ;
	    case SEEK_CUR:
	        off = ((op->bp - op->mapdata) + off) ;
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    if (rs >= 0) {
	        if (off < 0) {
	            rs = SR_INVALID ;
	        } else if (off > op->mapsize) {
	            off = op->mapsize ;
	        }
	        op->bp = (op->mapdata + off) ;
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgviewer_seek) */

int mailmsgviewer_tell(MMV *op,off_t *offp) noex {
	int		rs ;
	if ((rs = mailmsgviewer_magic(op,offp)) >= 0) ylikely {
	    rs = SR_OK ;
	    *offp = (op->bp - op->mapdata) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgviewer_tell) */

int mailmsgviewer_rewind(MMV *op) noex {
	int		rs ;
	if ((rs = mailmsgviewer_magic(op)) >= 0) ylikely {
	    rs = SR_OK ;
	    op->bp = op->mapdata ;
	}
	return rs ;
}
/* end subroutine (mailmsgviewer_rewind) */

#endif /* COMMENT */


/* private subroutines */

local int mailmsgviewer_mapbegin(MMV *op,cc *fn) noex {
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
        if ((rs = uc_open(fn,of,0666)) >= 0) ylikely {
            cint    fd = rs ;
            if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
                if (S_ISREG(sb.st_mode)) ylikely {
                    if (sb.st_size > 0) ylikely {
                        cnullptr    np{} ;
                        csize       ms = size_t(sb.st_size) ;
                        cint        mp = PROT_READ ;
                        cint        mf = MAP_SHARED ;
                        void        *md{} ;
                        if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0L,&md)) >= 0) {
                            op->mapdata = charp(md) ;
                            op->mapsize = ms ;
                        }
                    } else {
                        op->fl.eof = true ;
                    }
                } else {
                    rs = SR_PROTO ;
                }
            } /* end if (stat) */
            rs1 = uc_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
        } /* end if (file) */
	return rs ;
}
/* end subroutine (mailmsgviewer_mapbegin) */

local int mailmsgviewer_mapend(MMV *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata && (op->mapsize > 0)) {
	    csize	ms = op->mapsize ;
	    void	*md = voidp(op->mapdata) ;
	    rs1 = u_munmap(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	}
	return rs ;
}
/* end subroutine (mailmsgviewer_mapend) */

local int mailmsgviewer_findline(MMV *op,int ln,cchar **lpp) noex {
	int		rs = SR_OK ;
	int		ll = 0 ;
	*lpp = nullptr ;
	if (! op->fl.eof) {
	    if ((rs = vecobj_count(op->llp)) >= 0) ylikely {
	        MMV_LN		e ;
	        int		c = rs ;
	        int		bl ;
	        cchar		*tp ;
	        cchar		*bp ;
	        if (c > 0) {
		    void	*vp{} ;
	            if ((rs = vecobj_get(op->llp,(c - 1),&vp)) >= 0) {
	                MMV_LN	*ep = mlinep(vp) ;
	                bp = (ep->lp + ep->ll) ;
	                bl = intconv(op->mapsize - (bp - op->mapdata)) ;
	            }
	        } else {
	            bp = op->mapdata ;
	            bl = intsat(op->mapsize) ;
	        } /* end if */
	        if (rs >= 0) {
	            while ((tp = strnchr(bp,bl,'\n')) != nullptr) {
	                *lpp = bp ;
	                ll = intconv((tp + 1) - bp) ;
	                e.lp = bp ;
	                e.ll = intconv((tp + 1) - bp) ;
	                rs = vecobj_add(op->llp,&e) ;
	                if (rs < 0) break ;
	                if (c++ == ln) break ;
	                bl -= intconv((tp + 1) - bp) ;
	                bp = (tp + 1) ;
	            } /* end while */
	            if ((rs >= 0) && (tp == nullptr)) {
	                op->fl.eof = true ;
	                if (bl > 0) {
	                    *lpp = bp ;
	                    ll = bl ;
	                    e.lp = bp ;
	                    e.ll = bl ;
	                    rs = vecobj_add(op->llp,&e) ;
	                } /* end if */
	            } /* end if */
	        } /* end if (ok) */
	    } /* end if (vecobj_count) */
	} /* end if (not-EOF) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsgviewer_findline) */


