/* txtindexes */
/* lang=C20 */

/* read or audit a TXTINDEX database */
/* version %I% last-modified %G% */

#define	CF_SORT		1		/* sort primary tags */
#define	CF_DYNACOMPACT	1		/* try dynamic compacting */
#define	CF_SORTKEYS	0		/* sort hash keys (not needed!) */
#define	CF_SORTLISTS	1		/* sort tag lists */

/* revision history:

	= 2008-10-01, David A­D­ Morano
	This subroutine was originally written.

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
	int txtindexes_open(txtindexes *op,cchar *cchardbname) noex

	Arguments:
	- op		object pointer
	- dbname	name of (path-to) DB

	Returns:
	>=0		OK
	<0		error code

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
#include	<limits.h>
#include	<unistd.h>
#include	<time.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<endian.h>
#include	<vecstr.h>
#include	<storebuf.h>
#include	<char.h>
#include	<localmisc.h>
#include	<hash.h>

#include	"txtindexes.h"
#include	"txtindexhdr.h"
#include	"offindex.h"
#include	"naturalwords.h"


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

#define	LISTDESC	struct listdesc


/* external subroutines */

extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	mkfnamesuf2(char *,cchar *,cchar *,cchar *) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	nleadstr(cchar *,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	strtabfind(cchar *,int (*)[3],int,int,cchar *,int) ;
extern int	hasuc(cchar *,int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpylc(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;


/* external variables */


/* exported variables */

TXTINDEXES_OBJ	txtindexes = {
	"txtindexes",
	    sizeof(TXTINDEXES),
	    sizeof(TXTINDEXES_CUR)
} ;


/* local structures */

struct listdesc {
	uint		*listp ;
	uint		ntags ;
} ;


/* forward references */

static int	txtindexes_dbloadcreate(TXTINDEXES *,time_t) ;
static int	txtindexes_dbloaddestroy(TXTINDEXES *) ;
static int	txtindexes_filemapcreate(TXTINDEXES *,int,cchar *,time_t) ;
static int	txtindexes_filemapdestroy(TXTINDEXES *,int) ;
static int	txtindexes_dbmapcreate(TXTINDEXES *,time_t) ;
static int	txtindexes_dbmapdestroy(TXTINDEXES *) ;
static int	txtindexes_dbproc(TXTINDEXES *,time_t) ;
static int	txtindexes_mkhashkeys(TXTINDEXES *,vecstr *,cchar **) ;
static int	txtindexes_mktaglist(TXTINDEXES *,uint **,vecstr *) ;
static int	txtindexes_oureigen(TXTINDEXES *,cchar *,int) ;
static int	txtindexes_hdrverify(TXTINDEXES *,time_t) ;
static int	txtindexes_audithash(TXTINDEXES *,OFFINDEX *) ;
static int	txtindexes_auditeigen(TXTINDEXES *) ;

static int	offindex_tags(OFFINDEX *,cchar *,int) ;

static int	tag_parse(TXTINDEXES_TAG *,cchar *,int) ;

#if	CF_DYNACOMPACT
#else
static int	taglist_compact(uint *,int) ;
#endif

static int	vcmpuint(const void *,const void *) ;

#if	CF_SORTLISTS
static int	cmplistdesc(const LISTDESC *,const LISTDESC *) ;
#endif /* CF_SORTLISTS */

#if	CF_SORTKEYS
static int	vcmpkey(cchar **,cchar **) ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int txtindexes_open(TXTINDEXES *op,cchar *dbname) noex {
	custime		dt = time(NULL) ;
	int		rs = SR_OK ;
	cchar		*cp ;

	if (op == NULL) return SR_FAULT ;
	if (dbname == NULL) return SR_FAULT ;

	if (dbname[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(TXTINDEXES)) ;

	if ((rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
	    op->dbname = cp ;
	    if ((rs = ptm_create(&op->m,NULL)) >= 0) {
	        if ((rs = txtindexes_dbloadcreate(op,dt)) >= 0) {
	            op->ti_lastcheck = dt ;
	            op->magic = TXTINDEXES_MAGIC ;
	        }
	        if (rs < 0)
	            ptm_destroy(&op->m) ;
	    }
	    if (rs < 0) {
	        uc_free(op->dbname) ;
	        op->dbname = NULL ;
	    }
	} /* end if (m-a) */

	return rs ;
}
/* end subroutine (txtindexes_open) */


int txtindexes_close(TXTINDEXES *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

	rs1 = txtindexes_dbloaddestroy(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = ptm_destroy(&op->m) ;
	if (rs >= 0) rs = rs1 ;

	if (op->dbname != NULL) {
	    rs1 = uc_free(op->dbname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbname = NULL ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (txtindexes_close) */


int txtindexes_audit(TXTINDEXES *op)
{
	OFFINDEX	oi ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		n ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

/* line-index the TAG file */

	n = op->ifi.taglen ;
	if ((rs = offindex_start(&oi,n)) >= 0) {
	    cint	sl = op->tf.mapsize ;
	    cchar	*sp = op->tf.mapdata ;
	    if ((rs = offindex_tags(&oi,sp,sl)) >= 0) {
	        const int	taglen = rs ;
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

	return rs ;
}
/* end subroutine (txtindexes_audit) */


int txtindexes_count(TXTINDEXES *op)
{
	int		rs = SR_OK ;
	int		n = 0 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

	n = op->ifi.taglen ;

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (txtindexes_count) */


int txtindexes_neigen(TXTINDEXES *op)
{
	int		rs = SR_OK ;
	int		n = 0 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

	n = (op->ifi.erlen - 1) ;

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (txtindexes_neigen) */

int txtindexes_getinfo(TXTINDEXES *op,TXTINDEXES_INFO *ip) noex {
	int		rs = SR_OK ;
	int		n = 0 ;
	cchar	*sp ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

	n = op->ifi.taglen ;
	if (ip != NULL) {
	    TXTINDEXES_FI	*fip = &op->hf ;
	    const int		plen = MAXPATHLEN ;
	    memset(ip,0,sizeof(TXTINDEXES_INFO)) ;
	    ip->ctime = (time_t) op->ifi.wtime ;
	    ip->mtime = fip->ti_mod ;
	    ip->count = n ;
	    ip->neigen = (op->ifi.erlen - 1) ;
	    ip->minwlen = op->ifi.minwlen ;
	    ip->maxwlen = op->ifi.maxwlen ;
	    sp = (fip->mapdata + op->ifi.sdnoff) ;
	    if (sp[0] != '\0') {
	        strwcpy(ip->sdn,sp,plen) ;
	    }
	    sp = (fip->mapdata + op->ifi.sfnoff) ;
	    if (sp[0] != '\0') {
	        strwcpy(ip->sfn,sp,plen) ;
	    }
	} /* end if */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (txtindexes_info) */


int txtindexes_iseigen(TXTINDEXES *op,cchar *kp,int kl)
{
	int		rs ;
	int		klen = KEYBUFLEN ;
	char		keybuf[KEYBUFLEN + 1] ;

	if (op == NULL) return SR_FAULT ;
	if (kp == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

	if (kl < 0) kl = strlen(kp) ;
	if (kl > klen) kl = klen ;

	if (hasuc(kp,kl)) {
	    strwcpylc(keybuf,kp,kl) ;
	    kp = keybuf ;
	}

	rs = txtindexes_oureigen(op,kp,kl) ;

	return rs ;
}
/* end subroutine (txtindexes_iseigen) */


/* amazingly the only thread-shared data is the 'ncursors' variable here! */
int txtindexes_curbegin(TXTINDEXES *op,TXTINDEXES_CUR *curp)
{
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

	memset(curp,0,sizeof(TXTINDEXES_CUR)) ;

	if ((rs = ptm_lock(&op->m)) >= 0) {
	    op->ncursors += 1 ;
	    ptm_unlock(&op->m) ;
	} /* end if */

	return rs ;
}
/* end subroutine (txtindexes_curbegin) */


int txtindexes_curend(TXTINDEXES *op,TXTINDEXES_CUR *curp)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

	if (curp->taglist != NULL) {
	    rs1 = uc_free(curp->taglist) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->taglist = NULL ;
	}

	curp->taglen = 0 ;

	if ((rs1 = ptm_lock(&op->m)) >= 0) {
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	    ptm_unlock(&op->m) ;
	} /* end if */
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (txtindexes_curend) */


int txtindexes_lookup(TXTINDEXES *op,TXTINDEXES_CUR *curp,cchar **klp)
{
	vecstr		hkeys ;
	uint		*taglist = NULL ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		taglen = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;
	if (klp == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

	curp->taglen = 0 ;
	if (curp->taglist != NULL) {
	    uc_free(curp->taglist) ;
	    curp->taglist = NULL ;
	}

/* condition the keys for the lookup */

	if ((rs = vecstr_start(&hkeys,10,0)) >= 0) {

	    if ((rs = txtindexes_mkhashkeys(op,&hkeys,klp)) >= 0) {
	        if ((rs = txtindexes_mktaglist(op,&taglist,&hkeys)) >= 0) {
	            taglen = rs ;
	            if (taglist != NULL) {
	                curp->taglist = taglist ;
	                curp->taglen = taglen ;
	                curp->i = 0 ;
	            }
	        } /* end if (txtindexes_mktaglist) */
	    } /* end if (txtindexes_mkhashkeys) */

	    rs1 = vecstr_finish(&hkeys) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (hkeys) */

	return (rs >= 0) ? taglen : rs ;
}
/* end subroutine (txtindexes_lookup) */


/* returns length of the filename (if any) in the returned tag (if any) */
int txtindexes_read(TXTINDEXES *op,TXTINDEXES_CUR *curp,TXTINDEXES_TAG *tagp)
{
	TXTINDEXES_FI	*fip ;
	uint		tagoff ;
	int		rs = SR_OK ;
	int		i ;
	int		len = 0 ;
	cchar	*tagbuf ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;
	if (tagp == NULL) return SR_FAULT ;

	if (op->magic != TXTINDEXES_MAGIC) return SR_NOTOPEN ;

	fip = &op->tf ;
	if ((curp->taglist == NULL) || (curp->taglen == 0)) {
	    rs = SR_NOTFOUND ;
	}

	if ((rs >= 0) && (curp->taglen == 0)) {
	    rs = SR_NOTFOUND ;
	}

	if (rs >= 0) {

	    i = curp->i ;
	    if (i < 0) {
	        rs = SR_BADFMT ;
	    } else if ((i >= op->ifi.taglen) || (i >= curp->taglen)) {
	        rs = SR_NOTFOUND ;
	    }
	    if (rs >= 0) {

	        while ((i < curp->taglen) && (curp->taglist[i] == UINT_MAX)) {
	            i += 1 ;
	        }

	        if (i < curp->taglen) {
	            tagoff = curp->taglist[i] ;
	            if (tagoff < fip->mapsize) {
	                tagbuf = (fip->mapdata + tagoff) ;
	                rs = tag_parse(tagp,tagbuf,-1) ;
	                len = rs ;
	            } else {
	                rs = SR_BADFMT ;
		    }

	        } else {
	            rs = SR_NOTFOUND ;
	        }

	    } /* end if */

	    if (rs >= 0) {
	        curp->i = (i + 1) ;
	    }

	} /* end if (ok) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (txtindexes_read) */


/* private subroutines */

static int txtindexes_dbloadcreate(TXTINDEXES *op,time_t dt) noex {
	int		rs ;

	if ((rs = txtindexes_dbmapcreate(op,dt)) >= 0) {
	    rs = txtindexes_dbproc(op,dt) ;
	    if (rs < 0)
	        txtindexes_dbmapdestroy(op) ;
	}

	return rs ;
}
/* end subroutine (txtindexes_dbloadcreate) */

static int txtindexes_dbloaddestroy(TXTINDEXES *op) noex {
	TXTINDEXES_MI	*mip ;
	int		rs = SR_OK ;
	int		rs1 ;

	rs1 = txtindexes_dbmapdestroy(op) ;
	if (rs >= 0) rs = rs1 ;

	mip = &op->mi ;
	mip->table = NULL ;
	mip->estab = NULL ;
	mip->ertab = NULL ;
	mip->eitab = NULL ;
	return rs ;
}
/* end subroutine (txtindexes_dbloaddestroy) */


static int txtindexes_dbmapcreate(TXTINDEXES *op,time_t dt)
{
	int		rs ;
	char		tbuf[MAXPATHLEN + 1] ;

	if ((rs = mkfnamesuf2(tbuf,op->dbname,FE_HASH,ENDIANSTR)) >= 0) {
	    if ((rs = txtindexes_filemapcreate(op,0,tbuf,dt)) >= 0) {
	        if ((rs = mkfnamesuf1(tbuf,op->dbname,FE_TAG)) >= 0) {
	            rs = txtindexes_filemapcreate(op,1,tbuf,dt) ;
	        }
	        if (rs < 0)
	            txtindexes_filemapdestroy(op,0) ;
	    } /* end if (txtindexes_filemapcreate) */
	}

	return rs ;
}
/* end subroutine (txtindexes_dbmapcreate) */


static int txtindexes_dbmapdestroy(TXTINDEXES *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	rs1 = txtindexes_filemapdestroy(op,1) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = txtindexes_filemapdestroy(op,0) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (txtindexes_dbmapdestroy) */


static int txtindexes_filemapcreate(TXTINDEXES *op,int w,cchar *fn,time_t dt)
{
	TXTINDEXES_FI	*fip = (w) ? &op->tf : &op->hf ;
	int		rs ;

	{
	    fip->mapdata = NULL ;
	    fip->mapsize = 0 ;
	    fip->ti_mod = 0 ;
	    fip->ti_map = 0 ;
	}

	if ((rs = u_open(fn,O_RDONLY,0666)) >= 0) {
	    USTAT	sb ;
	    int		fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	        size_t	ms = (sb.st_size & UINT_MAX) ;
	        int	mp = PROT_READ ;
	        int	mf = MAP_SHARED ;
	        void	*md ;
	        if ((rs = u_mmap(NULL,ms,mp,mf,fd,0L,&md)) >= 0) {
	            fip->mapdata = md ;
	            fip->mapsize = ms ;
	            fip->ti_mod = sb.st_mtime ;
	            fip->ti_map = dt ;
	        }
	    } /* end if (fstat) */
	    u_close(fd) ;
	} /* end if (open-file) */

	return rs ;
}
/* end subroutine (txtindexes_filemapcreate) */


static int txtindexes_filemapdestroy(TXTINDEXES *op,int w)
{
	TXTINDEXES_FI	*fip ;
	int		rs = SR_OK ;

	fip = (w) ? &op->tf : &op->hf ;

	if (fip->mapdata != NULL) {
	    rs = u_munmap(fip->mapdata,fip->mapsize) ;
	    fip->mapdata = NULL ;
	    fip->mapsize = 0 ;
	    fip->ti_map = 0 ;
	}

	return rs ;
}
/* end subroutine (txtindexes_filemapdestroy) */


static int txtindexes_dbproc(TXTINDEXES *op,time_t dt)
{
	TXTINDEXES_FI	*fip = &op->hf ;
	TXTINDEXES_MI	*mip = &op->mi ;
	TXTINDEXHDR	*hip ;
	int		rs ;
	int		c = 0 ;

	hip = &op->ifi ;
	if ((rs = txtindexhdr(hip,1,fip->mapdata,fip->mapsize)) >= 0) {
	    if ((rs = txtindexes_hdrverify(op,dt)) >= 0) {
	        mip->sdn = (char *) (fip->mapdata + hip->sdnoff) ;
	        mip->sfn = (char *) (fip->mapdata + hip->sfnoff) ;
	        mip->lists = (uint *) (fip->mapdata + hip->listoff) ;
	        mip->estab = (cchar *) (fip->mapdata + hip->esoff) ;
	        mip->ertab = (int *) (fip->mapdata + hip->eroff) ;
	        mip->eitab = (int (*)[3]) (fip->mapdata + hip->eioff) ;
	        mip->table = (uint *) (fip->mapdata + hip->taboff) ;
	        c = hip->taglen ;
	    }
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (txtindexes_dbproc) */


static int txtindexes_mkhashkeys(TXTINDEXES *op,vecstr *clp,cchar **klp)
{
	const int	klen = KEYBUFLEN ;
	const int	nrs = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		minwlen ;
	int		maxwlen ;
	int		i ;
	int		kl ;
	int		c = 0 ;
	cchar	*kp ;
	char		keybuf[KEYBUFLEN + 1] ;

	minwlen = op->ifi.minwlen ;
	maxwlen = op->ifi.maxwlen ;

	for (i = 0 ; (kp = klp[i]) != NULL ; i += 1) {
	    if (kp != NULL) {

	        kl = strnlen(kp,klen) ;	/* also prevents overflow */
	        if (kl >= minwlen) {

	            if (hasuc(kp,kl)) {
	                strwcpylc(keybuf,kp,kl) ;	/* cannot overflow */
	                kp = keybuf ;
	            }

	    	    if ((rs = txtindexes_oureigen(op,kp,kl)) == 0) {
			if (kl > maxwlen) kl = maxwlen ;
	                if ((rs = vecstr_findn(clp,kp,kl)) == nrs) {
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


static int txtindexes_mktaglist(TXTINDEXES *op,uint **tlpp,vecstr *hlp)
{
	TXTINDEXHDR	*hip = &op->ifi ;
	int		rs ;
	int		tagcount = 0 ;

/* allocate an array to hold tag-list results */

	if ((rs = vecstr_count(hlp)) > 0) {
	    LISTDESC	*lists = NULL ;
	    int		n = rs ;
	    int		size ;

	    size = n * sizeof(LISTDESC) ;
	    if ((rs = uc_malloc(size,&lists)) >= 0) {
	        uint	*table = op->mi.table ;
	        uint	*taglist = NULL ;
	        uint	maxtags = hip->maxtags ;
	        uint	hv ;
	        uint	listoff ;
	        uint	*uip ;
	        int	tablen = hip->tablen ;
	        int	hi ;
	        int	taglen ;
	        int	ntags ;
	        cchar	*kp ;
	        int	k ;
		int	i ;
		int	c = 0 ;

/* fill in the tag-list array with results */

	        for (i = 0 ; vecstr_get(hlp,i,&kp) >= 0 ; i += 1) {
	            if (kp != NULL) {

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
	            int	(*cfn)(const void *,const void *) ;
	            cfn = (int (*)(const void *,const void *)) cmplistdesc ;
	            qsort(lists,n,sizeof(LISTDESC),cfn) ;
	        }
#endif /* CF_SORTLISTS */

/* perform the join operation on the tag lists (not the easiest thing to do) */

	        taglen = 0 ;
	        for (i = 0 ; (rs >= 0) && (i < n) ; i += 1) {

	            uip = lists[i].listp ;
	            ntags = lists[i].ntags ;

	            if (i == 0) {

	                tagcount = ntags ;
	                taglen = ntags ;
	                size = (taglen + 1) * sizeof(uint) ;
	                if ((rs = uc_malloc(size,&taglist)) >= 0) {
	                    memcpy(taglist,uip,size) ;
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

	        if ((rs >= 0) && (taglist != NULL)) {

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

	        if ((rs < 0) || (tlpp == NULL)) {
	            if (taglist != NULL) {
	                uc_free(taglist) ;
	                taglist = NULL ;
	            }
	        } else {
	            *tlpp = taglist ;
	        }

	        if (lists != NULL) {
	            uc_free(lists) ;
	            lists = NULL ;
	        }

	    } /* end if (m-a) */
	} /* end if (positive) */

	return (rs >= 0) ? tagcount : rs ;
}
/* end subroutine (txtindexes_mktaglist) */


int txtindexes_oureigen(TXTINDEXES *op,cchar *kp,int kl)
{
	TXTINDEXES_MI	*mip = &op->mi ;
	const int	nrs = SR_NOTFOUND ;
	int		rs ;
	int		(*eitab)[3] ;
	int		eilen ;
	int		nskip ;
	int		f = TRUE ;
	cchar	*estab ;

	estab = mip->estab ;
	eitab = mip->eitab ;
	eilen = op->ifi.eilen ;
	nskip = op->ifi.eiskip ;
	if ((rs = strtabfind(estab,eitab,eilen,nskip,kp,kl)) == nrs) {
	    f = FALSE ;
	    rs = SR_OK ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (txtindexes_oureigen) */


static int txtindexes_hdrverify(TXTINDEXES *op,time_t dt)
{
	TXTINDEXES_FI	*fip = &op->hf ;
	TXTINDEXHDR	*hip = &op->ifi ;
	uint		utime = (uint) dt ;
	int		rs = SR_OK ;
	int		hfsize, tfsize ;
	int		tabsize ;
	int		f = TRUE ;

	hfsize = hip->hfsize ;
	tfsize = hip->tfsize ;
	tabsize = (hip->tablen * sizeof(uint)) ;

	f = f && (hfsize == fip->mapsize) ;
	f = f && (tfsize == op->tf.mapsize) ;
	f = f && (hip->wtime > 0) && (hip->wtime <= (utime + SHIFTINT)) ;
	f = f && (hip->sdnoff <= fip->mapsize) ;
	f = f && (hip->sfnoff <= fip->mapsize) ;

/* alignment restriction */
	f = f && ((hip->listoff & (sizeof(int)-1)) == 0) ;
/* size restrictions */
	f = f && (hip->listoff <= fip->mapsize) ;

/* alignment restriction */
	f = f && ((hip->taboff & (sizeof(int)-1)) == 0) ;
/* size restrictions */
	f = f && (hip->taboff <= fip->mapsize) ;

	f = f && (tabsize <= fip->mapsize) ;

	f = f && ((hip->taboff + tabsize) <= fip->mapsize) ;

	if (f) {

	    int	essize = hip->essize ;
	    int	erlen = hip->erlen ;
	    int	eilen = hip->eilen ;

/* alignment restrictions */

	    f = f && ((hip->esoff & (sizeof(int)-1)) == 0) ;
	    f = f && ((hip->eroff & (sizeof(int)-1)) == 0) ;
	    f = f && ((hip->eioff & (sizeof(int)-1)) == 0) ;

/* size restrictions */

	    f = f && ((hip->esoff + essize) <= hfsize) ;
	    f = f && ((hip->eroff + (erlen * sizeof(int))) <= hfsize) ;
	    f = f && ((hip->eioff + (eilen * 3 * sizeof(int))) <= hfsize) ;

	} /* end if */

	if (! f)
	    rs = SR_BADFMT ;

	return rs ;
}
/* end subroutine (txtindexes_hdrverify) */


static int txtindexes_audithash(TXTINDEXES *op,OFFINDEX *oip)
{
	TXTINDEXES_FI	*fip = &op->hf ;
	TXTINDEXES_MI	*mip = &op->mi ;
	TXTINDEXHDR	*hip ;
	uint		listoff ;
	uint		tagoff ;
	uint		hfsize ;
	uint		tfsize ;
	uint		listsize ;
	uint		*uip ;
	int		rs = SR_OK ;
	int		i, j ;
	int		ntags ;

	hip = &op->ifi ;

/* loop over table entries */

	hfsize = fip->mapsize ;
	tfsize = op->tf.mapsize ;

	for (i = 0 ; i < hip->tablen ; i += 1) {

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

	        listsize = (ntags + 1) * sizeof(uint) ;
	        if ((listsize + listoff) >= hfsize) {

	            rs = SR_BADFMT ;
	            break ;

	        } /* end if (error) */

	        for (j = 0 ; j < ntags ; j += 1) {

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


static int txtindexes_auditeigen(TXTINDEXES *op)
{
	TXTINDEXHDR	*hip ;
	TXTINDEXES_MI	*mip ;
	int		rs = SR_OK ;
	int		i, si, nhi ;
	int		cl ;
	int		nskip ;
	int		essize ;
	int		erlen ;
	int		eilen ;
	int		*ertab ;
	int		(*eitab)[3] ;
	cchar	*estab ;
	cchar	*cp ;

	hip = &op->ifi ;
	mip = &op->mi ;

	essize = hip->essize ;
	erlen = hip->erlen ;
	eilen = hip->eilen ;

	estab = mip->estab ;
	eitab = mip->eitab ;
	ertab = mip->ertab ;

/* some record-table checkes */

	if ((rs >= 0) && (ertab[0] != 0)) {
	    rs = SR_BADFMT ;
	}

	if ((rs >= 0) && (estab[0] != '\0')) {
	    rs = SR_BADFMT ;
	}

	if (rs >= 0) {
	    nskip = hip->eiskip ;
	    for (i = 1 ; i < erlen ; i += 1) {

	        si = ertab[i] ;
	        if ((si <= 0) || (si > essize)) {
	            rs = SR_BADFMT ;
	            break ;
	        }

	        cp = (estab + si) ;
	        cl = strnlen(cp,(NATURALWORDLEN + 1)) ;
	        if (cl >= NATURALWORDLEN) {
	            rs = SR_BADFMT ;
	            break ;
	        }

	        if (cp[-1] == '\0') {
	            rs = strtabfind(estab,eitab,eilen,nskip,cp,cl) ;
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
	    for (i = 1 ; i < eilen ; i += 1) {

	        si = eitab[i][0] ;
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
	    int		j ;
	    cchar	*ecp ;

	    cp = (estab + 1) ;
	    ecp = (estab + essize) ;
	    while ((cp < ecp) && cp[0]) {

	        si = (cp - estab) ;
	        cl = strlen(cp) ;

	        for (j = 0 ; j < erlen ; j += 1) {
	            if (si == ertab[j])
	                break ;
	        }

	        if (j >= erlen) {
	            rs = SR_BADFMT ;
	        }

	        if (rs >= 0) {
	            rs = strtabfind(estab,eitab,eilen,nskip,cp,cl) ;
	        }

	        cp = (cp + cl + 1) ;
	        if (rs < 0) break ;
	    } /* end while */

	} /* end if (ok) */
	return rs ;
}
/* end subroutine (txtindexes_auditeigen) */

/* index the beginning-of-line offsets in the TAG file */
static int offindex_tags(OFFINDEX *oip,cchar *fp,int fl) noex {
	off_t	lineoff = 0 ;
	int		rs = SR_OK ;
	int		len ;
	int		ll ;
	int		n = 0 ;
	cchar	*tp ;

	while ((tp = strnchr(fp,fl,'\n')) != NULL) {

	    len = ((tp + 1) - fp) ;
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

static int tag_parse(TXTINDEXES_TAG *tagp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar	*tp ;

	if (sl < 0) {
	    if ((tp = strchr(sp,'\n')) != 0) {
	        sl = (tp - sp) ;
	    } else {
	        rs = SR_BADFMT ;
	    }
	}

	if (rs >= 0) {
	    tagp->recoff = 0 ;
	    tagp->reclen = 0 ;
	    tagp->fname[0] = '\0' ;

	    if ((tp = strnchr(sp,sl,':')) != NULL) {

	        rs = snwcpy(tagp->fname,MAXPATHLEN,sp,(tp - sp)) ;
	        len = rs ;

	        sl -= ((tp + 1) - sp) ;
	        sp = (tp + 1) ;

	    } /* end if */

	    if (rs >= 0) {

	        if ((tp = strnchr(sp,sl,',')) != NULL) {

	            rs = cfdecui(sp,(tp - sp),&tagp->recoff) ;

	            sl -= ((tp + 1) - sp) ;
	            sp = (tp + 1) ;
	            if (rs >= 0) {

	                if ((tp = strnpbrk(sp,sl,"\t ")) != NULL) {
	                    sl = (tp - sp) ;
	                }

	                tagp->reclen = INT_MAX ;
	                if (sl >= 0) {
	                    rs = cfdecui(sp,sl,&tagp->reclen) ;
	                }

	            } /* end if (ok) */

	        } else
	            rs = SR_BADFMT ;

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

static int vcmpuint(const void *v1p,const void *v2p) noex {
	const uint	*i1p = (const uint *) v1p ;
	const uint	*i2p = (const uint *) v2p ;
	int		rc = 0 ;
	if (i1p != NULL) {
	    if (i2p != NULL) {
	        rc = (*i1p - *i2p) ;
	    } else
	        rc = -1 ;
	} else
	    rc = +1 ;
	return rc ;
}
/* end subroutine (vcmpuint) */

#if	CF_SORTKEYS

/* reverse sort strings by 1. string length and 2. string value */
static int vcmpkey(cchar **s1pp,cchar **s2pp) noex {
	int		rc = 0 ;
	if ((s1pp != NULL) || (s2pp != NULL)) {
	    if (s1pp != NULL) {
	        if (s2pp != NULL) {
	            cchar	*s1 = *s1pp ;
	            cchar	*s2 = *s2pp ;
	            int		l1, l2 ;
	            l1 = strlen(s1) ;
	            l2 = strlen(s2) ;
	            rc = (l2 - l1) ;
	            if (rc == 0)
	                rc = strcmp(s2,s1) ;
	        } else
	            rc = -1 ;
	    } else
	        rc = +1 ;
	}
	return rc ;
}
/* end subroutine (vcmpkey) */

#endif /* CF_SORTKEYS */


#if	CF_SORTLISTS
static int cmplistdesc(const LISTDESC *l1p,const LISTDESC *l2p) noex {
	return (l1p->ntags - l2p->ntags) ;
}
/* end subroutine (cmplistdesc) */
#endif /* CF_SORTLISTS */


