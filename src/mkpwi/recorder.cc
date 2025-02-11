/* recorder SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* database of (strings) recorder object */
/* version %I% last-modified %G% */

#define	CF_SAFE		1		/* safety */
#define	CF_FASTGROW	1		/* grow exponetially? */
#define	CF_EXCLUDEKEY	0		/* exclude small keys from indices */

/* revision history:

	= 2002-04-29, David A­D­ Morano
	This object module was created for the MKPWI program.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	recorder

	Description:
	This object module creates records that represent system
	password (PASSWD DB) username and realname information.  The
	recrods can be indexed by various numbers of characters
	from the last name and the first name of each record.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<hash.h>
#include	<nextpowtwo.h>
#include	<randlc.h>
#include	<localmisc.h>
#include	<debug.h>

#include	"recorder.h"


/* local defines */

#define	RC		recorder
#define	RC_ENT		recorder_ent
#define	RC_INFO		recorder_info

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	NSHIFT		6


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int recorder_ctor(dw *op,Args ... args) noex {
    	RECORDER	*hup = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recorder_ctor) */

static int recorder_dtor(dw *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (recorder_dtor) */

template<typename ... Args>
static inline int recorder_magic(dw *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == RECORDER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (recorder_magic) */

static int	recorder_extend(RC *) ;
static int	recorder_matfl3(RC *,cchar *,uint [][2],int,cchar *) ;
static int	recorder_matun(RC *,cchar *,uint [][2],int,cchar *) ;
static int	recorder_cden(RC *,int,int) ;


/* local variables */

enum indices {
	index_l1,
	index_l3,
	index_f,
	index_fl3,
	index_un,
	index_overlast
} ;


/* exported variables */


/* exported subroutines */

int recorder_start(RC *asp,int n,int opts) noex {
	int		rs ;
	int		size ;
	void		*p ;

	if (asp == NULL) return SR_FAULT ;

	memclear(asp) ;

	if (n < RECORDER_STARTNUM)
	    n = RECORDER_STARTNUM ;

	size = (n * szof(RC_ENT)) ;
	if ((rs = uc_malloc(size,&p)) >= 0) {
	    asp->rectab = p ;
	    asp->e = n ;
	    asp->c = 0 ;
	    asp->opts = opts ;
	    asp->i = 0 ;
	    memset(&asp->rectab[asp->i],0,sizeof(RC_ENT)) ;
	    asp->i += 1 ;
	    asp->magic = RECORDER_MAGIC ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (recorder_start) */

int recorder_finish(RC *asp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (asp->rectab != NULL) {
	    rs1 = uc_free(asp->rectab) ;
	    if (rs >= 0) rs = rs1 ;
	    asp->rectab = NULL ;
	}

	rs = asp->i ;
	asp->e = 0 ;
	asp->i = 0 ;
	asp->magic = 0 ;
	return rs ;
}
/* end subroutine (recorder_finish) */

int recorder_add(RC *asp,RC_ENT *ep) noex {
	int		rs = SR_OK ;
	int		i = 0 ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (asp->i < 0) return asp->i ;

/* do we need to extend the table? */

	if ((asp->i + 1) > asp->e) {
	    rs = recorder_extend(asp) ;
	} /* end if */

/* copy the new one in */

	if (rs >= 0) {

	    i = asp->i ;
	    asp->rectab[i] = *ep ;
	    asp->i = (i+1) ;

	    asp->c += 1 ;
	} /* end if (ok) */

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (recorder_add) */

int recorder_already(RC *asp,RC_ENT *ep) noex {
	cint		esize = szof(RC_ENT) ;
	int		i ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (asp->i < 0)
	    return asp->i ;

	for (i = 0 ; i < asp->i ; i += 1) {
	    if (memcmp(ep,(asp->rectab+i),esize)) break ;
	} /* end for */

	return (i < asp->i) ? i : SR_NOTFOUND ;
}
/* end subroutine (recorder_already) */

int recorder_rtlen(RC *asp) noex {
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;

	return asp->i ;
}
/* end subroutine (recorder_rtlen) */

int recorder_count(RC *asp) noex {

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	return asp->c ;
}
/* end subroutine (recorder_count) */

/* calculate the index table length (number of entries) at this point */
int recorder_indlen(RC *asp) noex {
	int		n ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	n = nextpowtwo(asp->i) ;

	return n ;
}
/* end subroutine (recorder_indlen) */

/* calculate the index table size */
int recorder_indsize(RC *asp) noex {
	int		n ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	n = nextpowtwo(asp->i) ;

	return (n * 2 * szof(int)) ;
}
/* end subroutine (recorder_indsize) */

/* get the address of the rectab array */
int recorder_gettab(RC *asp,RC_ENT **rpp) noex {
	int		size ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (rpp != NULL) {
	    *rpp = asp->rectab ;
	}

	size = (asp->i * szof(RC_ENT)) ;
	return size ;
}
/* end subroutine (recorder_gettab) */

/* create a record index for the caller */
int recorder_mkindl1(RC *asp,cchar *s,uint (*it)[2],int itsize) noex {
	uint		rhash ;
	cint	ns = NSHIFT ;
	int		rs = SR_OK ;
	int		hi, ri, c, nc = 1 ;
	int		sl, hl, n, size ;
	int		wi = index_l1 ;
	cchar		*sp ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (it == NULL) return SR_FAULT ;

	n = nextpowtwo(asp->i) ;

	size = n * 2 * szof(uint) ;

	if (size > itsize)
	    return SR_OVERFLOW ;

	memset(it,0,size) ;

	for (ri = 1 ; ri < asp->i ; ri += 1) {

	    sp = s + asp->rectab[ri].last ;

	    sl = strlen(sp) ;

#if	CF_EXCLUDEKEY
	    if (sl < nc)
	        continue ;
#endif

	    hl = MIN(sl,nc) ;
	    rhash = hash_elf(sp,hl) ;

	    hi = hashindex(rhash,n) ;

	    c = 0 ;
	    if ((asp->opts & RECORDER_OSEC) && (it[hi][0] != 0)) {

	        while ((it[hi][0] != 0) &&
	            (strncmp(sp,(s + asp->rectab[it[hi][0]].last),hl) != 0)) {

	            if (asp->opts & RECORDER_ORANDLC) {
	                rhash = randlc(rhash + c) ;
	            } else {
	                rhash = ((rhash << (32 - ns)) | (rhash >> ns)) + c ;
	            }

	            hi = hashindex(rhash,n) ;
	            c += 1 ;
	        } /* end while */

	    } /* end if (secondary hash on collision) */

	    if (it[hi][0] != 0) {
	        int	lhi ;

	        c += 1 ;
	        while (it[hi][1] != 0) {
	            c += 1 ;
	            hi = it[hi][1] ;
	        }

	        lhi = hi ;			/* save last hash-index value */
	        hi = hashindex((hi + 1),n) ;

	        while (it[hi][0] != 0) {
	            c += 1 ;
	            hi = hashindex((hi + 1),n) ;
	        } /* end while */

	        it[lhi][1] = hi ;		/* update the previous slot */

	    } /* end if (got a hash collision) */

	    it[hi][0] = ri ;
	    it[hi][1] = 0 ;
	    asp->s.c_l1 += c ;
	    recorder_cden(asp,wi,c) ;

	} /* end for (looping through records) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (recorder_mkindl1) */

int recorder_mkindl3(RC *asp,cchar s[],uint it[][2],int itsize) noex {
	uint		rhash ;
	cint	ns = NSHIFT ;
	int		rs = SR_OK ;
	int		hi, ri, c ;
	int		nc = 3 ;
	int		sl, hl, n, size ;
	int		wi = index_l3 ;
	cchar	*sp ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (it == NULL)
	    return SR_FAULT ;

	n = nextpowtwo(asp->i) ;

	size = n * 2 * szof(uint) ;

	if (size > itsize)
	    return SR_OVERFLOW ;

	memset(it,0,size) ;

	for (ri = 1 ; ri < asp->i ; ri += 1) {

	    sp = s + asp->rectab[ri].last ;
	    sl = strlen(sp) ;

#if	CF_EXCLUDEKEY
	    if (sl < nc)
	        continue ;
#endif

	    hl = MIN(sl,nc) ;
	    rhash = hash_elf(sp,hl) ;

	    hi = hashindex(rhash,n) ;

	    c = 0 ;
	    if ((asp->opts & RECORDER_OSEC) && (it[hi][0] != 0)) {

	        while ((it[hi][0] != 0) &&
	            (strncmp(sp,(s + asp->rectab[it[hi][0]].last),hl) != 0)) {

	            if (asp->opts & RECORDER_ORANDLC) {
	                rhash = randlc(rhash + c) ;
	            } else {
	                rhash = ((rhash << (32 - ns)) | (rhash >> ns)) + c ;
	            }

	            hi = hashindex(rhash,n) ;
	            c += 1 ;

	        } /* end while */

	    } /* end if (secondary hash on collision) */

	    if (it[hi][0] != 0) {
	        int	lhi ;

	        c += 1 ;
	        while (it[hi][1] != 0) {
	            c += 1 ;
	            hi = it[hi][1] ;
	        }

	        lhi = hi ;			/* save last hash-index value */
	        hi = hashindex((hi + 1),n) ;

	        while (it[hi][0] != 0) {
	            c += 1 ;
	            hi = hashindex((hi + 1),n) ;
	        } /* end while */

	        it[lhi][1] = hi ;		/* update the previous slot */

	    } /* end if (got a hash collision) */

	    it[hi][0] = ri ;
	    it[hi][1] = 0 ;
	    asp->s.c_l3 += c ;
	    recorder_cden(asp,wi,c) ;

	} /* end for (looping through records) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (recorder_mkindl3) */

int recorder_mkindf(RC *asp,cchar s[],uint it[][2],int itsize) noex {
	uint		rhash ;
	cint	ns = NSHIFT ;
	int		rs = SR_OK ;
	int		hi, ri, c, nc = 1 ;
	int		sl, hl, n, size ;
	int		wi = index_f ;
	cchar	*sp ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (it == NULL) return SR_FAULT ;

	n = nextpowtwo(asp->i) ;

	size = n * 2 * szof(uint) ;

	if (size > itsize)
	    return SR_OVERFLOW ;

	memset(it,0,size) ;

	for (ri = 1 ; ri < asp->i ; ri += 1) {

	    if (asp->rectab[ri].first == 0)
	        continue ;

	    sp = s + asp->rectab[ri].first ;
	    sl = strlen(sp) ;

#if	CF_EXCLUDEKEY
	    if (sl < nc)
	        continue ;
#endif

	    hl = MIN(sl,nc) ;
	    rhash = hash_elf(sp,hl) ;

	    hi = hashindex(rhash,n) ;

	    c = 0 ;
	    if ((asp->opts & RECORDER_OSEC) && (it[hi][0] != 0)) {

	        while ((it[hi][0] != 0) &&
	            (strncmp(sp,(s + asp->rectab[it[hi][0]].first),hl) != 0)) {

	            if (asp->opts & RECORDER_ORANDLC) {
	                rhash = randlc(rhash + c) ;
	            } else {
	                rhash = ((rhash << (32 - ns)) | (rhash >> ns)) + c ;
	            }

	            hi = hashindex(rhash,n) ;
	            c += 1 ;

	        } /* end while */

	    } /* end if (secondary hash on collision) */

	    if (it[hi][0] != 0) {
	        int	lhi ;

	        c += 1 ;
	        while (it[hi][1] != 0) {
	            c += 1 ;
	            hi = it[hi][1] ;
	        }

	        lhi = hi ;			/* save last hash-index value */
	        hi = hashindex((hi + 1),n) ;

	        while (it[hi][0] != 0) {
	            c += 1 ;
	            hi = hashindex((hi + 1),n) ;
	        } /* end while */

	        it[lhi][1] = hi ;		/* update the previous slot */

	    } /* end if (got a hash collision) */

	    it[hi][0] = ri ;
	    it[hi][1] = 0 ;
	    asp->s.c_f += c ;
	    recorder_cden(asp,wi,c) ;

	} /* end for (looping through records) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (recorder_mkindf) */

int recorder_mkindfl3(RC *asp,cchar s[],uint it[][2],int itsize) noex {
	uint		rhash ;
	cint	ns = NSHIFT ;
	int		rs = SR_OK ;
	int		hi, ri, c, maxlast ;
	int		sl, hl, n, size ;
	int		wi = index_fl3 ;
	cchar	*sp ;
	char		hbuf[4 + 1] ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (it == NULL) return SR_FAULT ;

	n = nextpowtwo(asp->i) ;

	size = n * 2 * szof(uint) ;

	if (size > itsize)
	    return SR_OVERFLOW ;

	memset(it,0,size) ;

	for (ri = 1 ; ri < asp->i ; ri += 1) {

	    if ((asp->rectab[ri].first == 0) ||
	        (asp->rectab[ri].last == 0))
	        continue ;

	    sp = s + asp->rectab[ri].last ;
	    sl = strlen(sp) ;

	    if (sl < 3)
	        continue ;

	    maxlast = MIN(sl,3) ;
	    strncpy((hbuf + 1),sp,maxlast) ;

	    sp = s + asp->rectab[ri].first ;
	    hbuf[0] = sp[0] ;

#if	CF_EXCLUDEKEY
	    if (sl < nc)
	        continue ;
#endif

	    hl = 1 + maxlast ;
	    rhash = hash_elf(hbuf,hl) ;

	    hi = hashindex(rhash,n) ;

	    c = 0 ;
	    if ((asp->opts & RECORDER_OSEC) && (it[hi][0] != 0)) {

	        while ((it[hi][0] != 0) &&
	            (recorder_matfl3(asp,s,it,hi,hbuf) <= 0)) {

	            if (asp->opts & RECORDER_ORANDLC) {
	                rhash = randlc(rhash + c) ;
	            } else {
	                rhash = ((rhash << (32 - ns)) | (rhash >> ns)) + c ;
	            }

	            hi = hashindex(rhash,n) ;
	            c += 1 ;

	        } /* end while */

	    } /* end if (secondary hash on collision) */

	    if (it[hi][0] != 0) {
	        int	lhi ;

	        c += 1 ;
	        while (it[hi][1] != 0) {
	            c += 1 ;
	            hi = it[hi][1] ;
	        }

	        lhi = hi ;			/* save last hash-index value */
	        hi = hashindex((hi + 1),n) ;

	        while (it[hi][0] != 0) {
	            c += 1 ;
	            hi = hashindex((hi + 1),n) ;
	        } /* end while */

	        it[lhi][1] = hi ;		/* update the previous slot */

	    } /* end if (got a hash collision) */

	    it[hi][0] = ri ;
	    it[hi][1] = 0 ;
	    asp->s.c_fl3 += c ;
	    recorder_cden(asp,wi,c) ;

	} /* end for (looping through records) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (recorder_mkindfl3) */

int recorder_mkindun(RC *asp,cchar s[],uint it[][2],int itsize) noex {
	uint		rhash ;
	cint	ns = NSHIFT ;
	int		rs = SR_OK ;
	int		hi, ri, c ;
	int		hl, n, size ;
	int		wi = index_un ;
	cchar	*hp ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (it == NULL) return SR_FAULT ;

	n = nextpowtwo(asp->i) ;

	size = n * 2 * szof(uint) ;

	if (size > itsize)
	    return SR_OVERFLOW ;

	memset(it,0,size) ;

	for (ri = 1 ; ri < asp->i ; ri += 1) {

	    hp = s + asp->rectab[ri].username ;
	    hl = strlen(hp) ;

#if	CF_EXCLUDEKEY
	    if (sl < nc) continue ;
#endif

	    rhash = hash_elf(hp,hl) ;

	    hi = hashindex(rhash,n) ;

	    c = 0 ;
	    if ((asp->opts & RECORDER_OSEC) && (it[hi][0] != 0)) {

	        while ((it[hi][0] != 0) &&
	            (recorder_matun(asp,s,it,hi,hp) <= 0)) {

	            if (asp->opts & RECORDER_ORANDLC) {
	                rhash = randlc(rhash + c) ;
	            } else {
	                rhash = ((rhash << (32 - ns)) | (rhash >> ns)) + c ;
	            }

	            hi = hashindex(rhash,n) ;

	            c += 1 ;

	        } /* end while */

	    } /* end if (secondary hash on collision) */

	    if (it[hi][0] != 0) {
	        int	lhi ;

	        c += 1 ;
	        while (it[hi][1] != 0) {
	            c += 1 ;
	            hi = it[hi][1] ;
	        }

	        lhi = hi ;			/* save last hash-index value */
	        hi = hashindex((hi + 1),n) ;

	        while (it[hi][0] != 0) {
	            c += 1 ;
	            hi = hashindex((hi + 1),n) ;
	        } /* end while */

	        it[lhi][1] = hi ;		/* update the previous slot */

	    } /* end if (got a hash collision) */

	    it[hi][0] = ri ;
	    it[hi][1] = 0 ;
	    asp->s.c_un += c ;
	    recorder_cden(asp,wi,c) ;

	} /* end for (looping through records) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (recorder_mkindun) */

/* get statistics */
int recorder_info(RC *asp,RC_INFO *ip) noex {
	int		rs = SR_OK ;
	int		n ;

#if	CF_SAFE
	if (asp == NULL) return SR_FAULT ;

	if (asp->magic != RECORDER_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (ip == NULL)
	    return SR_FAULT ;

	n = nextpowtwo(asp->i) ;

	*ip = asp->s ;
	ip->ilen = n ;
	rs = asp->i ;

	return rs ;
}
/* end subroutine (recorder_info) */


/* private subroutines */

static int recorder_extend(RC *asp) noex {
	int		rs = SR_OK ;
	int		ne, size ;
	uint		*nrt = NULL ;

#if	CF_FASTGROW
	ne = ((asp->e + 1) * 2) ;
#else
	ne = (asp->e + RECORDER_STARTNUM) ;
#endif /* CF_FASTGROW */

	size = ne * szof(RC_ENT) ;
	if ((rs = uc_realloc(asp->rectab,size,&nrt)) >= 0) {
	    asp->e = ne ;
	    asp->rectab = (RC_ENT *) nrt ;
	} else {
	    asp->i = rs ;
	}

	return (rs >= 0) ? asp->e : rs ;
}
/* end subroutine (recorder_extend) */

static int recorder_matfl3(RC *asp,cc *s,uint it[][2],int hi,cc *hbuf) noex {
	int		si = asp->rectab[it[hi][0]].first ;
	int		f ;
	f = ((s + si)[0] == hbuf[0]) ;
	if (f) {
	    si = asp->rectab[it[hi][0]].last ;
	    f = (strncmp((s + si),(hbuf + 1),3) == 0) ;
	}
	return f ;
}
/* end subroutine (recorder_matfl3) */

static int recorder_matun(RC *asp,cc *s,uint (*it)[2],int hi,cc *hbuf) noex {
	int		si = asp->rectab[it[hi][0]].username ;
	int		f ;
	f = (strcmp((s + si),hbuf) == 0) ;
	return f ;
}
/* end subroutine (recorder_matun) */

static int recorder_cden(RC *asp,int wi,int c) noex {
	if (c >= RECORDER_NCOLLISIONS) {
	    c = (RECORDER_NCOLLISIONS - 1) ;
	}
	asp->s.cden[wi][c] += 1 ;
	return 0 ;
}
/* end subroutine (recorder_cden) */


