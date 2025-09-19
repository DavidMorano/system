/* calmgr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calendar manager object */
/* version %I% last-modified %G% */

#define	CF_EMPTYTERM	1		/* terminate entry on empty line */
#define	CF_SAMECITE	0		/* same entry citation? */
#define	CF_ALREADY	1		/* do not allow duplicate results */
#define	CF_MKDIRS	0		/* |mkdname()| */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	calmgr

	Description:
	We manage a single calendar object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getusername.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<vecobj.h>
#include	<sbuf.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<tmtime.hh>
#include	<fsdir.h>
#include	<mkdirs.h>
#include	<mkpathx.h>
#include	<snx.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<localmisc.h>		/* |COLUMNS| */

#include	"calmgr.h"
#include	"calent.h"
#include	"cyi.h"
#include	"cyimk.h"

import libutil ;

/* local defines */

#define	CALMGR_DBSUF	"calendar"
#define	CALMGR_MAXLINES	40		/* maximum lines per entry */

#define	IDXDNAME	".calyears"

#define	CEBUFLEN	((CALMGR_MAXLINES * COLUMNS) + (3 * szof(int)))


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct calmgr_idx {
	cyi		cy ;
	int		year ;
	uint		f_open:1 ;
} ;

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int calmgr_ctor(calmgr *op,Args ... args) noex {
    	CALMGR		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->idxp = new(nothrow) vechand) != np) {
		rs = SR_OK ;
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (calmgr_ctor) */

static int calmgr_dtor(calmgr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->idxp) {
		delete op->idxp ;
		op->idxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (calmgr_dtor) */

template<typename ... Args>
static inline int calmgr_magic(calmgr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CALMGR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (calmgr_magic) */

static int	calmgr_argbegin(calmgr *,cchar *,cchar *) noex ;
static int	calmgr_argend(calmgr *) noex ;
static int	calmgr_dbloadbegin(calmgr *,time_t) noex ;
static int	calmgr_dbloadend(calmgr *) noex ;
static int	calmgr_dbmapcreate(calmgr *,time_t) noex ;
static int	calmgr_dbmapdestroy(calmgr *) noex ;
static int	calmgr_dbmaper(calmgr *,cc *,time_t) noex ;
static int	calmgr_idxdir(calmgr *) noex ;

static int	calmgr_lookyear(calmgr *,calmgr_q *,cyi **) noex ;
static int	calmgr_lookone(calmgr *,vecobj *,cyi *,calmgr_q *) noex ;

static int	calmgr_mkidx(calmgr *,int) noex ;

static int	calmgr_idxbegin(calmgr *,calmgr_idx *,int) noex ;
static int	calmgr_idxend(calmgr *,calmgr_idx *) noex ;
static int	calmgr_idxends(calmgr *) noex ;
static int	calmgr_idxaudit(calmgr *,calmgr_idx *) noex ;

static int	calmgr_cyiopen(calmgr *,calmgr_idx *,int) noex ;
static int	calmgr_cyiclose(calmgr *,calmgr_idx *) noex ;

static int	calmgr_mkcyi(calmgr *,int) noex ;

static int	calmgr_mapdata(calmgr *,cchar **) noex ;

#if	CF_MKDIRS
static int	calmgr_mkdirs(calmgr *,cchar *,mode_t) noex ;
#endif /* CF_MKDIRS */

static int	mkbve_start(cyimk_ent *,cchar *,calent *) noex ;
static int	mkbve_finish(cyimk_ent *) noex ;

static bool	isempty(cchar *,int) noex ;


/* exported variables */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int calmgr_start(calmgr *op,calyears *cyp,int cidx,cchar *dn,cchar *cn) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = calmgr_ctor(op,cyp,dn,cn)) >= 0) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        op->cyp = cyp ; /* parent object */
	        op->cidx = cidx ; /* parent index */
	        if ((rs = calmgr_argbegin(op,dn,cn)) >= 0) {
	            if ((rs = calmgr_dbloadbegin(op,dt)) >= 0) {
	                if ((rs = calmgr_idxdir(op)) >= 0) {
	                    cint	vo = VECHAND_OSTATIONARY ;
	                    if ((rs = vechand_start(op->idxp,1,vo)) >= 0) {
	                        op->fl.idxes = true ;
				op->magic = CALMGR_MAGIC ;
	                    }
	                } /* end if (calmgr_idxdir) */
	                if (rs < 0) {
	                    calmgr_dbloadend(op) ;
		        }
	            } /* end if (calmgr_dbloadbegin) */
	            if (rs < 0) {
	                calmgr_argend(op) ;
	            }
	        } /* end if (calmgr_argbegin) */
	    } /* end if (vars) */
	    if (rs < 0) {
		calmgr_dtor(op) ;
	    }
	} /* end if (calmgr_ctor) */
	return rs ;
}
/* end subroutine (calmgr_start) */

