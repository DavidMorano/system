/* calmgr SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<ucmallreg.h>
#include	<getbufsize.h>
#include	<getusername.h>
#include	<estrings.h>
#include	<vecobj.h>
#include	<sbuf.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<tmtime.h>
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


/* local defines */

#define	CALMGR_DBSUF	"calendar"
#define	CALMGR_MAXLINES	40		/* maximum lines per entry */
#define	CALMGR_DMODE	0777

#define	IDXDNAME	".calyears"

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#define	CEBUFLEN	((CALMGR_MAXLINES*COLUMNS) + (3*szof(int)))

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct calmgr_idx {
	cyi		cy ;
	int		year ;
	int		f_open:1 ;
} ;


/* forward references */

static int	calmgr_argbegin(calmgr *,cchar *,cchar *) noex ;
static int	calmgr_argend(calmgr *) noex ;
static int	calmgr_dbloadbegin(calmgr *,time_t) noex ;
static int	calmgr_dbloadend(calmgr *) noex ;
static int	calmgr_dbmapcreate(calmgr *,time_t) noex ;
static int	calmgr_dbmapdestroy(calmgr *) noex ;
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

static int	mkbve_start(CYIMK_ENT *,cchar *,calent *) noex ;
static int	mkbve_finish(CYIMK_ENT *) noex ;

static bool	isempty(cchar *,int) noex ;


/* exported variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int calmgr_start(calmgr *calp,calyears *op,int cidx,cchar *dn,cchar *cn) noex {
	custime		dt = getustime ;
	int		rs ;

	memclear(calp) ;
	calp->cyp = op ; /* parent object */
	calp->cidx = cidx ; /* parent index */

	if ((rs = calmgr_argbegin(calp,dn,cn)) >= 0) {
	    if ((rs = calmgr_dbloadbegin(calp,dt)) >= 0) {
	        if ((rs = calmgr_idxdir(calp)) >= 0) {
	            cint	vo = VECHAND_OSTATIONARY ;
	            if ((rs = vechand_start(&calp->idxes,1,vo)) >= 0) {
	                calp->f.idxes = true ;
	            }
	        } /* end if (calmgr_idxdir) */
	        if (rs < 0) {
	            calmgr_dbloadend(calp) ;
		}
	    } /* end if (calmgr_dbloadbegin) */
	    if (rs < 0) {
	        calmgr_argend(calp) ;
	    }
	} /* end if (calmgr_argbegin) */

	return rs ;
}
/* end subroutine (calmgr_start) */

