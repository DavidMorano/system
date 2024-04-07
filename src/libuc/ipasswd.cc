/* ipasswd SUPPORT */
/* lang=C++20 */

/* indexed PASSWD GECOS DB */
/* version %I% last-modified %G% */

#define	CF_SAFE		0		/* safe mode */
#define	CF_USEFL3	1		/* use first-last3 index (fast) */

/* revision history:

	= 2002-04-29, David A�D� Morano
	This was rewritten (as messy as it currently is) from a
	previous pierce of code that was even worse.

*/

/* Copyright � 2002 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This object module provides an interface to a data base of
	information about the GECOS name in the system PASSWD
	database.

	The system PASSWD database (whether a file or whatever) was
	enumerated (separately) and an index file was made with
	several indices to lookup usernames based on a real name.
	For starters only indices consisting of using the first
	character of the last name, the first 3 characters of the
	last name, and the first character of the first name have
	been used.  But future index files might provide more
	combinations, like using the first 3 characters of the last
	name combined with the first character of the first name!

	The various data and indices were written into a file. We
	are accessing that file within this object.

	Extra note: this code needs another rewrite!

	Name:
	ipasswd_fetch

	Synopsis:
	int ipasswd_fetch(IP *op,realname *np,IP_CUR *curp,int opts,
			char *up) noex

	Arguments:
	op		object pointer
	np		pointer to realname object
	curp		cursor pointer
	opts		options
	up		result buffer (at least USERNAMELEN+1 in size)

	Returns:
	<0		error
	>=0		found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>		/* Memory Management */
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<mkfnamesuf.h>
#include	<storeitem.h>
#include	<realname.h>
#include	<pwihdr.h>
#include	<hash.h>
#include	<strn.h>
#include	<mkpath.h>
#include	<mkfnamesuf.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<intceil.h>
#include	<intsat.h>
#include	<randlc.h>
#include	<matxstr.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"ipasswd.h"


/* local defines */

#define	IPASSWD_IDLEN	(16 + 4)
#define	IPASSWD_HEADLEN	(pwihdr_overlast * sizeif(uint))
#define	IPASSWD_TOPLEN	(IPASSWD_IDLEN + IPASSWD_HEADLEN)

#define	IPASSWD_IDOFF	0
#define	IPASSWD_HEADOFF	IPASSWD_IDLEN
#define	IPASSWD_BUFOFF	(IPASSWD_HEADOFF + IPASSWD_HEADLEN)

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	TO_FILECOME	5
#define	TO_OPEN		(60 * 60)
#define	TO_ACCESS	(1 * 60)
#define	TO_CHECK	5

#define	TI_MINUPDATE	4		/* minimum time between updates */

#define	NSHIFT		6		/* shift bits for secondary hash */


/* external subroutines */

extern int	getfstype(char *,int,int) ;
extern int	isValidMagic(cchar *,int,cchar *) ;


/* local structures */


/* forward references */

static int	ipasswd_hdrload(IPASSWD *) noex ;
static int	ipasswd_hdrloader(IPASSWD *) noex ;
static int	ipasswd_enterbegin(IPASSWD *,time_t) noex ;
static int	ipasswd_enterend(IPASSWD *,time_t) noex ;

static int	ipasswd_fileopen(IPASSWD *,time_t) noex ;
static int	ipasswd_fileclose(IPASSWD *) noex ;
static int	ipasswd_mapbegin(IPASSWD *,time_t) noex ;
static int	ipasswd_mapend(IPASSWD *) noex ;
static int	ipasswd_remotefs(IPASSWD *) noex ;
static int	ipasswd_keymatchfl3(IPASSWD *,int,int,realname *) noex ;
static int	ipasswd_keymatchl3(IPASSWD *,int,int,realname *) noex ;
static int	ipasswd_keymatchl1(IPASSWD *,int,int,realname *) noex ;
static int	ipasswd_keymatchf(IPASSWD *,int,int,realname *) noex ;
static int	ipasswd_keymatchall(IPASSWD *,int,int,realname *) noex ;

static int	ipaswd_mapcheck(IPASSWD *,time_t) noex ;

#ifdef	COMMENT
static int	ipasswd_keymatchlast(IPASSWD *,int,int,char *,int) noex ;
#endif

static int	mkourfname(char *,cchar *) noex ;

static int	hashindex(uint,int) noex ;

static int	detOurSuf(cchar *,cchar *,int) noex ;


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

#ifdef	COMMENT
IPASSWD_OBJ	ipasswd = {
	"ipasswd",
	sizeof(ipasswd),
	sizeof(ipasswd_cur)
} ;
#endif /* COMMENT */


/* exported subroutines */