int calmgr_finish(calmgr *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = calmgr_magic(op)) >= 0) {
	    if (op->fl.idxes) {
	        {
	            rs1 = calmgr_idxends(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            op->fl.idxes = false ;
	            rs1 = vechand_finish(op->idxp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    }
	    {
	        rs1 = calmgr_dbloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }	
	    if (op->idxdname) {
	        rs1 = uc_free(op->idxdname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->idxdname = nullptr ;
	    }
	    {
	        rs1 = calmgr_argend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = calmgr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calmgr_finish) */

int calmgr_lookup(calmgr *op,vecobj *rlp,calmgr_q *qp) noex {
	int		rs ;
	if ((rs = calmgr_magic(op,rlp,qp)) >= 0) {
	    cyi		*cyp ;
	    if ((rs = calmgr_lookyear(op,qp,&cyp)) >= 0) {
	        rs = calmgr_lookone(op,rlp,cyp,qp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calmgr_lookup) */

int calmgr_getci(calmgr *op) noex {
    	int		rs ;
	if ((rs = calmgr_magic(op)) >= 0) {
	    rs = op->cidx ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calmgr_getci) */

int calmgr_getbase(calmgr *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = calmgr_magic(op,rpp)) >= 0) {
	    cchar	*md{} ;
	    rs = SR_INVALID ;
	    if (op->mapdata) {
	         md = op->mapdata ;
	         rs = intconv(op->mapsize) ;
	    }
	    *rpp = (rs >= 0) ? md : nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calmgr_getbase) */

int calmgr_gethash(calmgr *op,calent *ep,uint *rp) noex {
	int		rs ;
	if ((rs = calmgr_magic(op,ep,rp)) >= 0) {
	    if ((rs = calent_gethash(ep,rp)) == 0) {
	        cchar	*md = op->mapdata ;
	        if ((rs = calent_mkhash(ep,md)) >= 0) {
		    rs = calent_gethash(ep,rp) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calmgr_gethash) */

int calmgr_loadbuf(calmgr *op,char *rbuf,int rlen,calent *ep) noex {
	int		rs ;
	if ((rs = calmgr_magic(op,rbuf,ep)) >= 0) {
	    if (cchar *md ; (rs = calmgr_mapdata(op,&md)) >= 0) {
	        rs = calent_loadbuf(ep,rbuf,rlen,md) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calmgr_loadbuf) */

int calmgr_audit(calmgr *op) noex {
	int		rs ;
	if ((rs = calmgr_magic(op)) >= 0) {
	    vechand	*ilp = op->idxp ;
	    void	*vp{} ;
	    for (int i = 0 ; ilp->get(i,&vp) >= 0 ; i += 1) {
	        calmgr_idx	*cip = (calmgr_idx *) vp ;
	        if (vp) {
	            rs = calmgr_idxaudit(op,cip) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (calmgr_audit) */


/* private subroutines */

static int calmgr_argbegin(calmgr *op,cchar *dn,cchar *cn) noex {
	int		rs ;
	int		sz = 0 ;
	sz += (lenstr(dn)+1) ;
	sz += (lenstr(cn)+1) ;
	if (char *bp{} ; (rs = uc_malloc(sz,&bp)) >= 0) {
	    op->a = bp ;
	    op->dn = bp ;
	    bp = (strwcpy(bp,dn,-1)+1) ;
	    op->cn = bp ;
	    bp = (strwcpy(bp,cn,-1)+1) ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (calmgr_argbegin) */

static int calmgr_argend(calmgr *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a != nullptr) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	}
	return rs ;
}
/* end subroutine (calmgr_argend) */

static int calmgr_dbloadbegin(calmgr *op,time_t dt) noex {
	int		rs ;
	{
	    rs = calmgr_dbmapcreate(op,dt) ;
	}
	return rs ;
}
/* end subroutine (calmgr_dbloadbegin) */

static int calmgr_dbloadend(calmgr *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = calmgr_dbmapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (calmgr_dbloadend) */

static int calmgr_dbmapcreate(calmgr *op,time_t dt) noex {
    	cint		maxpath = var.maxpathlen ;
	cint		sz = ((var.maxpathlen + 1) * 2) ;
	int		rs ;
	int		ai = 0 ;
	if (char *a{} ; (rs = uc_malloc(sz,&a)) >= 0) {
	    cint	nlen = maxpath ;
	    cchar	*suf = CALMGR_DBSUF ;
	    char	*nbuf = (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = snsds(nbuf,nlen,op->cn,suf)) >= 0) {
	        char	*dbuf = (a + ((maxpath + 1) * ai++)) ;
	        if ((rs = mkpath2(dbuf,op->dn,nbuf)) >= 0) {
		    rs = calmgr_dbmaper(op,dbuf,dt) ;
	        } /* end if (mkpath) */
	    } /* end if (snsds) */
	    rs = rsfree(rs,a) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (calmgr_dbmapcreate) */

static int calmgr_dbmaper(calmgr *op,cc *dbuf,time_t dt) noex {
	cnullptr	np{} ;
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	cmode		om = 0666 ;
        if ((rs = u_open(dbuf,of,om)) >= 0) {
            cint        fd = rs ;
            if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
                if (S_ISREG(sb.st_mode)) {
                    csize       szm = size_t(INT_MAX) ;
                    op->fsize = size_t(sb.st_size) ;
                    op->ti_db = sb.st_mtime ;
                    if (op->fsize <= szm) {
                        csize   ms = op->fsize ;
                        cint    mp = PROT_READ ;
                        cint    mf = MAP_SHARED ;
                        void    *md{} ;
                        if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
                            if (dt == 0) dt = getustime ;
                            op->mapdata = charp(md) ;
                            op->mapsize = op->fsize ;
                            op->ti_map = dt ;
                            op->ti_lastcheck = dt ;
                        } /* end if (u_mmapbegin) */
                    } else {
                        rs = SR_TOOBIG ;
                    }
                } else {
                    rs = SR_NOTSUP ;
                }
            } /* end if (stat) */
            rs1 = u_close(fd) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (file) */
	return rs ;
}
/* end subroutine (calmgr_dbmaper) */

static int calmgr_dbmapdestroy(calmgr *op) noex {
	int		rs = SR_OK ;
	if (op->mapdata) {
	    csize	ms = op->mapsize ;
	    void	*md = voidp(op->mapdata) ;
	    rs = u_mmapend(md,ms) ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	}
	return rs ;
}
/* end subroutine (calmgr_dbmapdestroy) */

static int calmgr_idxdir(calmgr *op) noex {
	int		rs ;
	if (char *ibuf ; (rs = malloc_mp(&ibuf)) >= 0) {
	    cchar	*idc = IDXDNAME ;
	    if ((rs = mkpath(ibuf,op->dn,idc)) >= 0) {
	        if (cchar *cp{} ; (rs = uc_mallocstrw(ibuf,rs,&cp)) >= 0) {
	            op->idxdname = cp ;
	        }
	    }
	    rs = rsfree(rs,ibuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (calmgr_idxdir) */

static int calmgr_lookyear(calmgr *op,calmgr_q *qp,cyi **cypp) noex {
	cyi		*yip = nullptr ;
	vechand		*ilp = op->idxp ;
	int		rs ;
	int		i = 0 ; /* used-afterwards */
	void		*vp{} ;
	for (i = 0 ; (rs = ilp->get(i,&vp)) >= 0 ; i += 1) {
	    calmgr_idx	*cip = (calmgr_idx *) vp ;
	    if (vp && (cip->year == qp->y)) {
	        yip = &cip->cy ;
	        break ;
	    }
	} /* end for */
	if (rs == SR_NOTFOUND) {
	    cint	y = qp->y ;
	    if ((rs = calmgr_mkidx(op,y)) >= 0) {
	        cint	idx = rs ;
	        if ((rs = ilp->get(idx,&vp)) >= 0) {
	    	    calmgr_idx	*cip = (calmgr_idx *) vp ;
	            yip = &cip->cy ;
	        }
	    }
	}
	if (cypp != nullptr) {
	    *cypp = (rs >= 0) ? yip : nullptr ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (calmgr_lookyear) */

static int calmgr_lookone(calmgr *op,vecobj *rlp,cyi *cip,calmgr_q *qp) noex {
	cint		cidx = op->cidx ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (cyi_cur ccur ; (rs = cyi_curbegin(cip,&ccur)) >= 0) {
	    if ((rs = cyi_curcite(cip,&ccur,qp)) >= 0) {
	        cint		celen = CEBUFLEN ;
		if (char *cebuf ; (rs = uc_malloc((celen+1),&cebuf)) >= 0) {
		cyi_ent		ce ;
	        calent		e ;
	        uint		loff ;
	        int		llen ;
	        bool		f_ent = false ;
	        bool		f_already = false ;
	        while (rs >= 0) {
	            rs1 = cyi_curread(cip,&ccur,&ce,cebuf,celen) ;
	            if ((rs1 == SR_NOTFOUND) || (rs1 == 0)) break ;
	            rs = rs1 ;
	            if (rs < 0) break ;
	            if (rs1 > 0) {
	                int	n = 0 ;
	                for (int i = 0 ; i < ce.nlines ; i += 1) {
	                    loff = ce.lines[i].loff ;
	                    llen = (int) ce.lines[i].llen ;
	                    n += 1 ;
	                    if (! f_ent) {
	                        uint	lo = loff ;
	                        int	ll = llen ;
	                        if ((rs = calent_start(&e,qp,lo,ll)) >= 0) {
	                            f_ent = true ;
	                            calent_sethash(&e,ce.hash) ;
	                            rs = calent_setidx(&e,cidx) ;
	                        }
	                    } else {
	                        rs = calent_add(&e,loff,llen) ;
	                    }
	                } /* end for */
	                if ((rs >= 0) && (n > 0) && f_ent) {
	                    calyears	*cyp = (calyears *) op->cyp ;
	                    c += 1 ;
#if	CF_ALREADY
	                    rs = calyears_already(cyp,rlp,&e) ;
	                    f_already = (rs > 0) ;
#endif
	                    f_ent = false ;
	                    if ((rs >= 0) && (! f_already)) {
	                        rs = vecobj_add(rlp,&e) ;
	                    } else {
	                        calent_finish(&e) ;
			    }
	                } /* end if */
	            } /* end if (positive) */
	        } /* end while */
	        if (f_ent) {
	            f_ent = false ;
	            calent_finish(&e) ;
	        }
		    rs = rsfree(rs,cebuf) ;
		} /* end if (m-a-f) */
	    } else if (rs == SR_NOTFOUND) {
	        rs = SR_OK ;
	    }
	    rs1 = cyi_curend(cip,&ccur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cyi-cur) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calmgr_lookone) */

static int calmgr_mkidx(calmgr *op,int y) noex {
	cint		csz = szof(calmgr_idx) ;
	int		rs ;
	if (void *vp ; (rs = uc_malloc(csz,&vp)) >= 0) {
	    calmgr_idx *cip = (calmgr_idx *) vp ;
	    if ((rs = calmgr_idxbegin(op,cip,y)) >= 0) {
	        vechand		*ilp = op->idxp ;
	        rs = vechand_add(ilp,cip) ;
	        if (rs < 0) {
	            calmgr_idxend(op,cip) ;
		}
	    } /* end if (calmgr_idxbegin) */
	    if (rs < 0) {
	        uc_free(cip) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (calmgr_mkidx) */

static int calmgr_idxbegin(calmgr *op,calmgr_idx *cip,int y) noex {
	int		rs ;
	cip->year = y ;
	rs = calmgr_cyiopen(op,cip,y) ;
	return rs ;
}
/* end subroutine (calmgr_idxbegin) */

static int calmgr_idxend(calmgr *op,calmgr_idx *cip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = calmgr_cyiclose(op,cip) ;
	    if (rs >= 0) rs = rs1 ;
	}
	cip->year = 0 ;
	return rs ;
}
/* end subroutine (calmgr_idxend) */

static int calmgr_cyiopen(calmgr *op,calmgr_idx *cip,int y) noex {
	cyi		*cyp = &cip->cy ;
	custime		ti_db = op->ti_db ;
	int		rs ;
	cchar		*dn = op->idxdname ;
	cchar		*cn = op->cn ;
	if ((rs = cyi_open(cyp,y,dn,cn)) >= 0) {
	    bool	f_open = true ;
	    if (cyi_info ci ; (rs = cyi_getinfo(cyp,&ci)) >= 0) {
	        if ((ti_db > ci.ctime) || (ti_db > ci.mtime)) {
	            cyi_close(cyp) ;
	            f_open = false ;
	            if ((rs = calmgr_mkcyi(op,y)) >= 0) {
	                if ((rs = cyi_open(cyp,y,dn,cn)) >= 0) {
	                    f_open = true ;
	                }
	            }
	        }
	    } /* end if (cyi_getinfo) */
	    if ((rs < 0) && f_open) {
	        cyi_close(cyp) ;
	    }
	} else if (rs == SR_NOTFOUND) {
	    if ((rs = calmgr_mkcyi(op,y)) >= 0) {
	        rs = cyi_open(cyp,y,dn,cn) ;
	    }
	} /* end if (cyi_open) */
	return rs ;
}
/* end subroutine (calent_cyiopen) */

static int calmgr_cyiclose(calmgr *op,calmgr_idx *cip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    cyi		*cyp = &cip->cy ;
	    {
	        rs1 = cyi_close(cyp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (calmgr_cyiclose) */

static int calmgr_mkcyi(calmgr *op,int y) noex {
	cyimk		cyind ;
	cyimk_ent	bve ;
	int		rs ;
	int		rs1 ;
	int		of = 0 ;
	int		si ;
	int		c = 0 ;
	cmode		om = 0664 ;
	cchar	*dn = op->idxdname ;
	cchar	*cn = op->cn ;
	if ((rs = cyimk_open(&cyind,y,dn,cn,of,om)) >= 0) {
	    calent	e ;
	    calcite	q ;
	    uint	foff = 0 ;
	    cint	cidx = op->cidx ;
	    int		ml = intconv(op->mapsize) ;
	    int		len ;
	    int		ll ;
	    int		f_ent = false ;
	    cchar	*md = op->mapdata ;
	    cchar	*mp = op->mapdata ;
	    cchar	*lp ;
	    bool	f = false ;
	    for (cchar *tp ; (tp = strnchr(mp,ml,'\n')) != nullptr ; ) {
	        len = intconv((tp + 1) - mp) ;
	        lp = mp ;
	        ll = (len - 1) ;
	        if (! isempty(lp,ll)) {
	            calyears	*cyp = (calyears *) op->cyp ;
	            if ((rs = calyears_havestart(cyp,&q,y,lp,ll)) > 0) {
	                si = rs ;
	                if (f_ent) {
	                    c += 1 ;
	                    if ((rs = mkbve_start(&bve,md,&e)) >= 0) {
	                        rs = cyimk_add(&cyind,&bve) ;
	                        mkbve_finish(&bve) ;
	                    }
	                    f_ent = false ;
	                    calent_finish(&e) ;
	                }
	                if (rs >= 0) {
			    uint	eoff = (foff + si) ;
			    int		elen = (ll - si) ;
	                    if ((rs = calent_start(&e,&q,eoff,elen)) >= 0) {
	                        f_ent = true ;
	                        rs = calent_setidx(&e,cidx) ;
	                    }
	                } /* end if (OK) */
	            } else if (rs == 0) { /* continuation */
	                if (f_ent) {
	                    rs = calent_add(&e,foff,ll) ;
	                }
	            } else { /* bad */
	                f = false ;
	                f = f || (rs == SR_NOENT) || (rs == SR_NOTFOUND) ;
	                f = f || (rs == SR_ILSEQ) ;
	                f = f || (rs == SR_INVALID) ;
	                f = f || (rs == SR_NOTSUP) ;
	                if (f && f_ent) {
	                    c += 1 ;
	                    if ((rs = mkbve_start(&bve,md,&e)) >= 0) {
	                        rs = cyimk_add(&cyind,&bve) ;
	                        mkbve_finish(&bve) ;
	                    }
	                    f_ent = false ;
	                    calent_finish(&e) ;
	                }
	            } /* end if (entry start of add) */
	        } else {
#if	CF_EMPTYTERM
	            if (f_ent) {
	                c += 1 ;
	                if ((rs = mkbve_start(&bve,md,&e)) >= 0) {
	                    rs = cyimk_add(&cyind,&bve) ;
	                    mkbve_finish(&bve) ;
	                }
	                f_ent = false ;
	                calent_finish(&e) ;
	            }
#else
	            rs = SR_OK ;
#endif /* CF_EMPTYTERM */
	        } /* end if (not empty) */
	        foff += len ;
	        ml -= len ;
	        mp += len ;
	        if (rs < 0) break ;
	    } /* end while (readling lines) */
	    if ((rs >= 0) && f_ent) {
	        c += 1 ;
	        if ((rs = mkbve_start(&bve,md,&e)) >= 0) {
	            rs = cyimk_add(&cyind,&bve) ;
	            mkbve_finish(&bve) ;
	        }
	        f_ent = false ;
	        calent_finish(&e) ;
	    }
	    if (f_ent) {
	        f_ent = false ;
	        calent_finish(&e) ;
	    }
	    rs1 = cyimk_close(&cyind) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cyimk) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calmgr_mkcyi) */

#if	CF_MKDIRS
static int calmgr_mkdirs(calmgr *op,cchar *dname,mode_t dm) noex {
	int		rs ;
	dm &= S_IAMB ;
	if ((rs = mkdirs(dname,dm)) >= 0) {
	    if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) {
	        if (((sb.st_mode & dm) != dm)) {
	            rs = uc_minmod(dname,dm) ;
	        }
	    }
	} /* end if (mkdirs) */
	return rs ;
}
/* end subroutine (calmgr_mkdirs) */
#endif /* CF_MKDIRS */

static int calmgr_mapdata(calmgr *op,cchar **rpp) noex {
	int		rs = SR_INVALID ;
	if (op->mapdata) {
	    if (rpp) {
	       	*rpp = charp(op->mapdata) ;
	    }
	    rs = int(op->mapsize) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (calmgr_mapdata) */

static int calmgr_idxends(calmgr *op) noex {
	vechand		*ilp = op->idxp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; ilp->get(i,&vp) >= 0 ; i += 1) {
	    calmgr_idx	*cip = (calmgr_idx *) vp ;
	    if (vp) {
		{
	            rs1 = ilp->del(i--) ; /* really optional! */
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = calmgr_idxend(op,cip) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(cip) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (calmgr_idxends) */

static int calmgr_idxaudit(calmgr *op,calmgr_idx *cip) noex {
	int		rs = SR_FAULT ;
	if (op && cip) {
	    cyi		*cyp = &cip->cy ;
	    rs = cyi_audit(cyp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (calmgr_idxaudit) */

static int mkbve_start(cyimk_ent *bvep,cchar *md,calent *ep) noex {
	int		rs = SR_FAULT ;
	int		nlines = 0 ;
	if (bvep && md && ep) {
	    if ((rs = calent_mkhash(ep,md)) >= 0) {
	        bvep->m = ep->q.m ;
	        bvep->d = ep->q.d ;
	        bvep->voff = ep->voff ;
	        bvep->vlen = ep->vlen ;
	        bvep->hash = ep->hash ;
	        bvep->lines = nullptr ;
	        nlines = ep->i ;
	        if (nlines <= UCHAR_MAX) {
	            cyimk_ln	*lines ;
	            cint	lsz = (nlines + 1) * szof(cyimk_ln) ;
	            bvep->nlines = uchar(nlines) ;
	            if ((rs = uc_malloc(lsz,&lines)) >= 0) {
	                int	i ; /* used-afterwards */
	                bvep->lines = lines ;
	                for (i = 0 ; i < nlines ; i += 1) {
	                    lines[i].loff = ep->lines[i].loff ;
	                    lines[i].llen = ep->lines[i].llen ;
	                }
	                lines[i].loff = 0 ;
	                lines[i].llen = 0 ;
	            } /* end if (memory-allocation) */
	        } else {
	            rs = SR_TOOBIG ;
	        }
	    } /* end if (calent_mkhash) */
	} /* end if (non-null) */
	return (rs >= 0) ? nlines : rs ;
}
/* end subroutine (mkbve_start) */

static int mkbve_finish(cyimk_ent *bvep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (bvep) {
	    rs = SR_OK ;
	    if (bvep->lines) {
	        rs1 = uc_free(bvep->lines) ;
	        if (rs >= 0) rs = rs1 ;
	        bvep->lines = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkbve_finish) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	}
    	return rs ;
}
/* end method (vars::operator) */

static bool isempty(cchar *lp,int ll) noex {
	int		f = false ;
	f = f || (ll == 0) ;
	f = f || (lp[0] == '#') ;
	if ((! f) && CHAR_ISWHITE(*lp)) {
	    int		cl ;
	    cchar	*cp ;
	    cl = sfskipwhite(lp,ll,&cp) ;
	    f = ((cl == 0) || (cp[0] == '#')) ;
	}
	return f ;
}
/* end subroutine (isempty) */


