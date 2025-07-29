/* txtindexes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* read or audit a TXTINDEX database */
/* version %I% last-modified %G% */

#define	CF_SORT		1		/* sort primary tags */
#define	CF_DYNACOMPACT	1		/* try dynamic compacting */
#define	CF_SORTKEYS	0		/* sort hash keys (not needed!) */
#define	CF_SORTLISTS	1		/* sort tag lists */

/* revision history:

	= 2008-10-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	txtindexes

	Description:
	This subroutine opens and allows for reading or auditing
	of a TXTINDEX database (which currently consists of two
	files).

	Synopsis:
	int txtindexes_open(txtindexes *op,cchar *dbname) noex

	Arguments:
	- op		object pointer
	- dbname	name of (path-to) DB

	Returns:
	>=0		OK
	<0		error code (system-return)

	Notes:
	Compile-time options include:
	+ sorting primary tags (always recommended)
	+ sorting hash keys (no longer recommended)
	+ sorting tag-list results (recommended)

	Sorting the tag-list results is what really speeds up
	lookups.  Sorting the hash keys (actually reverse sorting
	the hash keys) is a (lame?) attempt at getting similar
	results to sorting tag-list results.  So we may as well
	just sort the tag-list results and get the optimal performance
	for all lookups!  Once sorting of tag-list results is chosen,
	there is no need to sort the hash keys at the input of the
	process since the effect of that is lost after sorting the
	tag-list results anyway.  The original idea of sorting hash
	keys was really a heuristic to try to get the shortest (or
	a shorter) tag-list at the front of the set of tag-lists.
	This is why just sorting tag-lists by the number of tags
	each contains (in ascending order) is probably a better
	idea.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<climits>		/* |UINT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<endian.h>
#include	<vecstr.h>
#include	<storebuf.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<strn.h>
#include	<mkpathxw.h>
#include	<mkfname.h>
#include	<hash.h>
#include	<cfdec.h>
#include	<offindex.h>
#include	<strtabfind.h>
#include	<char.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"txtindexes.h"
#include	"txtindexhdr.h"
#include	"offindex.h"
#include	"naturalwords.h"

import libutil ;

/* local defines */

#ifndef	KEYBUFLEN
#ifdef	NATURALWORDLEN
#define	KEYBUFLEN	NATURALWORDLEN
#else
#define	KEYBUFLEN	80
#endif
#endif

#define	FE_HASH		"hash"
#define	FE_TAG		"tag"

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	TI		txtindexes
#define	TI_CUR		txtindexes_cur
#define	TI_TAG		txtindexes_tag
#define	TI_OBJ		txtindexes_obj
#define	TI_INFO		txtindexes_info
#define	TI_FI		txtindexes_fi
#define	TI_MI		txtindexes_mi

#ifndef	CF_SORT
#define	CF_SORT		1		/* sort primary tags */
#endif
#ifndef	CF_DYNACOMPACT
#define	CF_DYNACOMPACT	1		/* try dynamic compacting */
#endif
#ifndef	CF_SORTKEYS
#define	CF_SORTKEYS	0		/* sort hash keys (not needed!) */
#endif
#ifndef	CF_SORTLISTS
#define	CF_SORTLISTS	1		/* sort tag lists */
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

typedef int (*strtab_t)[3] ;		/* for |strtabfind(3uc)| */


/* external variables */


/* exported variables */

TI_OBJ	txtindexes_modinfo = {
	"txtindexes",
	szof(txtindexes),
	szof(txtindexes_cur)
} ;


/* local structures */

struct listdesc {
	uint		*listp ;
	uint		ntags ;
} ;


/* forward references */