int ipasswd_open(IPASSWD *op,cchar *dbname) noex {
	const time_t	dt = time(nullptr) ;
	int		rs ;
	int		rs1 ;
	char		dbfname[MAXPATHLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (dbname == nullptr) return SR_FAULT ;

	memclear(op) ;
	op->fd = -1 ;
	op->oflags = O_RDONLY ;
	op->operm = 0666 ;
	op->pagesize = getpagesize() ;

/* store filename away */

	if ((rs = mkourfname(dbfname,dbname)) >= 0) {
	    cchar	*cp ;
	    if ((rs = uc_mallocstrw(dbfname,-1,&cp)) >= 0) {
	        op->fname = cp ;
	        if ((rs = ipasswd_fileopen(op,dt)) >= 0) {
	            if ((rs = ipasswd_mapbegin(op,dt)) >= 0) {
	                if ((rs = ipasswd_hdrload(op)) >= 0) {
	                    op->magic = IPASSWD_MAGIC ;
	                }
	                if (rs < 0) {
	                    ipasswd_mapend(op) ;
	                }
	            } /* end if (ipasswd_mapbegin) */
	            rs1 = ipasswd_fileclose(op) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (file-open) */
	        if (rs < 0) {
	            uc_free(op->fname) ;
	            op->fname = nullptr ;
	        }
	    } /* end if (store) */
	} /* end if (mk-filename) */

	return rs ;
}
/* end subroutine (ipasswd_open) */

int ipasswd_close(IPASSWD *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;

	if (op->mapdata != nullptr) {
	    rs1 = ipasswd_mapend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}

	if (op->fname != nullptr) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (ipasswd_close) */

int ipasswd_count(IPASSWD *op) noex {
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;

	return (op->rtlen - 1) ;
}
/* end subroutine (ipasswd_count) */

int ipasswd_countindex(IPASSWD *op) noex {
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;

	return op->rilen ;
}
/* end subroutine (ipasswd_countindex) */

int ipasswd_curbegin(IPASSWD *op,IPASSWD_CUR *curp) noex {
	int		i ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	op->f.cursor = true ;
	op->f.cursoracc = false ;

	for (i = 0 ; i < IPASSWD_NINDICES ; i += 1) {
	    curp->i[i] = -1 ;
	}

	curp->magic = IPASSWD_CURMAGIC ;
	return SR_OK ;
}
/* end subroutine (ipasswd_curbegin) */

int ipasswd_curend(IPASSWD *op,IPASSWD_CUR *curp) noex {
	const time_t	dt = time(nullptr) ;
	int		rs = SR_OK ;
	int		i ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != IPASSWD_CURMAGIC) return SR_NOTOPEN ;

	if (op->f.cursoracc)
	    op->ti_access = dt ;

	op->f.cursor = false ;
	for (i = 0 ; i < IPASSWD_NINDICES ; i += 1) {
	    curp->i[i] = -1 ;
	}

	curp->magic = 0 ;
	return rs ;
}
/* end subroutine (ipasswd_curend) */

int ipasswd_enum(IPASSWD *op,IPASSWD_CUR *curp,char *ubuf,cc *sa,
		char *rbuf,int rlen) noex {
	time_t		dt = 0 ;
	int		rs ;
	int		rs1 ;
	int		ri ;
	int		ul = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == nullptr) return SR_FAULT ;
	if (ubuf == nullptr) return SR_FAULT ;
	if (sa == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (curp->magic != IPASSWD_CURMAGIC) return SR_NOTOPEN ;

	if (! op->f.cursor) return SR_INVALID ;

	ri = (curp->i[0] < 1) ? 1 : (curp->i[0] + 1) ;

/* capture a hold on the file */

	if ((rs = ipasswd_enterbegin(op,dt)) >= 0) {
	    if (ri < op->rtlen) {
	        int	si ;
	        int	ui ;
	        cchar	*cp ;

	        if (sa != nullptr) {
	            STOREITEM	sio ;
	            DSTR	ss[5] ;

/* first */

	            si = op->rectab[ri].first ;
	            ss[0].sbuf = (char *) (op->stab + si) ;
	            ss[0].slen = -1 ;

/* middle-1 */

	            si = op->rectab[ri].m1 ;
	            ss[1].sbuf = (char *) (op->stab + si) ;
	            ss[1].slen = -1 ;

/* middle-2 */

	            si = op->rectab[ri].m2 ;
	            ss[2].sbuf = (char *) (op->stab + si) ;
	            ss[2].slen = -1 ;

/* middle=3 */

	            ss[3].sbuf = nullptr ;
	            ss[3].slen = 0 ;

/* last */

	            si = op->rectab[ri].last ;
	            ss[4].sbuf = (char *) (op->stab + si) ;
	            ss[4].slen = -1 ;

/* done */

	            if ((rs = storeitem_start(&sio,rbuf,rlen)) >= 0) {
	                int		j ;
	                int		sl ;
	                int		c = 0 ;
	                cchar	*sp ;
	                cchar	**spp ;
	                for (j = 0 ; (rs >= 0) && (j < 5) ; j += 1) {
	                    sp = ss[j].sbuf ;
	                    sl = ss[j].slen ;
	                    if ((sp != nullptr) && (sl != 0) && (*sp != '\0')) {
				cint	ssl = ss[j].slen ;
				cchar		*ssp = ss[j].sbuf ;
	                        spp = (sa+c++) ;
	                        rs = storeitem_strw(&sio,ssp,ssl,spp) ;
	                    }
	                } /* end for */
	                sa[c] = nullptr ;
	                rs1 = storeitem_finish(&sio) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (storeitem) */
	        } /* end if */

	        if (rs >= 0) {

	            ui = op->rectab[ri].username ;

	            if (ubuf != nullptr) {
			cint	ulen = IPASSWD_USERNAMELEN ;
	                cp = strwcpy(ubuf,(op->stab + ui),ulen) ;
	                ul = (cp - ubuf) ;
	            } else {
	                ul = strlen(op->stab + ui) ;
	            }

/* update the cursor */

	            curp->i[0] = ri ;

	        } /* end if */

	    } else {
	        rs = SR_NOTFOUND ;
	    } /* end if (valid) */
	    rs1 = ipasswd_enterend(op,dt) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ipasswd-enter) */

	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (ipasswd_enum) */


/* fetch an entry by a real-name key lookup */
int ipasswd_fetch(IPASSWD *op,realname *rp,IPASSWD_CUR *curp,
		int opts,char *up) noex {
	IPASSWD_CUR	cur ;
	time_t		dt = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		ul = 0 ;
	int		f_cur = false ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;
#endif

	if (rp == nullptr) return SR_FAULT ;

	if (curp == nullptr) {
	    curp = &cur ;
	    curp->magic = IPASSWD_CURMAGIC ;
	    curp->wi = 0 ;
	    for (i = 0 ; i < IPASSWD_NINDICES ; i += 1) {
	        curp->i[i] = -1 ;
	    }
	} else {
	    f_cur = true ;
	    if (curp->magic != IPASSWD_CURMAGIC) return SR_NOTOPEN ;
	    if (! op->f.cursor) return SR_INVALID ;
	}

	if (up) *up = '\0' ;

/* do we have a hold on the file? */

	if ((rs = ipasswd_enterbegin(op,dt)) >= 0) {
	    uint	hv, hi, ri, ui ;
	    cint	ns = NSHIFT ;
	    int		wi ;
	    int		hl, c ;
	    cchar	*hp ;
	    cchar	*cp ;
	    char	hbuf[4 + 1] ;

/* continue with regular fetch activities */

	    op->f.cursoracc = true ;	/* doesn't hurt if no cursor! */

/* which index do we want to use? */

	    wi = -1 ;
	    if (rs >= 0) {

	        if ((rp->len.first >= 1) && (rp->len.last >= 3)) {
#if	CF_USEFL3
	            wi = index_fl3 ;
	            hp = hbuf ;
	            hl = 4 ;
	            hbuf[0] = rp->first[0] ;
	            strncpy((hbuf + 1),rp->last,3) ;
#else
	            wi = index_l3 ;
	            hp = rp->last ;
	            hl = 3 ;
#endif /* CF_USEFL3 */
	        } else if (rp->len.last >= 3) {
	            wi = index_l3 ;
	            hp = rp->last ;
	            hl = 3 ;
	        } else if (rp->len.last >= 1) {
	            wi = index_l1 ;
	            hp = rp->last ;
	            hl = 1 ;
	        } else if (rp->len.first >= 1) {
	            wi = index_f ;
	            hp = rp->first ;
	            hl = 1 ;
	        } else {
	            wi = -1 ;
	        }

	        if (wi < 0) {
	            rs = SR_INVALID ;
	        }

	    } /* end if (ok) */

/* OK, we go from here */

	    if (rs >= 0) {

	        if (curp->i[wi] < 0) {

	            hv = hash_elf(hp,hl) ;

	            hi = hashindex(hv,op->rilen) ;

/* start searching! */

	            if (op->ropts & IPASSWD_ROSEC) {
	                bool	f{} ;

	                c = 0 ;
	                while ((ri = (op->recind[wi])[hi][0]) != 0) {

	                    if (ri >= op->rtlen) {
	                        rs = 0 ;
	                        break ;
	                    }

	                    switch (wi) {
	                    case index_fl3:
	                        f = ipasswd_keymatchfl3(op,opts,ri,rp) ;
	                        break ;
	                    case index_l3:
	                        f = ipasswd_keymatchl3(op,opts,ri,rp) ;
	                        break ;
	                    case index_l1:
	                        f = ipasswd_keymatchl1(op,opts,ri,rp) ;
	                        break ;
	                    case index_f:
	                        f = ipasswd_keymatchf(op,opts,ri,rp) ;
	                        break ;
	                    } /* end switch */

	                    if (f) break ;

	                    op->collisions += 1 ;
	                    if (op->ropts & IPASSWD_RORANDLC) {
	                        hv = randlc(hv + c) ;
	                    } else {
	                        hv = ((hv << (32 - ns)) | (hv >> ns)) + c ;
	                    }

	                    hi = hashindex(hv,op->rilen) ;

	                    c += 1 ;

	                } /* end while */

	                if (ri == 0) {
	                    rs = SR_NOTFOUND ;
	                }

	            } /* end if (secondary hashing) */

	        } else {

/* get the next record index (if there is one) */

	            hi = curp->i[wi] ;
	            if ((hi != 0) && (hi < op->rilen)) {

	                ri = (op->recind[wi])[hi][0] ;
	                if ((ri != 0) && (ri < op->rtlen)) {

	                    hi = (op->recind[wi])[hi][1] ;
	                    if ((hi != 0) && (hi < op->rilen)) {
	                        ri = (op->recind[wi])[hi][0] ;
	                    } else {
	                        rs = SR_NOTFOUND ;
			    }

	                } else {
	                    rs = SR_NOTFOUND ;
			}

	            } else {
	                rs = SR_NOTFOUND ;
		    }

	        } /* end if (preparation) */

	    } /* end if (ok) */

	    if (rs >= 0) {

	        while ((ri = (op->recind[wi])[hi][0]) != 0) {

	            if (ri >= op->rtlen) {
	                ri = 0 ;
	                break ;
	            }

	            if (ipasswd_keymatchall(op,opts,ri,rp)) break ;

	            hi = (op->recind[wi])[hi][1] ;
	            if (hi == 0)
	                break ;

	            op->collisions += 1 ;

	        } /* end while */

	        if ((ri == 0) || (hi == 0)) {
	            rs = SR_NOTFOUND ;
	        }

	    } /* end if (following the existing chain) */

/* if successful, retrieve username */

	    if (rs >= 0) {

	        ui = op->rectab[ri].username ;
	        if (up != nullptr) {

	            cp = strwcpy(up,(op->stab + ui),IPASSWD_USERNAMELEN) ;

	            ul = (cp - up) ;

	        } else {
	            ul = strlen(op->stab + ui) ;
	        }

/* update cursor */

	        if (f_cur) {
	            curp->i[wi] = hi ;
	        }

	    } /* end if (got one) */

	    rs1 = ipasswd_enterend(op,dt) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ipasswd-enter) */

	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (ipasswd_fetch) */


/* fetch an entry by an array of real-name component strings */
int ipasswd_fetcher(IPASSWD *op,IPASSWD_CUR *curp,int opts,char *ubuf,
		cchar **sa,int sn) noex {
	IPASSWD_CUR	cur ;
	time_t		dt = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		ul = 0 ;
	int		f_cur = false ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;
#endif

	if (sa == nullptr) return SR_FAULT ;

	if (curp == nullptr) {
	    curp = &cur ;
	    curp->magic = IPASSWD_CURMAGIC ;
	    for (i = 0 ; i < IPASSWD_NINDICES ; i += 1) curp->i[i] = -1 ;
	} else {
	    f_cur = true ;
	    if (curp->magic != IPASSWD_CURMAGIC) return SR_NOTOPEN ;
	    if (! op->f.cursor) return SR_INVALID ;
	}

	if (ubuf) ubuf[0] = '\0' ;

/* do we have a hold on the file? */

	if ((rs = ipasswd_enterbegin(op,dt)) >= 0) {
	    realname	rn, *rp = &rn ;
	    uint	hv, hi, ri, ui ;
	    cint	ns = NSHIFT ;
	    int		wi ;
	    int		hl, c ;
	    cchar	*hp ;
	    cchar	*cp ;
	    char	hbuf[4 + 1] ;

/* continue with regular fetch activities */

	    op->f.cursoracc = true ;	/* doesn't hurt if no cursor! */

	    if (rs >= 0) {
	        if ((rs = realname_startpieces(rp,sa,sn)) >= 0) {

/* which index do we want to use? */

	            wi = -1 ;
	            if ((rp->len.first >= 1) && (rp->len.last >= 3)) {
#if	CF_USEFL3
	                wi = index_fl3 ;
	                hp = hbuf ;
	                hl = 4 ;
	                hbuf[0] = rp->first[0] ;
	                strncpy((hbuf + 1),rp->last,3) ;
#else
	                wi = index_l3 ;
	                hp = rp->last ;
	                hl = 3 ;
#endif /* CF_USEFL3 */
	            } else if (rp->len.last >= 3) {
	                wi = index_l3 ;
	                hp = rp->last ;
	                hl = 3 ;
	            } else if (rp->len.last >= 1) {
	                wi = index_l1 ;
	                hp = rp->last ;
	                hl = 1 ;
	            } else if (rp->len.first >= 1) {
	                wi = index_f ;
	                hp = rp->first ;
	                hl = 1 ;
	            } else {
	                wi = -1 ;
	            }

	            if (wi < 0) {
	                rs = SR_INVALID ;
	            }

#if	CF_DEBUGS
	            debugprintf("ipasswd_fetcher: wi=%d key=%t\n",wi,hp,hl) ;
	            debugprintf("ipasswd_fetcher: rilen=%u (\\x%08x)\n",
	                op->rilen, op->rilen) ;
#endif

/* OK, we go from here */

	            if (rs >= 0) {

	                if (curp->i[wi] < 0) {

#if	CF_DEBUGS
	                    debugprintf("ipasswd_fetcher: new cursor\n") ;
#endif

	                    hv = hash_elf(hp,hl) ;

	                    hi = hashindex(hv,op->rilen) ;

#if	CF_DEBUGS
	                    debugprintf("ipasswd_fetcher: hv=%08x hi=%u\n",
				hv,hi) ;
#endif

/* start searching! */

	                    if (op->ropts & IPASSWD_ROSEC) {
	                        int	f ;

#if	CF_DEBUGS
	                        if (hi < op->rilen)
	                            debugprintf("ipasswd_fetcher: "
	                                "sec initial ri=%u\n",
	                                (op->recind[wi])[hi][0]) ;
#endif

	                        c = 0 ;
	                        while ((ri = (op->recind[wi])[hi][0]) != 0) {

#if	CF_DEBUGS
	                            debugprintf("ipasswd_fetcher: try ri=%u\n",
	                                ri) ;
#endif

	                            if (ri >= op->rtlen) {
	                                rs = 0 ;
	                                break ;
	                            }

	                            switch (wi) {
	                            case index_fl3:
	                                f = ipasswd_keymatchfl3(op,opts,ri,rp) ;
	                                break ;
	                            case index_l3:
	                                f = ipasswd_keymatchl3(op,opts,ri,rp) ;
	                                break ;
	                            case index_l1:
	                                f = ipasswd_keymatchl1(op,opts,ri,rp) ;
	                                break ;
	                            case index_f:
	                                f = ipasswd_keymatchf(op,opts,ri,rp) ;
	                                break ;
	                            } /* end switch */

	                            if (f)
	                                break ;

	                            op->collisions += 1 ;
	                            if (op->ropts & IPASSWD_RORANDLC) {
	                                hv = randlc(hv + c) ;
	                            } else {
					hv = 0 ;
	                                hv |= (hv << (32 - ns)) ;
					hv |= (hv >> ns) ;
					hv += c ;
	                            }

	                            hi = hashindex(hv,op->rilen) ;

#if	CF_DEBUGS
	                            debugprintf("ipasswd_fetcher: "
	                                "new hv=%08x hi=%u\n", hv,hi) ;
#endif

	                            c += 1 ;

	                        } /* end while */

#if	CF_DEBUGS
	                        debugprintf("ipasswd_fetcher: "
	                            "index-key-match ri=%u\n", ri) ;
#endif

	                        if (ri == 0) {
	                            rs = SR_NOTFOUND ;
	                        }

	                    } /* end if (secondary hashing) */

	                } else {

/* get the next record index (if there is one) */

#if	CF_DEBUGS
	                    debugprintf("ipasswd_fetcher: old cursor\n") ;
#endif

	                    hi = curp->i[wi] ;
	                    if ((hi != 0) && (hi < op->rilen)) {

	                        ri = (op->recind[wi])[hi][0] ;
	                        if ((ri != 0) && (ri < op->rtlen)) {

	                            hi = (op->recind[wi])[hi][1] ;
	                            if ((hi != 0) && (hi < op->rilen)) {
	                                ri = (op->recind[wi])[hi][0] ;
	                            } else {
	                                rs = SR_NOTFOUND ;
				    }

	                        } else {
	                            rs = SR_NOTFOUND ;
				}

	                    } else {
	                        rs = SR_NOTFOUND ;
			    }

	                } /* end if (preparation) */

	            } /* end if (ok) */

#if	CF_DEBUGS
	            debugprintf("ipasswd_fetcher: match search rs=%d hi=%u\n",
	                rs,hi) ;
#endif

	            if (rs >= 0) {

	                while ((ri = (op->recind[wi])[hi][0]) != 0) {

	                    if (ri >= op->rtlen) {
	                        ri = 0 ;
	                        break ;
	                    }

	                    if (ipasswd_keymatchall(op,opts,ri,rp)) break ;

	                    hi = (op->recind[wi])[hi][1] ;
	                    if (hi == 0) break ;

	                    op->collisions += 1 ;

	                } /* end while */

	                if ((ri == 0) || (hi == 0)) {
	                    rs = SR_NOTFOUND ;
	                }

	            } /* end if (following the existing chain) */

#if	CF_DEBUGS
	            debugprintf("ipasswd_fetcher: done w/ search rs=%d\n",rs) ;
#endif

/* if successful, retrieve username */

	            if (rs >= 0) {

	                ui = op->rectab[ri].username ;
	                if (ubuf != nullptr) {
			    cint	ulen = IPASSWD_USERNAMELEN ;
	                    cp = strwcpy(ubuf,(op->stab + ui),ulen) ;
	                    ul = (cp - ubuf) ;
#if	CF_DEBUGS
	                    debugprintf("ipasswd_fetcher: username=%s\n",ubuf) ;
#endif
	                } else {
	                    ul = strlen(op->stab + ui) ;
	                }

/* update cursor */

	                if (f_cur) {
	                    curp->i[wi] = hi ;
	                }

	            } /* end if (got one) */

	            rs1 = realname_finish(rp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (realname) */
	    } /* end if (ok) */

	    rs1 = ipasswd_enterend(op,dt) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ipasswd-enter) */

#if	CF_DEBUGS
	debugprintf("ipasswd_fetcher: ret rs=%d ul=%u\n",rs,ul) ;
#endif

	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (ipasswd_fetcher) */

int ipasswd_getinfo(IPASSWD *op,IPASSWD_INFO *rp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (rp == nullptr) return SR_FAULT ;

	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;

	if (rp != nullptr) {
	    rp->collisions = op->collisions ;
	}

	return rs ;
}
/* end subroutine (ipasswd_info) */

int ipasswd_check(IPASSWD *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;

	if ((! op->f.cursor) && (op->fd >= 0)) {
	    f = f || ((dt - op->ti_access) >= TO_ACCESS) ;
	    f = f || ((dt - op->ti_open) >= TO_OPEN) ;
	    if (f) {
	        if ((rs = ipasswd_mapend(op)) >= 0) {
	            rs = ipasswd_fileclose(op) ;
	        }
	    }
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ipasswd_check) */


/* private subroutines */

static int ipasswd_hdrload(IPASSWD *op) noex {
	int		rs = SR_OK ;
	cchar	*cp = (cchar *) op->mapdata ;

#if	CF_DEBUGS
	debugprintf("ipasswd_hdrload: ent\n") ;
	debugprintf("ipasswd_hdrload: magic=%s\n",cp) ;
#endif

	if (isValidMagic(cp,PWIHDR_MAGICSIZE,PWIHDR_MAGICSTR)) {
	    uint	*table = (uint *) (op->mapdata + IPASSWD_IDLEN) ;
	    const uchar	*vetu = (const uchar *) (cp + 16) ;
	    if (vetu[0] == PWIHDR_VERSION) {
	        if (vetu[1] == ENDIAN) {
	            int		i ;
	            op->ropts = vetu[2] ;

#if	CF_DEBUGS
	            debugprintf("ipasswd_hdrload: ropts=%02x\n",op->ropts) ;
#endif

/* if looks good, read the header stuff */

#if	CF_DEBUGS
	            {
	                int	i ;
	                for (i = 0 ; i < pwihdr_overlast ; i += 1) {
	                    debugprintf("ipasswd_hdrload: header[%02d]=%u\n",
	                        i,table[i]) ;
	                }
	            }
#endif /* CF_DEBUGS */

/* try to validate the header table */

	            for (i = pwihdr_rectab ; i < IPASSWD_IDLEN ; i += 1) {
	                if (table[i] >= op->filesize) break ;
	            } /* end for */

	            if (i >= IPASSWD_IDLEN) {
	                rs = ipasswd_hdrloader(op) ;
	            } else {
	                rs = SR_BADFMT ;
	            }

	        } else {
	            rs = SR_NOTSUP ;
	        }
	    } else {
	        rs = SR_NOTSUP ;
	    }
	} else {
	    rs = SR_NXIO ;
	} /* end if (magic matched) */

#if	CF_DEBUGS
	debugprintf("ipasswd_hdrload: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (ipasswd_hdrload) */

static int ipasswd_hdrloader(IPASSWD *op) noex {
	uint		*table = (uint *) (op->mapdata + IPASSWD_IDLEN) ;
	int		rs = SR_OK ;

/* extract the header table values */

	op->rectab = (IPASSWD_ENT *) (op->mapdata + table[pwihdr_rectab]) ;
	op->rtlen = table[pwihdr_reclen] ;
	op->rtsize = table[pwihdr_recsize] ;

	op->stab = (cchar *) (op->mapdata + table[pwihdr_strtab]) ;
	op->stcount = table[pwihdr_strlen] ;
	op->stsize = table[pwihdr_strsize] ;

	op->rilen = table[pwihdr_idxlen] ;

#if	CF_DEBUGS
	debugprintf("ipasswd_hdrload: rectab=%u\n",table[pwihdr_rectab]) ;
	debugprintf("ipasswd_hdrload: strtab=%u\n",table[pwihdr_strtab]) ;
	debugprintf("ipasswd_hdrload: rtlen=%u\n",op->rtlen) ;
	debugprintf("ipasswd_hdrload: rtsize=%u\n",op->rtsize) ;
	debugprintf("ipasswd_hdrload: stcount=%u\n",op->stcount) ;
	debugprintf("ipasswd_hdrload: stsize=%u\n",op->stsize) ;
	debugprintf("ipasswd_hdrload: rilen=%u\n",op->rilen) ;
#endif

	op->recind[0] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxl1]) ;
	op->recind[1] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxl3]) ;
	op->recind[2] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxf]) ;
	op->recind[3] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxfl3]) ;
	op->recind[4] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxun]) ;

#if	CF_DEBUGS
	{
	    int	i ;
	    for (i = 1 ; i < op->rtlen ; i += 1) {
	        debugprintf("ipasswd_hdrload: rec[%02d] ui=%03u li=%03u\n",
	            i,op->rectab[i].username,op->rectab[i].last) ;
	    }
	}
#endif /* CF_DEBUGS */

#if	CF_DEBUGS
	{
	    int		i ;
	    cchar	*ol = (op->stab + op->stsize) ;
	    cchar	*cp = (cchar *) (op->stab + 1) ;
	    char	prstr[PRSTRLEN + 1] ;
	    for (i = 0 ; cp < ol ; i += 1) {
	        mkprstr(prstr,PRSTRLEN,cp,-1) ;
	        debugprintf("ipasswd_hdrload: s[%03d]=>%s<\n",i,prstr) ;
	        cp += (strlen(cp) + 1) ;
	    }
	}
#endif /* CF_DEBUGS */

	return rs ;
}
/* end subroutine (ipasswd_hdrloader) */

static int ipasswd_enterbegin(IPASSWD *op,time_t dt) noex {
	int		rs ;
	int		f = false ;

#if	CF_DEBUGS
	debugprintf("ipasswd_enterbegin: ent\n") ;
#endif

	op->f.held = true ;
	if ((rs = ipaswd_mapcheck(op,dt)) > 0) {
	    f = true ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ipasswd_enterbegin) */

static int ipasswd_enterend(IPASSWD *op,time_t dt) noex {
	int		rs = SR_OK ;
	(void) dt ;
	if (op->f.held) {
	    op->f.held = false ;
	}
	return rs ;
}
/* end subroutine (ipasswd_enterend) */

static int ipasswd_fileopen(IPASSWD *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    if ((rs = uc_open(op->fname,op->oflags,op->operm)) >= 0) {
	        op->fd = rs ;
	        if (dt == 0) dt = time(nullptr) ;
	        op->ti_open = dt ;
	        if ((uc_closeonexec(op->fd,true)) >= 0) {
	            USTAT	sb ;
	            if ((rs = u_fstat(op->fd,&sb)) >= 0) {
			csize	fsz = size_t(sb.st_size) ;
	                int	sz = 0 ;
	                sz += IPASSWD_IDLEN ;
	                sz += (pwihdr_overlast * sizeof(int)) ;
	                if (int isz = intsat(fsz) ; sb.st_size >= sz) {
	                    op->mtime = sb.st_mtime ;
	                    op->filesize = sb.st_size ;
	                    rs = ipasswd_remotefs(op) ;
	                } else {
	                    rs = SR_NOMSG ;
	                }
	            }
	        }
	        if (rs < 0) {
	            u_close(op->fd) ;
	            op->fd = -1 ;
	        }
	    } /* end if (u_open) */
	} /* end if (needed) */
	return (rs >= 0) ? op->fd : rs ;
}
/* end subroutine (ipasswd_fileopen) */

static int ipasswd_fileclose(IPASSWD *op) noex {
	int		rs = SR_OK ;
	if (op->fd >= 0) {
	    rs = u_close(op->fd) ;
	    op->fd = -1 ;
	}

	return rs ;
}
/* end subroutine (ipasswd_fileclose) */

static int ipasswd_mapbegin(IPASSWD *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->mapdata == nullptr) {
	    cnullptr	np{} ;
	    size_t	ms = uceil(op->filesize,op->pagesize) ;
	    cint	fd = op->fd ;
	    cint	mp = PROT_READ ;
	    cint	mf = MAP_SHARED ;
	    void	*md ;
	    if (ms == 0) ms = op->pagesize ;
	    if ((rs = u_mmap(np,ms,mp,mf,fd,0L,&md)) >= 0) {
	        op->mapdata = charp(md) ;
	        op->mapsize = ms ;
	        op->ti_map = dt ;
	        f = true ;
	    }
	} /* end if (needed) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ipasswd_mapbegin) */

static int ipasswd_mapend(IPASSWD *op) noex {
	int		rs = SR_OK ;
	if (op->mapdata != nullptr) {
	    void	*ma = voidp(op->mapdata) ;
	    csize	ms = op->mapsize ;
	    rs = u_munmap(ma,ms) ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	}
	return rs ;
}
/* end subroutine (ipasswd_mapend) */

static int ipasswd_remotefs(IPASSWD *op) noex {
	int		rs ;
	int		fslen = USERNAMELEN ;
	int		f = false ;
	char		fstype[USERNAMELEN + 1] ;
	if ((rs = getfstype(fstype,fslen,op->fd)) >= 0) {
	    fslen = rs ;
	    f = (matlocalfs(fstype,fslen) < 0) ;
	    op->f.remote = f ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ipasswd_remotefs) */

static int ipasswd_keymatchfl3(IPASSWD *op,int opts,int ri,realname *rp) noex {
	int		si = op->rectab[ri].last ;
	int		f ;

	f = (strncmp((op->stab + si),rp->last,3) == 0) ;

	if (f) {
	    si = op->rectab[ri].first ;
	    f = ((op->stab + si)[0] == rp->first[0]) ;
	}

	return f ;
}
/* end subroutine (ipasswd_keymatchfl3) */

static int ipasswd_keymatchl3(IPASSWD *op,int opts,int ri,realname *rp) noex {
	int		si ;
	int		f ;

	si = op->rectab[ri].last ;
	f = (strncmp((op->stab + si),rp->last,3) == 0) ;

	return f ;
}
/* end subroutine (ipasswd_keymatchl3) */

static int ipasswd_keymatchl1(IPASSWD *op,int opts,int ri,realname *rp) noex {
	int		si ;
	int		f ;

	si = op->rectab[ri].last ;
	f = ((op->stab + si)[0] == rp->last[0]) ;

	return f ;
}
/* end subroutine (ipasswd_keymatchl1) */

static int ipasswd_keymatchf(IPASSWD *op,int opts,int ri,realname *rp) noex {
	int		si ;
	int		f ;

	si = op->rectab[ri].first ;
	f = ((op->stab + si)[0] == rp->first[0]) ;

	return f ;
}
/* end subroutine (ipasswd_keymatchf) */

#ifdef	COMMENT
static int ipasswd_keymatchlast(IPASSWD *op,int opts,int ri,
		char *sp,int hl) noex {
	int		si ;
	int		f ;

	si = op->rectab[ri].last ;

	if (opts & IPASSWD_FOLASTFULL) {
	    f = (strncmp((op->stab + si),sp,3) == 0) ;
	} else {
	    f = (strncmp((op->stab + si),sp,hl) == 0) ;
	}

	return f ;
}
/* end subroutine (ipasswd_keymatchlast) */

#endif /* COMMENT */

static int ipasswd_keymatchall(IPASSWD *op,int opts,int ri,realname *rp) noex {
	int		si ;
	int		f = true ;
	cchar	*s = op->stab ;
	cchar	*sp ;

/* last */

	if (f) {
	    if (opts & IPASSWD_FOLASTFULL) {
	        si = op->rectab[ri].last ;
	        f = (strcmp((s + si),rp->last) == 0) ;
	    } else {
	        si = op->rectab[ri].last ;
	        f = (strncmp((s + si),rp->last,rp->len.last) == 0) ;
	    } /* end if */
	}

/* first */

	if (f) {
	    si = op->rectab[ri].first ;
	    sp = rp->first ;
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        si = op->rectab[ri].first ;
	        f = (strncmp((s + si),sp,rp->len.first) == 0) ;
	    }
	}

/* middle-1 */

	if (f) {
	    si = op->rectab[ri].m1 ;
	    sp = rp->m1 ;
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        si = op->rectab[ri].m1 ;
	        f = (strncmp((s + si),sp,rp->len.m1) == 0) ;
	    }
	}

/* middle-2 */

	if (f) {
	    si = op->rectab[ri].m2 ;
	    sp = rp->m2 ;
	    if ((sp != nullptr) && (sp[0] != '\0')) {
	        si = op->rectab[ri].m2 ;
	        f = (strncmp((s + si),sp,rp->len.m2) == 0) ;
	    }
	}

	return f ;
}
/* end subroutine (ipasswd_keymatchall) */