int calmgr_finish(calmgr *calp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (calp->f.idxes) {
	    {
	        rs1 = calmgr_idxends(calp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        calp->f.idxes = false ;
	        rs1 = vechand_finish(&calp->idxes) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}
	{
	    rs1 = calmgr_dbloadend(calp) ;
	    if (rs >= 0) rs = rs1 ;
	}	
	if (calp->idxdname != nullptr) {
	    rs1 = uc_free(calp->idxdname) ;
	    if (rs >= 0) rs = rs1 ;
	    calp->idxdname = nullptr ;
	}
	{
	    rs1 = calmgr_argend(calp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (calmgr_finish) */

int calmgr_lookup(calmgr *calp,vecobj *rlp,calmgr_q *qp) noex {
	cyi		*cyp ;
	int		rs ;
	if ((rs = calmgr_lookyear(calp,qp,&cyp)) >= 0) {
	    rs = calmgr_lookone(calp,rlp,cyp,qp) ;
	}
	return rs ;
}
/* end subroutine (calmgr_lookup) */

int calmgr_getci(calmgr *calp) noex {
	int		cidx = calp->cidx ;
	return cidx ;
}
/* end subroutine (calmgr_getci) */

int calmgr_getbase(calmgr *calp,cchar **rpp) noex {
	int		rs = SR_OK ;
	cchar		*md ;
	if (calp->mapdata != nullptr) {
	     md = calp->mapdata ;
	     rs = calp->mapsize ;
	} else {
	     rs = SR_INVALID ;
	}
	*rpp = (rs >= 0) ? md : nullptr ;
	return rs ;
}
/* end subroutine (calmgr_getbase) */

int calmgr_gethash(calmgr *calp,calent *ep,uint *rp) noex {
	int		rs ;
	if ((rs = calent_gethash(ep,rp)) == 0) {
	    cchar	*md = calp->mapdata ;
	    if ((rs = calent_mkhash(ep,md)) >= 0) {
		rs = calent_gethash(ep,rp) ;
	    }
	}
	return rs ;
}
/* end subroutine (calmgr_gethash) */

int calmgr_loadbuf(calmgr *calp,char *rbuf,int rlen,calent *ep) noex {
	int		rs ;
	if (cchar *md{} ; (rs = calmgr_mapdata(calp,&md)) >= 0) {
	    rs = calent_loadbuf(ep,rbuf,rlen,md) ;
	}
	return rs ;
}
/* end subroutine (calmgr_loadbuf) */

int calmgr_audit(calmgr *calp) noex {
	vechand		*ilp = &calp->idxes ;
	int		rs = SR_OK ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(ilp,i,&vp) >= 0 ; i += 1) {
	    calmgr_idx	*cip = (calmgr_idx *) vp ;
	    if (vp) {
	        rs = calmgr_idxaudit(calp,cip) ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (calmgr_audit) */


/* private subroutines */

static int calmgr_argbegin(calmgr *calp,cchar *dn,cchar *cn) noex {
	int		rs ;
	int		size = 0 ;
	size += (strlen(dn)+1) ;
	size += (strlen(cn)+1) ;
	if (char *bp{} ; (rs = uc_malloc(size,&bp)) >= 0) {
	    calp->a = bp ;
	    calp->dn = bp ;
	    bp = (strwcpy(bp,dn,-1)+1) ;
	    calp->cn = bp ;
	    bp = (strwcpy(bp,cn,-1)+1) ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (calmgr_argbegin) */

static int calmgr_argend(calmgr *calp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (calp->a != nullptr) {
	    rs1 = uc_free(calp->a) ;
	    if (rs >= 0) rs = rs1 ;
	    calp->a = nullptr ;
	}
	return rs ;
}
/* end subroutine (calmgr_argend) */

static int calmgr_dbloadbegin(calmgr *calp,time_t dt) noex {
	int		rs ;
	{
	    rs = calmgr_dbmapcreate(calp,dt) ;
	}
	return rs ;
}
/* end subroutine (calmgr_dbloadbegin) */

static int calmgr_dbloadend(calmgr *calp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = calmgr_dbmapdestroy(calp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (calmgr_dbloadend) */

static int calmgr_dbmapcreate(calmgr *calp,time_t dt) noex {
	cint		nlen = MAXNAMELEN ;
	int		rs ;
	int		rs1 ;
	cchar		*suf = CALMGR_DBSUF ;
	char		nbuf[MAXNAMELEN + 1] ;

	if ((rs = snsds(nbuf,nlen,calp->cn,suf)) >= 0) {
	    char	dbfname[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(dbfname,calp->dn,nbuf)) >= 0) {
	        if ((rs = u_open(dbfname,O_RDONLY,0666)) >= 0) {
	            cint	fd = rs ;
	            if (USTAT sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	                if (S_ISREG(sb.st_mode)) {
			    csize	im = size_t(INT_MAX) ;
	                    calp->fsize = size_t(sb.st_size) ;
	                    calp->ti_db = sb.st_mtime ;
	                    if (calp->fsize <= im) {
	                        csize	ms = calp->fsize ;
	                        cint	mp = PROT_READ ;
	                        cint	mf = MAP_SHARED ;
	                        void	*n = nullptr ;
	                        void	*md{} ;
	                        if ((rs = u_mmap(n,ms,mp,mf,fd,0L,&md)) >= 0) {
	                            if (dt == 0) dt = getustime ;
	                            calp->mapdata = charp(md) ;
	                            calp->mapsize = calp->fsize ;
	                            calp->ti_map = dt ;
	                            calp->ti_lastcheck = dt ;
	                        } /* end if (u_mmap) */
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
	    } /* end if (mkpath) */
	} /* end if (snsds) */

	return rs ;
}
/* end subroutine (calmgr_dbmapcreate) */

static int calmgr_dbmapdestroy(calmgr *calp) noex {
	int		rs = SR_OK ;
	if (calp->mapdata) {
	    csize	ms = calp->mapsize ;
	    void	*md = voidp(calp->mapdata) ;
	    rs = u_munmap(md,ms) ;
	    calp->mapdata = nullptr ;
	    calp->mapsize = 0 ;
	}
	return rs ;
}
/* end subroutine (calmgr_dbmapdestroy) */

static int calmgr_idxdir(calmgr *calp) noex {
	int		rs ;
	cchar		*idc = IDXDNAME ;
	char		idxdname[MAXPATHLEN+1] ;
	if ((rs = mkpath2(idxdname,calp->dn,idc)) >= 0) {
	    if (cchar *cp{} ; (rs = uc_mallocstrw(idxdname,rs,&cp)) >= 0) {
	        calp->idxdname = cp ;
	    }
	}
	return rs ;
}
/* end subroutine (calmgr_idxdir) */

static int calmgr_lookyear(calmgr *calp,calmgr_q *qp,cyi **cypp) noex {
	cyi		*yip = nullptr ;
	vechand		*clp = &calp->idxes ;
	int		rs ;
	int		i = 0 ; /* used-afterwards */
	void		*vp{} ;
	for (i = 0 ; (rs = vechand_get(clp,i,&vp)) >= 0 ; i += 1) {
	    calmgr_idx	*cip = (calmgr_idx *) vp ;
	    if (vp && (cip->year == qp->y)) {
	        yip = &cip->cy ;
	        break ;
	    }
	} /* end for */
	if (rs == SR_NOTFOUND) {
	    cint	y = qp->y ;
	    if ((rs = calmgr_mkidx(calp,y)) >= 0) {
	        cint	idx = rs ;
	        if ((rs = vechand_get(clp,idx,&vp)) >= 0) {
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

static int calmgr_lookone(calmgr *calp,vecobj *rlp,cyi *cip,calmgr_q *qp) noex {
	cyi_cur		ccur ;
	cyi_ent		ce ;
	cint		cidx = calp->cidx ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if ((rs = cyi_curbegin(cip,&ccur)) >= 0) {
	    if ((rs = cyi_curcite(cip,&ccur,qp)) >= 0) {
	        calent		e ;
	        uint		loff ;
	        cint		celen = CEBUFLEN ;
	        int		llen ;
	        int		f_ent = false ;
	        int		f_already = false ;
	        char		cebuf[CEBUFLEN + 1] ;

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
	                    calyears	*op = (calyears *) calp->cyp ;
	                    c += 1 ;

#if	CF_ALREADY
	                    rs = calyears_already(op,rlp,&e) ;
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

	    } else if (rs == SR_NOTFOUND) {
	        rs = SR_OK ;
	    }
	    cyi_curend(cip,&ccur) ;
	} /* end if (cyi-cur) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calmgr_lookone) */

static int calmgr_mkidx(calmgr *calp,int y) noex {
	cint		csize = szof(calmgr_idx) ;
	int		rs ;
	if (calmgr_idx *cip{} ; (rs = uc_malloc(csize,&cip)) >= 0) {
	    if ((rs = calmgr_idxbegin(calp,cip,y)) >= 0) {
	        vechand		*ilp = &calp->idxes ;
	        rs = vechand_add(ilp,cip) ;
	        if (rs < 0) {
	            calmgr_idxend(calp,cip) ;
		}
	    } /* end if (calmgr_idxbegin) */
	    if (rs < 0) {
	        uc_free(cip) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (calmgr_mkidx) */

static int calmgr_idxbegin(calmgr *calp,calmgr_idx *cip,int y) noex {
	int		rs ;
	cip->year = y ;
	rs = calmgr_cyiopen(calp,cip,y) ;
	return rs ;
}
/* end subroutine (calmgr_idxbegin) */

static int calmgr_idxend(calmgr *calp,calmgr_idx *cip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = calmgr_cyiclose(calp,cip) ;
	    if (rs >= 0) rs = rs1 ;
	}
	cip->year = 0 ;
	return rs ;
}
/* end subroutine (calmgr_idxend) */

static int calmgr_cyiopen(calmgr *calp,calmgr_idx *cip,int y) noex {
	cyi		*cyp = &cip->cy ;
	time_t		ti_db = calp->ti_db ;
	int		rs ;
	cchar		*dn = calp->idxdname ;
	cchar		*cn = calp->cn ;

	if ((rs = cyi_open(cyp,y,dn,cn)) >= 0) {
	    bool	f_open = true ;
	    if (cyi_info ci ; (rs = cyi_getinfo(cyp,&ci)) >= 0) {
	        if ((ti_db > ci.ctime) || (ti_db > ci.mtime)) {
	            cyi_close(cyp) ;
	            f_open = false ;
	            if ((rs = calmgr_mkcyi(calp,y)) >= 0) {
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
	    if ((rs = calmgr_mkcyi(calp,y)) >= 0) {
	        rs = cyi_open(cyp,y,dn,cn) ;
	    }
	} /* end if (cyi_open) */

	return rs ;
}
/* end subroutine (calent_cyiopen) */

static int calmgr_cyiclose(calmgr *calp,calmgr_idx *cip) noex {
	cyi		*cyp = &cip->cy ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (calp == nullptr) return SR_FAULT ;
	{
	    rs1 = cyi_close(cyp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (calmgr_cyiclose) */

static int calmgr_mkcyi(calmgr *calp,int y) noex {
	CYIMK		cyind ;
	CYIMK_ENT	bve ;
	int		rs ;
	int		rs1 ;
	int		of = 0 ;
	int		si ;
	int		c = 0 ;
	int		f ;
	cmode		om = 0664 ;
	cchar	*dn = calp->idxdname ;
	cchar	*cn = calp->cn ;

	if ((rs = cyimk_open(&cyind,y,dn,cn,of,om)) >= 0) {
	    calent	e ;
	    CALCITE	q ;
	    uint	foff = 0 ;
	    cint	cidx = calp->cidx ;
	    int		ml = calp->mapsize ;
	    int		len ;
	    int		ll ;
	    int		f_ent = false ;
	    cchar	*md = calp->mapdata ;
	    cchar	*mp = calp->mapdata ;
	    cchar	*lp ;
	    cchar	*tp ;

	    while ((tp = strnchr(mp,ml,'\n')) != nullptr) {

	        len = ((tp + 1) - mp) ;
	        lp = mp ;
	        ll = (len - 1) ;

	        if (! isempty(lp,ll)) {
	            calyears	*op = (calyears *) calp->cyp ;

	            if ((rs = calyears_havestart(op,&q,y,lp,ll)) > 0) {
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
	                }

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
static int calmgr_mkdirs(calmgr *calp,cchar *dname,mode_t dm) noex {
	int		rs ;
	dm &= S_IAMB ;
	if ((rs = mkdirs(dname,dm)) >= 0) {
	    struct ustat	sb ;
	    if ((rs = u_stat(dname,&sb)) >= 0) {
	        if (((sb.st_mode & dm) != dm)) {
	            rs = uc_minmod(dname,dm) ;
	        }
	    }
	} /* end if (mkdirs) */
	return rs ;
}
/* end subroutine (calmgr_mkdirs) */
#endif /* CF_MKDIRS */

static int calmgr_mapdata(calmgr *calp,cchar **rpp) noex {
	int		rs ;
	if (calp->mapdata != nullptr) {
	    if (rpp) {
	       	*rpp = (cchar *) calp->mapdata ;
	    }
	    rs = (int) calp->mapsize ;
	} else {
	    rs = SR_INVALID ;
	}
	return rs ;
}
/* end subroutine (calmgr_mapdata) */

static int calmgr_idxends(calmgr *calp) noex {
	vechand		*ilp = &calp->idxes ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(ilp,i,&vp) >= 0 ; i += 1) {
	    calmgr_idx	*cip = (calmgr_idx *) vp ;
	    if (vp) {
		{
	        rs1 = vechand_del(ilp,i--) ; /* really optional! */
	        if (rs >= 0) rs = rs1 ;
		}
		{
	        rs1 = calmgr_idxend(calp,cip) ;
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

static int calmgr_idxaudit(calmgr *calp,calmgr_idx *cip) noex {
	cyi		*cyp = &cip->cy ;
	int		rs ;
	if (calp == nullptr) return SR_FAULT ;
	rs = cyi_audit(cyp) ;
	return rs ;
}
/* end subroutine (calmgr_idxaudit) */

static int mkbve_start(CYIMK_ENT *bvep,cchar *md,calent *ep) noex {
	int		rs ;
	int		nlines = 0 ;

	if (ep == nullptr) return SR_FAULT ;

	if ((rs = calent_mkhash(ep,md)) >= 0) {
	    bvep->m = ep->q.m ;
	    bvep->d = ep->q.d ;
	    bvep->voff = ep->voff ;
	    bvep->vlen = ep->vlen ;
	    bvep->hash = ep->hash ;
	    bvep->lines = nullptr ;
	    nlines = ep->i ;
	    if (nlines <= UCHAR_MAX) {
	        CYIMK_LINE	*lines ;
	        cint		size = (nlines + 1) * szof(CYIMK_LINE) ;
	        bvep->nlines = nlines ;
	        if ((rs = uc_malloc(size,&lines)) >= 0) {
	            int		i ; /* used-afterwards */
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

	return (rs >= 0) ? nlines : rs ;
}
/* end subroutine (mkbve_start) */

static int mkbve_finish(CYIMK_ENT *bvep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (bvep == nullptr) return SR_FAULT ;

	if (bvep->lines != nullptr) {
	    rs1 = uc_free(bvep->lines) ;
	    if (rs >= 0) rs = rs1 ;
	    bvep->lines = nullptr ;
	}

	return rs ;
}
/* end subroutine (mkbve_finish) */

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