template<typename ... Args>
static int txtindexes_ctor(txtindexes *op,Args ... args) noex {
	TXTINDEXES	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
		rs = SR_OK ;
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (txtindexes_ctor) */

static int txtindexes_dtor(txtindexes *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (txtindexes_dtor) */

template<typename ... Args>
static inline int txtindexes_magic(txtindexes *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TXTINDEXES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (txtindexes_magic) */

static int	txtindexes_dbloadcreate(txtindexes *,time_t) noex ;
static int	txtindexes_dbloaddestroy(txtindexes *) noex ;
static int	txtindexes_fimapcreate(txtindexes *,int,cchar *,time_t) noex ;
static int	txtindexes_fimapdestroy(txtindexes *,int) noex ;
static int	txtindexes_dbmapcreate(txtindexes *,time_t) noex ;
static int	txtindexes_dbmapdestroy(txtindexes *) noex ;
static int	txtindexes_dbproc(txtindexes *,time_t) noex ;
static int	txtindexes_mkhashkeys(txtindexes *,vecstr *,mainv) noex ;
static int	txtindexes_mktaglist(txtindexes *,uint **,vecstr *) noex ;
static int	txtindexes_oureigen(txtindexes *,cchar *,int) noex ;
static int	txtindexes_hdrverify(txtindexes *,time_t) noex ;
static int	txtindexes_audithash(txtindexes *,offindex *) noex ;
static int	txtindexes_auditeigen(txtindexes *) noex ;

static int	offindex_tags(offindex *,cchar *,int) noex ;

static int	tag_parse(TI_TAG *,char *,int,cchar *,int) noex ;

#if	CF_DYNACOMPACT
#else
static int	taglist_compact(uint *,int) noex ;
#endif

static int	vcmpuint(cvoid *,cvoid *) noex ;

#if	CF_SORTLISTS
static int	cmplistdesc(const listdesc *,const listdesc *) noex ;
#endif /* CF_SORTLISTS */

#if	CF_SORTKEYS
static int	vcmpkey(cchar **,cchar **) noex ;
#endif


/* local variables */

constexpr bool		f_sort		= CF_SORT ;
constexpr bool		f_dynacompact	= CF_DYNACOMPACT ;
constexpr bool		f_sortkeys	= CF_SORTKEYS ;
constexpr bool		f_sortlists	= CF_SORTLISTS ;


/* exported variables */


/* exported subroutines */

int txtindexes_open(txtindexes *op,cchar *dbname) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = txtindexes_ctor(op,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
	        cchar	*cp ;
	        if ((rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
	            op->dbname = cp ;
	            if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
	                if ((rs = txtindexes_dbloadcreate(op,dt)) >= 0) {
	                    op->ti_lastcheck = dt ;
	                    op->magic = TXTINDEXES_MAGIC ;
	                }
	                if (rs < 0) {
	                    ptm_destroy(op->mxp) ;
			}
	             } /* end if (ptm_create) */
	             if (rs < 0) {
	                 uc_free(op->dbname) ;
	                 op->dbname = nullptr ;
	             }
	         } /* end if (m-a) */
	    } /* end if (valid) */
	    if (rs < 0) {
		txtindexes_dtor(op) ;
	    }
	} /* end if (txtindexes_ctor) */
	return rs ;
}
/* end subroutine (txtindexes_open) */

int txtindexes_close(txtindexes *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = txtindexes_magic(op)) >= 0) {
	    {
	        rs1 = txtindexes_dbloaddestroy(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mxp) {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbname) {
	        rs1 = uc_free(op->dbname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    }
	    {
		rs1 = txtindexes_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexes_close) */

int txtindexes_audit(txtindexes *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = txtindexes_magic(op)) >= 0) {
	    offindex	oi ;
	    int		n = op->ifi.taglen ;
	    if ((rs = offindex_start(&oi,n)) >= 0) {
	        cint	sl = int(op->tf.mapsize) ;
	        cchar	*sp = op->tf.mapdata ;
	        if ((rs = offindex_tags(&oi,sp,sl)) >= 0) {
	            const uint	taglen = uint(rs) ;
	            if (taglen == op->ifi.taglen) {
	                rs = txtindexes_audithash(op,&oi) ;
	            } else {
	                rs = SR_BADFMT ;
	            }
	        }
	        rs1 = offindex_finish(&oi) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (offindex) */
	    if (rs >= 0) {
	        rs = txtindexes_auditeigen(op) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexes_audit) */

int txtindexes_count(txtindexes *op) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = txtindexes_magic(op)) >= 0) {
	    n = op->ifi.taglen ;
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (txtindexes_count) */

int txtindexes_neigen(txtindexes *op) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = txtindexes_magic(op)) >= 0) {
	    n = (op->ifi.erlen - 1) ;
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (txtindexes_neigen) */

int txtindexes_getinfo(txtindexes *op,TI_INFO *ip) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = txtindexes_magic(op)) >= 0) {
	    n = op->ifi.taglen ;
	    memclear(ip) ;
	    if (ip != nullptr) {
	        TI_FI	*fip = &op->hf ;
	        ip->ticreat = time_t(op->ifi.wtime) ;
	        ip->timod = fip->ti_mod ;
	        ip->count = n ;
	        ip->neigen = (op->ifi.erlen - 1) ;
	        ip->minwlen = op->ifi.minwlen ;
	        ip->maxwlen = op->ifi.maxwlen ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (txtindexes_getinfo) */

int txtindexes_getsdn(txtindexes *op,char *rb,int rl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = txtindexes_magic(op,rb)) >= 0) {
	    TI_FI	*fip = &op->hf ;
	    if (cc *sp = (fip->mapdata + op->ifi.sdnoff) ; sp[0] != '\0') {
	        rs = sncpy(rb,rl,sp) ;
	        len = rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (txtindexes_getsdn) */

int txtindexes_getsfn(txtindexes *op,char *rb,int rl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = txtindexes_magic(op,rb)) >= 0) {
	    TI_FI	*fip = &op->hf ;
	    if (cc *sp = (fip->mapdata + op->ifi.sfnoff) ; sp[0] != '\0') {
	        rs = sncpy(rb,rl,sp) ;
		len = rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (txtindexes_getsfn) */

int txtindexes_iseigen(txtindexes *op,cchar *kp,int kl) noex {
	int		rs ;
	if ((rs = txtindexes_magic(op,kp)) >= 0) {
	    int		klen = KEYBUFLEN ;
	    char	kbuf[KEYBUFLEN + 1] ;
	    if (kl < 0) kl = lenstr(kp) ;
	    if (kl > klen) kl = klen ;
	    if (hasuc(kp,kl)) {
	        strwcpylc(kbuf,kp,kl) ;
	        kp = kbuf ;
	    }
	    rs = txtindexes_oureigen(op,kp,kl) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexes_iseigen) */

int txtindexes_curbegin(txtindexes *op,TI_CUR *curp) noex {
	int		rs ;
	if ((rs = txtindexes_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    if ((rs = ptm_lockbegin(op->mxp)) >= 0) {
	        op->ncursors += 1 ;
	        ptm_lockend(op->mxp) ;
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexes_curbegin) */

int txtindexes_curend(txtindexes *op,TI_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = txtindexes_magic(op,curp)) >= 0) {
	    if (curp->taglist) {
	        rs1 = uc_free(curp->taglist) ;
	        if (rs >= 0) rs = rs1 ;
	        curp->taglist = nullptr ;
	    }
	    curp->taglen = 0 ;
	    if ((rs1 = ptm_lockbegin(op->mxp)) >= 0) {
	        if (op->ncursors > 0) {
	            op->ncursors -= 1 ;
	        }
	        ptm_lockend(op->mxp) ;
	    } /* end if */
	    if (rs >= 0) rs = rs1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexes_curend) */

int txtindexes_curlook(txtindexes *op,TI_CUR *curp,mainv klp) noex {
	int		rs ;
	int		rs1 ;
	int		taglen = 0 ;
	if ((rs = txtindexes_magic(op,curp,klp)) >= 0) {
	    cint	vn = 10 ;
	    cint	vo = 0 ;
	    uint	*taglist = nullptr ;
	    curp->taglen = 0 ;
	    if (curp->taglist != nullptr) {
	        uc_free(curp->taglist) ;
	        curp->taglist = nullptr ;
	    }
	    /* condition the keys for the lookup */
	    if (vecstr hkeys ; (rs = hkeys.start(vn,vo)) >= 0) {
	        if ((rs = txtindexes_mkhashkeys(op,&hkeys,klp)) >= 0) {
	            if ((rs = txtindexes_mktaglist(op,&taglist,&hkeys)) >= 0) {
	                taglen = rs ;
	                if (taglist != nullptr) {
	                    curp->taglist = taglist ;
	                    curp->taglen = taglen ;
	                    curp->i = 0 ;
	                }
	            } /* end if (txtindexes_mktaglist) */
	        } /* end if (txtindexes_mkhashkeys) */
	        rs1 = hkeys.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (hkeys) */
	} /* end if (magic) */
	return (rs >= 0) ? taglen : rs ;
}
/* end subroutine (txtindexes_curlook) */

/* returns length of the filename (if any) in the returned tag (if any) */
int txtindexes_curenum(txtindexes *op,TI_CUR *curp,TI_TAG *tagp,
		char *rbuf,int rlen) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = txtindexes_magic(op,curp,tagp,rbuf)) >= 0) {
	    TI_FI	*fip = &op->tf ;
	    cchar	*tagbuf ;
	    uint	tagoff ;
	    if ((curp->taglist == nullptr) || (curp->taglen == 0)) {
	        rs = SR_NOTFOUND ;
	    }
	    if ((rs >= 0) && (curp->taglen == 0)) {
	        rs = SR_NOTFOUND ;
	    }
	    if (rs >= 0) {
	        uint	idx = curp->i ;
	        if ((idx >= op->ifi.taglen) || (idx >= curp->taglen)) {
	            rs = SR_NOTFOUND ;
	        }
	        if (rs >= 0) {
		    const uint	um = UINT_MAX ;
	            while ((idx < curp->taglen) && (curp->taglist[idx] == um)) {
	                idx += 1 ;
	            }
	            if (idx < curp->taglen) {
	                tagoff = curp->taglist[idx] ;
	                if (tagoff < fip->mapsize) {
	                    tagbuf = (fip->mapdata + tagoff) ;
	                    rs = tag_parse(tagp,rbuf,rlen,tagbuf,-1) ;
	                    len = rs ;
	                } else {
	                    rs = SR_BADFMT ;
		        }
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } /* end if */
	        if (rs >= 0) {
	            curp->i = (idx + 1) ;
	        }
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (txtindexes_curenum) */


/* private subroutines */

static int txtindexes_dbloadcreate(txtindexes *op,time_t dt) noex {
	int		rs ;
	if ((rs = txtindexes_dbmapcreate(op,dt)) >= 0) {
	    rs = txtindexes_dbproc(op,dt) ;
	    if (rs < 0) {
	        txtindexes_dbmapdestroy(op) ;
	    }
	}
	return rs ;
}
/* end subroutine (txtindexes_dbloadcreate) */

static int txtindexes_dbloaddestroy(txtindexes *op) noex {
	TI_MI		*mip = &op->mi ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = txtindexes_dbmapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	mip->table = nullptr ;
	mip->estab = nullptr ;
	mip->ertab = nullptr ;
	mip->eitab = nullptr ;
	return rs ;
}
/* end subroutine (txtindexes_dbloaddestroy) */

static int txtindexes_dbmapcreate(txtindexes *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkfnamesuf2(tbuf,op->dbname,FE_HASH,ENDIANSTR)) >= 0) {
	        if ((rs = txtindexes_fimapcreate(op,0,tbuf,dt)) >= 0) {
	            if ((rs = mkfnamesuf1(tbuf,op->dbname,FE_TAG)) >= 0) {
	                rs = txtindexes_fimapcreate(op,1,tbuf,dt) ;
	            }
	            if (rs < 0) {
	                txtindexes_fimapdestroy(op,0) ;
		    }
	        } /* end if (txtindexes_fimapcreate) */
	    } /* end if (mkfnamesuf) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (txtindexes_dbmapcreate) */

static int txtindexes_dbmapdestroy(txtindexes *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = txtindexes_fimapdestroy(op,1) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = txtindexes_fimapdestroy(op,0) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (txtindexes_dbmapdestroy) */

static int txtindexes_fimapcreate(txtindexes *op,int w,cc *fn,time_t dt) noex {
	TI_FI		*fip = (w) ? &op->tf : &op->hf ;
	int		rs ;
	int		rs1 ;
	{
	    fip->mapdata = nullptr ;
	    fip->mapsize = 0 ;
	    fip->ti_mod = 0 ;
	    fip->ti_map = 0 ;
	}
	if ((rs = u_open(fn,O_RDONLY,0666)) >= 0) {
	    cnullptr	np{} ;
	    USTAT	sb ;
	    cint	fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	        size_t	ms = int(sb.st_size & UINT_MAX) ;
	        int	mp = PROT_READ ;
	        int	mf = MAP_SHARED ;
	        void	*md ;
	        if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	            fip->mapdata = charp(md) ;
	            fip->mapsize = ms ;
	            fip->ti_mod = sb.st_mtime ;
	            fip->ti_map = dt ;
	        }
	    } /* end if (fstat) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open-file) */
	return rs ;
}
/* end subroutine (txtindexes_fimapcreate) */

static int txtindexes_fimapdestroy(txtindexes *op,int w) noex {
	TI_FI		*fip = (w) ? &op->tf : &op->hf ;
	int		rs = SR_OK ;
	if (fip->mapdata != nullptr) {
	    rs = u_mmapend(fip->mapdata,fip->mapsize) ;
	    fip->mapdata = nullptr ;
	    fip->mapsize = 0 ;
	    fip->ti_map = 0 ;
	}
	return rs ;
}
/* end subroutine (txtindexes_fimapdestroy) */

static int txtindexes_dbproc(txtindexes *op,time_t dt) noex {
	TI_FI		*fip = &op->hf ;
	txtindexhdr	*hip = &op->ifi ;
	int		rs ;
	int		c = 0 ; /* return-value */
	cint	fsz = intsat(fip->mapsize) ;
	if ((rs = txtindexhdr_rd(hip,fip->mapdata,fsz)) >= 0) {
	    if ((rs = txtindexes_hdrverify(op,dt)) >= 0) {
		TI_MI		*mip = &op->mi ;
	        mip->sdn = (char *) (fip->mapdata + hip->sdnoff) ;
	        mip->sfn = (char *) (fip->mapdata + hip->sfnoff) ;
	        mip->lists = (uint *) (fip->mapdata + hip->listoff) ;
	        mip->estab = (cchar *) (fip->mapdata + hip->esoff) ;
	        mip->ertab = (uint *) (fip->mapdata + hip->eroff) ;
	        mip->eitab = (uint (*)[3]) (fip->mapdata + hip->eioff) ;
	        mip->table = (uint *) (fip->mapdata + hip->taboff) ;
	        c = hip->taglen ;
	    } /* end if (txtindexes_hdrverify) */
	} /* endif (txtindexhdr_rd) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (txtindexes_dbproc) */

static int txtindexes_mkhashkeys(txtindexes *op,vecstr *clp,mainv klp) noex {
	cint		klen = KEYBUFLEN ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		minwlen ;
	int		maxwlen ;
	int		i ;
	int		kl ;
	int		c = 0 ;
	cchar		*kp ;
	char		keybuf[KEYBUFLEN + 1] ;

	minwlen = op->ifi.minwlen ;
	maxwlen = op->ifi.maxwlen ;

	for (i = 0 ; (kp = klp[i]) != nullptr ; i += 1) {
	    if (kp != nullptr) {

	        kl = lenstr(kp,klen) ;	/* also prevents overflow */
	        if (kl >= minwlen) {

	            if (hasuc(kp,kl)) {
	                strwcpylc(keybuf,kp,kl) ;	/* cannot overflow */
	                kp = keybuf ;
	            }

	    	    if ((rs = txtindexes_oureigen(op,kp,kl)) == 0) {
			if (kl > maxwlen) kl = maxwlen ;
	                if ((rs = vecstr_findn(clp,kp,kl)) == rsn) {
	                    c += 1 ;
	                    rs = vecstr_add(clp,kp,kl) ;
	                }
	            } /* end if */

		} /* end if (go) */

	    }
	    if (rs < 0) break ;
	} /* end for */

#if	CF_SORTKEYS
	if ((rs >= 0) && (c > 1))
	    vecstr_sort(clp,vcmpkey) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (txtindexes_mkhashkeys) */

static int txtindexes_mktaglist(txtindexes *op,uint **tlpp,vecstr *hlp) noex {
	txtindexhdr	*hip = &op->ifi ;
	int		rs ;
	int		tagcount = 0 ;
	/* allocate an array to hold tag-list results */
	if ((rs = vecstr_count(hlp)) > 0) {
	    listdesc	*lists = nullptr ;
	    int		n = rs ;
	    int		sz ;

	    sz = n * szof(listdesc) ;
	    if ((rs = uc_malloc(sz,&lists)) >= 0) {
	        uint	hv ;
	        uint	*table = op->mi.table ;
	        uint	*taglist = nullptr ;
	        uint	*uip ;
	        int	maxtags = hip->maxtags ;
	        uint	tablen = hip->tablen ;
	        uint	listoff ;
	        int	hi ;
	        int	taglen ;
	        int	ntags ;
	        int	k ;
		int	c = 0 ;
	        cchar	*kp ;

/* fill in the tag-list array with results */

	        for (int i = 0 ; vecstr_get(hlp,i,&kp) >= 0 ; i += 1) {
	            if (kp != nullptr) {

	                hv = hash_elf(kp,-1) ;

	                hi = (hv % tablen) ;
	                listoff = table[hi] ;

	                if (listoff != 0) {

/* sanity checks */

	                    if ((listoff & 3) != 0) {
	                        rs = SR_BADFMT ;
	                        break ;
	                    }

	                    if ((listoff < hip->listoff) || 
				(listoff >= op->hf.mapsize)) {
	                        rs = SR_BADFMT ;
	                        break ;
	                    }

/* continue */

	                    uip = (uint *) (op->hf.mapdata + listoff) ;
	                    ntags = *uip++ ;

	                    if (ntags > 0) {

/* sanity check (optionally requested by index creator!) */

	                        if ((maxtags > 0) && (ntags > maxtags)) {
	                            rs = SR_BADFMT ;
	                            break ;
	                        }

	                        lists[c].listp = uip ;
	                        lists[c].ntags = ntags ;
	                        c += 1 ;

	                    } else {
	                        c = 0 ;
	                        break ;
	                    } /* end if (non-zero number of tags) */

	                } else {
	                    c = 0 ;
	                    break ;
	                }

	            }
	            if (rs < 0) break ;
	        } /* end for */
	        n = c ;

/* sort the list-descriptors by number of tag entries */

#if	CF_SORTLISTS /* optional but strongly recommended, for performance */
	        if ((rs >= 0) && (n > 1)) {
	            int	(*cfn)(cvoid *,cvoid *) ;
	            cfn = (int (*)(cvoid *,cvoid *)) cmplistdesc ;
	            qsort(lists,n,szof(listdesc),cfn) ;
	        }
#endif /* CF_SORTLISTS */

/* perform the join operation on the tag lists (not the easiest thing to do) */

	        taglen = 0 ;
	        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {

	            uip = lists[i].listp ;
	            ntags = lists[i].ntags ;

	            if (i == 0) {

	                tagcount = ntags ;
	                taglen = ntags ;
	                sz = (taglen + 1) * szof(uint) ;
	                if ((rs = uc_malloc(sz,&taglist)) >= 0) {
	                    memcpy(taglist,uip,sz) ;
	                }

	            } else {
			int	j ;

#if	CF_DYNACOMPACT /* try dynamic compaction? (nerves of steel!) */

	                j = 0 ;
	                while (j < taglen) {

	                    for (k = 0 ; k < ntags ; k += 1) {
	                        if (taglist[j] == uip[k]) break ;
	                    } /* end for */

	                    if (k >= ntags) {
	                        taglen -= 1 ;
	                        if (taglen == 0) break ;
	                        if (j < taglen) {
				    taglist[j] = taglist[taglen] ;
				}
	                    } else {
	                        j += 1 ;
	                    }

	                } /* end while */
	                tagcount = taglen ;

#else /* CF_DYNACOMPACT */

	                for (j = 0 ; j < taglen ; j += 1) {
	                    if (taglist[j] != UINT_MAX) {

	                        for (k = 0 ; k < ntags ; k += 1) {
	                            if (taglist[j] == uip[k]) break ;
	                        } /* end for */

	                        if (k >= ntags) {
	                            taglist[j] = UINT_MAX ;
	                            tagcount -= 1 ;
	                            if (tagcount == 0) break ;
	                        }

	                    } /* end if */
	                } /* end for */

#endif /* CF_DYNACOMPACT */

	                if (tagcount == 0) break ;
	            } /* end if */

	        } /* end for (looping through hash keys) */

/* finishing */

	        if ((rs >= 0) && (taglist != nullptr)) {

#if	CF_DYNACOMPACT
#else
	            if ((tagcount > 0) && (taglen != tagcount)) {
	                rs = taglist_compact(taglist,taglen) ;
	            }
#endif /* CF_DYNACOMPACT */

	            taglist[tagcount] = UINT_MAX ;

/* sort the tags in this resulting tag-list */

#if	CF_SORT
	            if ((rs >= 0) && (tagcount > 1)) {
	                qsort(taglist,tagcount,sizeof(uint),vcmpuint) ;
	            }
#endif

	        } /* end if (finishing) */

/* done */

	        if ((rs < 0) || (tlpp == nullptr)) {
	            if (taglist != nullptr) {
	                uc_free(taglist) ;
	                taglist = nullptr ;
	            }
	        } else {
	            *tlpp = taglist ;
	        }

	        if (lists != nullptr) {
	            uc_free(lists) ;
	            lists = nullptr ;
	        }

	    } /* end if (m-a) */
	} /* end if (positive) */

	return (rs >= 0) ? tagcount : rs ;
}
/* end subroutine (txtindexes_mktaglist) */

int txtindexes_oureigen(txtindexes *op,cchar *kp,int kl) noex {
	TI_MI		*mip = &op->mi ;
	strtab_t	eitab = strtab_t(mip->eitab) ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	uint		eilen = op->ifi.eilen ;
	uint		nskip = op->ifi.eiskip ;
	int		f = true ;
	cchar		*estab = mip->estab ;
	if ((rs = strtabfind(estab,eitab,eilen,nskip,kp,kl)) == rsn) {
	    f = false ;
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (txtindexes_oureigen) */

static int txtindexes_hdrverify(txtindexes *op,time_t dt) noex {
	TI_FI		*fip = &op->hf ;
	txtindexhdr	*hip = &op->ifi ;
	uint		utime = uint(dt) ;
	int		rs = SR_OK ;
	uint		hfsize ;
	uint		tfsize ;
	uint		tabsize ;
	bool		f = true ;
	hfsize = hip->hfsize ;
	tfsize = hip->tfsize ;
	tabsize = (hip->tablen * szof(uint)) ;
	f = f && (hfsize == fip->mapsize) ;
	f = f && (tfsize == op->tf.mapsize) ;
	f = f && (hip->wtime > 0) && (hip->wtime <= (utime + SHIFTINT)) ;
	f = f && (hip->sdnoff <= fip->mapsize) ;
	f = f && (hip->sfnoff <= fip->mapsize) ;
/* alignment restriction */
	f = f && ((hip->listoff & (szof(int)-1)) == 0) ;
/* size restrictions */
	f = f && (hip->listoff <= fip->mapsize) ;
/* alignment restriction */
	f = f && ((hip->taboff & (szof(int)-1)) == 0) ;
/* size restrictions */
	f = f && (hip->taboff <= fip->mapsize) ;
	f = f && (tabsize <= fip->mapsize) ;
	f = f && ((hip->taboff + tabsize) <= fip->mapsize) ;
	if (f) {
	    uint	essize = hip->essize ;
	    uint	erlen = hip->erlen ;
	    uint	eilen = hip->eilen ;
/* alignment restrictions */
	    f = f && ((hip->esoff & (szof(int)-1)) == 0) ;
	    f = f && ((hip->eroff & (szof(int)-1)) == 0) ;
	    f = f && ((hip->eioff & (szof(int)-1)) == 0) ;
/* size restrictions */
	    f = f && ((hip->esoff + essize) <= hfsize) ;
	    f = f && ((hip->eroff + (erlen * szof(int))) <= hfsize) ;
	    f = f && ((hip->eioff + (eilen * 3 * szof(int))) <= hfsize) ;
	} /* end if */
	if (! f) {
	    rs = SR_BADFMT ;
	}
	return rs ;
}
/* end subroutine (txtindexes_hdrverify) */

static int txtindexes_audithash(txtindexes *op,offindex *oip) noex {
	TI_FI		*fip = &op->hf ;
	TI_MI		*mip = &op->mi ;
	uint		listoff ;
	uint		tagoff ;
	uint		hfsize ;
	uint		tfsize ;
	uint		listsize ;
	txtindexhdr	*hip ;
	int		rs = SR_OK ;
	hip = &op->ifi ;
	/* loop over table entries */
	hfsize = intsat(fip->mapsize) ;
	tfsize = intsat(op->tf.mapsize) ;
	cint	n = int(hip->tablen) ;
	for (int i = 0 ; i < n ; i += 1) {
	    uint	*uip ;
	    uint	ntags ;
	    listoff = mip->table[i] ;
	    if (listoff == 0) continue ;
	    if (listoff >= hfsize) {
	        rs = SR_BADFMT ;
	        break ;

	    } /* end if (error) */
	    if ((listoff & 3) != 0) {
	        rs = SR_BADFMT ;
	        break ;
	    }
	    uip = (uint *) (fip->mapdata + listoff) ;
	    ntags = *uip++ ;
	    if (ntags > 0) {
	        if (ntags > hip->taglen) {
	            rs = SR_BADFMT ;
	            break ;
	        } /* end if (error) */
	        listsize = (ntags + 1) * szof(uint) ;
	        if ((listsize + listoff) >= hfsize) {
	            rs = SR_BADFMT ;
	            break ;
	        } /* end if (error) */
	        for (int j = 0 ; j < int(ntags) ; j += 1) {
	            tagoff = *uip++ ;
	            if (tagoff >= tfsize) {
	                rs = SR_BADFMT ;
	                break ;
	            } /* end if (error) */
	            rs = offindex_lookup(oip,tagoff) ;
	            if (rs < 0) break ;
	        } /* end for (tag-list entries) */
	    } /* end if (had some tags) */
	    if (rs < 0) break ;
	} /* end for (hash-table entries) */
	return rs ;
}
/* end subroutine (txtindexes_audithash) */

static int txtindexes_auditeigen(txtindexes *op) noex {
	txtindexhdr	*hip = &op->ifi ;
	TI_MI		*mip = &op->mi ;
	int		rs = SR_OK ;
	int		nhi ;
	int		cl ;
	int		nskip ;
	uint		essize ;
	int		erlen ;
	int		eilen ;
	uint		*ertab ;
	uint		(*eitab)[3] ;
	cchar		*estab ;
	cchar		*cp ;
	{
	    essize = hip->essize ;
	    erlen = int(hip->erlen) ;
	    eilen = int(hip->eilen) ;
	}
	{
	    estab = mip->estab ;
	    eitab = mip->eitab ;
	    ertab = mip->ertab ;
	}
/* some record-table checkes */
	if ((rs >= 0) && (ertab[0] != 0)) {
	    rs = SR_BADFMT ;
	}
	if ((rs >= 0) && (estab[0] != '\0')) {
	    rs = SR_BADFMT ;
	}
	if (rs >= 0) {
	    nskip = hip->eiskip ;
	    for (int i = 1 ; i < erlen ; i += 1) {
	        uint	si = ertab[i] ;
	        if ((si <= 0) || (si > essize)) {
	            rs = SR_BADFMT ;
	            break ;
	        }
	        cp = (estab + si) ;
	        cl = lenstr(cp,(NATURALWORDLEN + 1)) ;
	        if (cl >= NATURALWORDLEN) {
	            rs = SR_BADFMT ;
	            break ;
	        }
	        if (cp[-1] == '\0') {
		    strtab_t	stab = strtab_t(eitab) ;
	            rs = strtabfind(estab,stab,eilen,nskip,cp,cl) ;
	        } else {
	            rs = SR_BADFMT ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (ok) */
/* some index-table checks */
	if ((rs >= 0) && (eitab[0][0] != 0)) {
	    rs = SR_BADFMT ;
	}
	if (rs >= 0) {
	    for (int i = 1 ; i < eilen ; i += 1) {
	        uint	si = eitab[i][0] ;
	        if (si > essize) {
	            rs = SR_BADFMT ;
	            break ;
	        }
	        nhi = eitab[i][2] ;
	        if (nhi > eilen) {
	            rs = SR_BADFMT ;
	            break ;
	        }
	    } /* end for */
	} /* end if (ok) */
/* while we are here, we may as well go all out! (inverted str-tab check) */
	if (rs >= 0) {
	    cchar	*ecp = (estab + essize) ;
	    cp = (estab + 1) ;
	    while ((cp < ecp) && cp[0]) {
	        uint	si = intconv(cp - estab) ;
	        int	j{} ; /* used-afterwards */
	        cl = lenstr(cp) ;
	        for (j = 0 ; j < erlen ; j += 1) {
	            if (si == ertab[j])
	                break ;
	        }
	        if (j >= erlen) {
	            rs = SR_BADFMT ;
	        }
	        if (rs >= 0) {
		    strtab_t	stab = strtab_t(eitab) ;
	            rs = strtabfind(estab,stab,eilen,nskip,cp,cl) ;
	        }
	        cp = (cp + cl + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (txtindexes_auditeigen) */

/* index the beginning-of-line offsets in the TAG file */
static int offindex_tags(offindex *oip,cchar *fp,int fl) noex {
	off_t		lineoff = 0 ;
	int		rs = SR_OK ;
	int		ll ;
	int		n = 0 ;
	cchar		*tp ;
	while ((tp = strnchr(fp,fl,'\n')) != nullptr) {
	    cint	len = intconv((tp + 1) - fp) ;
	    ll = (len - 1) ;
	    if (ll > 0) {
	        n += 1 ;
	        rs = offindex_add(oip,lineoff,ll) ;
	        if (rs < 0) break ;
	    } /* end if */
	    lineoff += len ;
	    fp += len ;
	    fl -= len ;
	} /* end while */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (offindex_tags) */

static int tag_parse(TI_TAG *tagp,char *rbuf,int rlen,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	cchar		*tp ; /* used-multiple */
	rbuf[0] = '\0' ;
	if (sl < 0) {
	    if ((tp = strchr(sp,'\n')) != 0) {
	        sl = intconv(tp - sp) ;
	    } else {
	        rs = SR_BADFMT ;
	    }
	}
	if (rs >= 0) {
	    tagp->recoff = 0 ;
	    tagp->reclen = 0 ;
	    if ((tp = strnchr(sp,sl,':')) != nullptr) {
		{
		    cint tl = intconv(tp - sp) ;
	            rs = snwcpy(rbuf,rlen,sp,tl) ;
	            len = rs ;
		}
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	    } /* end if */
	    if (rs >= 0) {
	        if ((tp = strnchr(sp,sl,',')) != nullptr) {
		    cint	vl = intconv(tp - sp) ;
		    if (uint uv ; (rs = cfdecui(sp,vl,&uv)) >= 0) {
			tagp->recoff = uv ;
	                sl -= intconv((tp + 1) - sp) ;
	                sp = (tp + 1) ;
	                if ((tp = strnbrk(sp,sl,"\t ")) != nullptr) {
	                    sl = intconv(tp - sp) ;
	                }
	                tagp->reclen = INT_MAX ;
	                if (sl >= 0) {
			    if (uint v{} ; (rs = cfdecui(sp,sl,&v)) >= 0) {
			        tagp->reclen = v ;
			    }
	                }
	            } /* end if (ok) */
	        } else {
	            rs = SR_BADFMT ;
	 	}
	    } /* end if (ok) */
	} /* end if (ok) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (tag_parse) */

#if	CF_DYNACOMPACT
#else
static int taglist_compact(uint *taglist,int taglen) noex {
	while (taglist[taglen - 1] == UINT_MAX) {
	    taglen -= 1 ;
	}
	for (int i = 0 ; (i < (taglen - 1)) ; i += 1) {
	    if (taglist[i] == UINT_MAX) {
	        int	j = (taglen - 1) ;
	        if ((i < j) && (taglist[j] != UINT_MAX)) {
	            taglist[i] = taglist[j] ;
	            taglen -= 1 ;
	            while (taglist[taglen - 1] == UINT_MAX) {
	                taglen -= 1 ;
	            }
	        }
	    } /* end if */
	} /* end for (compacting tag list) */
	taglist[taglen] = UINT_MAX ;
	return taglen ;
}
/* end subroutine (taglist_compact) */
#endif /* CF_DYNACOMPACT */

static int vcmpuint(cvoid *v1p,cvoid *v2p) noex {
	const uint	*i1p = (const uint *) v1p ;
	const uint	*i2p = (const uint *) v2p ;
	int		rc = 0 ;
	if (i1p) {
	    if (i2p) {
	        rc = (*i1p - *i2p) ;
	    } else {
	        rc = -1 ;
	    }
	} else {
	    rc = +1 ;
	}
	return rc ;
}
/* end subroutine (vcmpuint) */

#if	CF_SORTKEYS

/* reverse sort strings by 1. string length and 2. string value */
static int vcmpkey(cchar **s1pp,cchar **s2pp) noex {
	cchar		*s1 = *s1pp ;
	cchar		*s2 = *s2pp ;
	int		rc = 0 ;
	if (s1 || s2) {
	    if (s1) {
	        if (s2) {
	            cint	l1 = lenstr(s1) ;
	            cint 	l2 = lenstr(s2) ;
	            if ((rc = (l2 - l1)) == 0) {
	                rc = strcmp(s2,s1) ;
		    }
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = +1 ;
	    }
	}
	return rc ;
}
/* end subroutine (vcmpkey) */

#endif /* CF_SORTKEYS */

#if	CF_SORTLISTS
static int cmplistdesc(const listdesc *l1p,const listdesc *l2p) noex {
	return (l1p->ntags - l2p->ntags) ;
}
/* end subroutine (cmplistdesc) */
#endif /* CF_SORTLISTS */