static int ipaswd_mapcheck(IPASSWD *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->mapdata == nullptr) {
	    if ((rs = ipasswd_fileopen(op,dt)) >= 0) {
	        if ((rs = ipasswd_mapbegin(op,dt)) > 0) { /* only if */
	            f = true ;
	            rs = ipasswd_hdrload(op) ;
	            if (rs < 0) {
	                ipasswd_mapend(op) ;
	            }
		}
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ipaswd_mapcheck) */

static int mkourfname(char *dbfname,cchar *dbname) noex {
	int		rs ;
	cchar	*suf = IPASSWD_SUF ;
	cchar	*endstr = ((ENDIAN != 0) ? "1" : "0") ;

	if (detOurSuf(suf,dbname,-1) > 0) {
	    rs = mkpath1(dbfname,dbname) ;
	} else {
	    rs = mkfnamesuf2(dbfname,dbname,suf,endstr) ;
	}

	return rs ;
}
/* end subroutine (mkourfname) */

static int hashindex(uint i,int n) noex {
	uint		hi = MODP2(i,n) ;
	if (hi == 0) hi = 1 ;
	return hi ;
}
/* end subroutine (hashindex) */

static int detOurSuf(cchar *suf,cchar *fname,int fl) noex {
	int		len = 0 ;
	int		cl ;
	cchar	*cp ;
	if ((cl = sfbasename(fname,fl,&cp)) > 0) {
	    cchar	*tp ;
	    if ((tp = strnrchr(cp,cl,'.')) != nullptr) {
	        cint	suflen = strlen(suf) ;
	        if (strncmp((tp+1),suf,suflen) == 0) {
	            len = (tp-fname) ;
	        }
	    }
	}
	return len ;
}
/* end subroutine (detOurSuf) */


