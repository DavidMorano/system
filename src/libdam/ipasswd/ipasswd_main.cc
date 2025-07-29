/* ipasswd SUPPORT */
/* lang=C++20 */

/* indexed PASSWD GECOS DB */
/* version %I% last-modified %G% */

#define	CF_USEFL3	1		/* use first-last3 index (fast) */

/* revision history:

	= 2002-04-29, David A­D­ Morano
	This was rewritten (as messy as it currently is) from a
	previous pierce of code that was even worse.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ipasswd

	Description:
	This object module provides an interface to a data base of
	information about the GECOS name in the system PASSWD
	database.  The system PASSWD database (whether a file or
	whatever) was enumerated (separately) and an index file was
	made with several indices to lookup usernames based on a
	real name.  For starters only indices consisting of using
	the first character of the last name, the first 3 characters
	of the last name, and the first character of the first name
	have been used.  But future index files might provide more
	combinations, like using the first 3 characters of the last
	name combined with the first character of the first name!
	The various data and indices were written into a file. We
	are accessing that file within this object.  Extra note:
	this code needs another rewrite!

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
	>=0		found
	<0		error (system-return)

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
#include	<getbufsize.h>
#include	<sysval.hh>
#include	<mallocxx.h>
#include	<endian.h>
#include	<mkfnamesuf.h>
#include	<storeitem.h>
#include	<realname.h>
#include	<pwihdr.h>
#include	<hash.h>
#include	<hashindex.h>
#include	<strn.h>
#include	<mkpath.h>
#include	<mkfnamesuf.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<intceil.h>
#include	<intsat.h>
#include	<randlc.h>
#include	<matxstr.h>
#include	<mkx.h>
#include	<isfiledesc.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"ipasswd.h"

import libutil ;

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

#define	SI_STA		storeitem_start
#define	SI_STR		storeitem_strw
#define	SI_FIN		storeitem_finish

#ifndef	CF_USEFL3
#define	CF_USEFL3	1		/* use first-last3 index (fast) */
#endif


/* external subroutines */


/* external variables */


/* local structures */

enum indices {
	index_l1,
	index_l3,
	index_f,
	index_fl3,
	index_un,
	index_overlast
} ;

struct vars {
	int		maxpathlen ;
	int		usernamelen ;
} ;


/* forward references */

template<typename ... Args>
static inline int ipasswd_magic(ipasswd *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == IPASSWD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (ipasswd_magic) */

static int	ipasswd_hdrload(ipasswd *) noex ;
static int	ipasswd_hdrloader(ipasswd *) noex ;
static int	ipasswd_enterbegin(ipasswd *,time_t) noex ;
static int	ipasswd_enterend(ipasswd *,time_t) noex ;

static int	ipasswd_fileopen(ipasswd *,time_t) noex ;
static int	ipasswd_fileclose(ipasswd *) noex ;
static int	ipasswd_mapbegin(ipasswd *,time_t) noex ;
static int	ipasswd_mapend(ipasswd *) noex ;
static int	ipasswd_remotefs(ipasswd *) noex ;
static int	ipasswd_keymatchfl3(ipasswd *,int,int,realname *) noex ;
static int	ipasswd_keymatchl3(ipasswd *,int,int,realname *) noex ;
static int	ipasswd_keymatchl1(ipasswd *,int,int,realname *) noex ;
static int	ipasswd_keymatchf(ipasswd *,int,int,realname *) noex ;
static int	ipasswd_keymatchall(ipasswd *,int,int,realname *) noex ;

static int	ipaswd_mapcheck(ipasswd *,time_t) noex ;

#ifdef	COMMENT
static int	ipasswd_keymatchlast(ipasswd *,int,int,char *,int) noex ;
#endif

static int	mkourfname(char *,cchar *) noex ;

static int	detOurSuf(cchar *,cchar *,int) noex ;

static int	mkvars() noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;

static vars		var ;

constexpr bool		f_usefl3 = CF_USEFL3 ;


/* exported variables */

#ifdef	COMMENT
const ipassswd_obj	ipasswd_modinfo = {
	"ipasswd",
	szof(ipasswd),
	szof(ipasswd_cur)
} ;
#endif /* COMMENT */


/* exported subroutines */

int ipasswd_open(ipasswd *op,cchar *dbname) noex {
	custime		dt = getustime ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && dbname) {
	    static cint		rsv = mkvars() ;
	    memclear(op) ;
	    if ((rs = rsv) >= 0) {
	        if ((rs = pagesize) >= 0) {
	            op->pagesize = rs ;
	            op->fd = -1 ;
	            op->oflags = O_RDONLY ;
	            op->operm = 0666 ;
	            if (char *dbfname ; (rs = malloc_mp(&dbfname)) >= 0) {
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
		        rs1 = uc_free(dbfname) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	        } /* end if (pagesize) */
	    } /* end if (mkvars) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ipasswd_open) */

int ipasswd_close(ipasswd *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ipasswd_magic(op)) >= 0) {
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
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ipasswd_close) */

int ipasswd_count(ipasswd *op) noex {
	int		rs ;
	if ((rs = ipasswd_magic(op)) >= 0) {
	    rs = (op->rtlen - 1) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ipasswd_count) */

int ipasswd_countindex(ipasswd *op) noex {
	int		rs ;
	if ((rs = ipasswd_magic(op)) >= 0) {
	    rs = op->rilen ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ipasswd_countindex) */

/* fetch an entry by a real-name key lookup */
int ipasswd_fetch(ipasswd *op,realname *rp,ipasswd_cur *curp,
		int opts,char *up) noex {
	int		rs ;
	int		rs1 ;
	int		ul = 0 ;
	if ((rs = ipasswd_magic(op,rp,curp,up)) >= 0) {
            ipasswd_cur     cur ;
            time_t          dt = 0 ;
            bool	    f_cur = false ;
            if (curp == nullptr) {
                curp = &cur ;
                curp->magic = IPASSWD_CURMAGIC ;
                curp->wi = 0 ;
                for (int i = 0 ; i < IPASSWD_NINDICES ; i += 1) {
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
                cint        ns = NSHIFT ;
                int         hv, hi, ri, ui ;
                int         wi ;
                int         hl ;
                cchar       *hp ;
                cchar       *cp ;
                char        hbuf[4 + 1] ;
    /* continue with regular fetch activities */
                op->f.cursoracc = true ;    /* does not hurt if no cursor! */
    /* which index do we want to use? */
                wi = -1 ;
                if (rs >= 0) {
                    if ((rp->len.first >= 1) && (rp->len.last >= 3)) {
			if_constexpr (f_usefl3) {
                            wi = index_fl3 ;
                            hp = hbuf ;
                            hl = 4 ;
                            hbuf[0] = rp->first[0] ;
                            strncpy((hbuf + 1),rp->last,3) ;
			} else {
                            wi = index_l3 ;
                            hp = rp->last ;
                            hl = 3 ;
			} /* end if_constexpr (f_usefl3) */
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
                            int     c = 0 ;
                            bool    f{} ;
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
                        if (hi == 0) break ;
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
                        ul = intconv(cp - up) ;
                    } else {
                        ul = lenstr(op->stab + ui) ;
                    }
                    /* update cursor */
                    if (f_cur) {
                        curp->i[wi] = hi ;
                    }
                } /* end if (got one) */
                rs1 = ipasswd_enterend(op,dt) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (ipasswd-enter) */
	} /* end if (magic) */
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (ipasswd_fetch) */

int ipasswd_curbegin(ipasswd *op,ipasswd_cur *curp) noex {
	int		rs ;
	if ((rs = ipasswd_magic(op,curp)) >= 0) {
	    op->f.cursor = true ;
	    op->f.cursoracc = false ;
	    for (int i = 0 ; i < IPASSWD_NINDICES ; i += 1) {
	        curp->i[i] = -1 ;
	    }
	    curp->magic = IPASSWD_CURMAGIC ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ipasswd_curbegin) */

int ipasswd_curend(ipasswd *op,ipasswd_cur *curp) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = ipasswd_magic(op,curp)) >= 0) {
	    if (curp->magic == IPASSWD_CURMAGIC) {
	        if (op->f.cursoracc) op->ti_access = dt ;
	        op->f.cursor = false ;
	        for (int i = 0 ; i < IPASSWD_NINDICES ; i += 1) {
	            curp->i[i] = -1 ;
	        }
	        curp->magic = 0 ;
	    } /* end if (magic) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ipasswd_curend) */

int ipasswd_curenum(ipasswd *op,ipasswd_cur *curp,char *ubuf,cc **sa,
		char *rbuf,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		ul = 0 ;
	if ((rs = ipasswd_magic(op,curp,ubuf,sa,rbuf)) >= 0) {
	    if (curp->magic == IPASSWD_CURMAGIC) {
		rs = SR_INVALID ;
	        if (op->f.cursor) {
		    time_t	dt = 0 ;
		    int		ri = (curp->i[0] < 1) ? 1 : (curp->i[0] + 1) ;
		    /* capture a hold on the file */
		    if ((rs = ipasswd_enterbegin(op,dt)) >= 0) {
	                if (ri < op->rtlen) {
	                    if (sa != nullptr) {
	                        storeitem	sio ;
	                        dstr		ss[5] ;
	                        int		si ;
			            /* first */
	                        si = op->rectab[ri].first ;
	                        ss[0].sbuf = charp(op->stab + si) ;
	                        ss[0].slen = -1 ;
			            /* middle-1 */
	                        si = op->rectab[ri].m1 ;
	                        ss[1].sbuf = charp(op->stab + si) ;
	                        ss[1].slen = -1 ;
			            /* middle-2 */
	                        si = op->rectab[ri].m2 ;
	                        ss[2].sbuf = charp(op->stab + si) ;
	                        ss[2].slen = -1 ;
			            /* middle-3 */
	                        ss[3].sbuf = nullptr ;
	                        ss[3].slen = 0 ;
			            /* last */
	                        si = op->rectab[ri].last ;
	                        ss[4].sbuf = charp(op->stab + si) ;
	                        ss[4].slen = -1 ;
			            /* done */
				if ((rs = SI_STA(&sio,rbuf,rlen)) >= 0) {
	                	    int		c = 0 ;
	                	    for (int j = 0 ; j < 5 ; j += 1) {
	                    	        cchar	*sp = ss[j].sbuf ;
	                    	        cint	sl = ss[j].slen ;
	                    	        if (sp && (sl != 0) && (*sp != '\0')) {
					    cint	ssl = ss[j].slen ;
					    cchar	*ssp = ss[j].sbuf ;
	                        	    cchar	**spp = (sa+c++) ;
	                        	    rs = SI_STR(&sio,ssp,ssl,spp) ;
	                    	        }
			    	        if (rs < 0) break ;
	                	    } /* end for */
	                	    sa[c] = nullptr ;
	                	    rs1 = SI_FIN(&sio) ;
	                	    if (rs >= 0) rs = rs1 ;
	            	        } /* end if (storeitem) */
	        	    } /* end if */
	                    if (rs >= 0) {
	                        cint	ui = op->rectab[ri].username ;
				cchar	*cp ;
	                        if (ubuf != nullptr) {
			            cint	ulen = IPASSWD_USERNAMELEN ;
	                            cp = strwcpy(ubuf,(op->stab + ui),ulen) ;
	                            ul = intconv(cp - ubuf) ;
	                        } else {
	                            ul = lenstr(op->stab + ui) ;
	                        }
				/* update the cursor */
	            		curp->i[0] = ri ;
	        	    } /* end if (ok) */
	    		} else {
	        	    rs = SR_NOTFOUND ;
	    		} /* end if (valid) */
	    		rs1 = ipasswd_enterend(op,dt) ;
	    		if (rs >= 0) rs = rs1 ;
		    } /* end if (ipasswd-enter) */
	        } /* end if (valid) */
	    } /* end if (magic) */
	} /* end if (magic) */
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (ipasswd_curenum) */

int ipasswd_curfetch(ipasswd *op,ipasswd_cur *curp,int opts,char *ubuf,
		cchar **sa,int sn) noex {
	ipasswd_cur	cur ;
	time_t		dt = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		ul = 0 ;
	bool		f_cur = false ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != IPASSWD_MAGIC) return SR_NOTOPEN ;

	if (sa == nullptr) return SR_FAULT ;

	if (curp == nullptr) {
	    curp = &cur ;
	    curp->magic = IPASSWD_CURMAGIC ;
	    for (int i = 0 ; i < IPASSWD_NINDICES ; i += 1) curp->i[i] = -1 ;
	} else {
	    f_cur = true ;
	    if (curp->magic != IPASSWD_CURMAGIC) return SR_NOTOPEN ;
	    if (! op->f.cursor) return SR_INVALID ;
	}

	if (ubuf) ubuf[0] = '\0' ;

/* do we have a hold on the file? */

	if ((rs = ipasswd_enterbegin(op,dt)) >= 0) {
	    cint	ns = NSHIFT ;
	    int		hv, hi, ri, ui ;
	    int		wi ;
	    int		hl, c ;
	    cchar	*hp ;
	    cchar	*cp ;
	    char	hbuf[4 + 1] ;

/* continue with regular fetch activities */

	    op->f.cursoracc = true ;	/* does not hurt if no cursor! */

	    if (rs >= 0) {
	        realname	rn, *rp = &rn ;
	        if ((rs = rp->start(sa,sn)) >= 0) {
	            wi = -1 ;
	            if ((rp->len.first >= 1) && (rp->len.last >= 3)) {
			if_constexpr (f_usefl3) {
	                    wi = index_fl3 ;
	                    hp = hbuf ;
	                    hl = 4 ;
	                    hbuf[0] = rp->first[0] ;
	                    strncpy((hbuf + 1),rp->last,3) ;
			} else {
	                    wi = index_l3 ;
	                    hp = rp->last ;
	                    hl = 3 ;
			} /* end if_constexpr (f_usefl3) */
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

/* OK, we go from here */

	            if (rs >= 0) {

	                if (curp->i[wi] < 0) {

	                    hv = hash_elf(hp,hl) ;

	                    hi = hashindex(hv,op->rilen) ;

/* start searching! */

	                    if (op->ropts & IPASSWD_ROSEC) {
	                        int	f ;

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
	                    if (hi == 0) break ;

	                    op->collisions += 1 ;

	                } /* end while */

	                if ((ri == 0) || (hi == 0)) {
	                    rs = SR_NOTFOUND ;
	                }

	            } /* end if (following the existing chain) */

/* if successful, retrieve username */

	            if (rs >= 0) {

	                ui = op->rectab[ri].username ;
	                if (ubuf != nullptr) {
			    cint	ulen = IPASSWD_USERNAMELEN ;
	                    cp = strwcpy(ubuf,(op->stab + ui),ulen) ;
	                    ul = intconv(cp - ubuf) ;
	                } else {
	                    ul = lenstr(op->stab + ui) ;
	                }

/* update cursor */

	                if (f_cur) {
	                    curp->i[wi] = hi ;
	                }

	            } /* end if (got one) */
	            rs1 = rp->finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (realname) */
	    } /* end if (ok) */

	    rs1 = ipasswd_enterend(op,dt) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ipasswd-enter) */

	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (ipasswd_curfetch) */

int ipasswd_getinfo(ipasswd *op,ipasswd_info *rp) noex {
	int		rs ;
	if ((rs = ipasswd_magic(op,rp)) >= 0) {
	    if (rp) {
	        rp->collisions = op->collisions ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ipasswd_info) */

int ipasswd_check(ipasswd *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = ipasswd_magic(op)) >= 0) {
	    if ((! op->f.cursor) && (op->fd >= 0)) {
	        f = f || ((dt - op->ti_access) >= TO_ACCESS) ;
	        f = f || ((dt - op->ti_open) >= TO_OPEN) ;
	        if (f) {
	            if ((rs = ipasswd_mapend(op)) >= 0) {
	                rs = ipasswd_fileclose(op) ;
	            }
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ipasswd_check) */


/* private subroutines */

static int ipasswd_hdrload(ipasswd *op) noex {
	int		rs = SR_OK ;
	cchar		*cp = ccharp(op->mapdata) ;
	if (hasValidMagic(cp,PWIHDR_MAGICSIZE,PWIHDR_MAGICSTR)) {
	    cint	*table = intp(op->mapdata + IPASSWD_IDLEN) ;
	    const uchar	*vetu = (const uchar *) (cp + 16) ;
	    if (vetu[0] == PWIHDR_VERSION) {
	        if (vetu[1] == ENDIAN) {
	            int		i ; /* used-afterwards */
	            op->ropts = vetu[2] ;
		    /* if looks good, read the header stuff */
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
	return rs ;
}
/* end subroutine (ipasswd_hdrload) */

static int ipasswd_hdrloader(ipasswd *op) noex {
	uint		*table = (uint *) (op->mapdata + IPASSWD_IDLEN) ;
	int		rs = SR_OK ;
	/* extract the header table values */
	op->rectab = (IPASSWD_ENT *) (op->mapdata + table[pwihdr_rectab]) ;
	op->rtlen = table[pwihdr_reclen] ;
	op->rtsize = table[pwihdr_recsize] ;
	op->stab = ccharp(op->mapdata + table[pwihdr_strtab]) ;
	op->stcount = table[pwihdr_lenstr] ;
	op->stsize = table[pwihdr_strsize] ;
	op->rilen = table[pwihdr_idxlen] ;
	op->recind[0] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxl1]) ;
	op->recind[1] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxl3]) ;
	op->recind[2] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxf]) ;
	op->recind[3] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxfl3]) ;
	op->recind[4] = (uint (*)[2]) (op->mapdata + table[pwihdr_idxun]) ;
	return rs ;
}
/* end subroutine (ipasswd_hdrloader) */

static int ipasswd_enterbegin(ipasswd *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	op->f.held = true ;
	if ((rs = ipaswd_mapcheck(op,dt)) > 0) {
	    f = true ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ipasswd_enterbegin) */

static int ipasswd_enterend(ipasswd *op,time_t dt) noex {
	int		rs = SR_OK ;
	(void) dt ;
	if (op->f.held) {
	    op->f.held = false ;
	}
	return rs ;
}
/* end subroutine (ipasswd_enterend) */

static int ipasswd_fileopen(ipasswd *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    if ((rs = uc_open(op->fname,op->oflags,op->operm)) >= 0) {
	        op->fd = rs ;
	        if (dt == 0) dt = getustime ;
	        op->ti_open = dt ;
	        if ((uc_closeonexec(op->fd,true)) >= 0) {
	            USTAT	sb ;
	            if ((rs = u_fstat(op->fd,&sb)) >= 0) {
			csize	fsz = size_t(sb.st_size) ;
	                int	sz = 0 ;
	                sz += IPASSWD_IDLEN ;
	                sz += (pwihdr_overlast * szof(int)) ;
	                if (int isz = intsat(fsz) ; isz >= sz) {
	                    op->mtime = sb.st_mtime ;
	                    op->filesize = intsat(sb.st_size) ;
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

static int ipasswd_fileclose(ipasswd *op) noex {
	int		rs = SR_OK ;
	if (op->fd >= 0) {
	    rs = u_close(op->fd) ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (ipasswd_fileclose) */

static int ipasswd_mapbegin(ipasswd *op,time_t dt) noex {
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

static int ipasswd_mapend(ipasswd *op) noex {
	int		rs = SR_OK ;
	if (op->mapdata) {
	    void	*ma = voidp(op->mapdata) ;
	    csize	ms = op->mapsize ;
	    rs = u_munmap(ma,ms) ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	}
	return rs ;
}
/* end subroutine (ipasswd_mapend) */

static int ipasswd_remotefs(ipasswd *op) noex {
	int		rs ;
	int		f = false ;
	if ((rs = isfsremote(op->fd)) > 0) {
	    f = true ;
	    op->f.remote = !!f ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ipasswd_remotefs) */

static int ipasswd_keymatchfl3(ipasswd *op,int opts,int ri,realname *rp) noex {
	int		si = op->rectab[ri].last ;
	int		f ;
	(void) opts ;
	f = (strncmp((op->stab + si),rp->last,3) == 0) ;
	if (f) {
	    si = op->rectab[ri].first ;
	    f = ((op->stab + si)[0] == rp->first[0]) ;
	}
	return f ;
}
/* end subroutine (ipasswd_keymatchfl3) */

static int ipasswd_keymatchl3(ipasswd *op,int opts,int ri,realname *rp) noex {
	cint		si = op->rectab[ri].last ;
	int		f ;
	(void) opts ;
	f = (strncmp((op->stab + si),rp->last,3) == 0) ;
	return f ;
}
/* end subroutine (ipasswd_keymatchl3) */

static int ipasswd_keymatchl1(ipasswd *op,int opts,int ri,realname *rp) noex {
	cint		si = op->rectab[ri].last ;
	int		f ;
	(void) opts ;
	f = ((op->stab + si)[0] == rp->last[0]) ;
	return f ;
}
/* end subroutine (ipasswd_keymatchl1) */

static int ipasswd_keymatchf(ipasswd *op,int opts,int ri,realname *rp) noex {
	cint		si = op->rectab[ri].first ;
	int		f ;
	(void) opts ;
	f = ((op->stab + si)[0] == rp->first[0]) ;
	return f ;
}
/* end subroutine (ipasswd_keymatchf) */

#ifdef	COMMENT
static int ipasswd_keymatchlast(ipasswd *op,int opts,int ri,
		char *sp,int hl) noex {
	cint		si = op->rectab[ri].last ;
	int		f ;
	if (opts & IPASSWD_FOLASTFULL) {
	    f = (strncmp((op->stab + si),sp,3) == 0) ;
	} else {
	    f = (strncmp((op->stab + si),sp,hl) == 0) ;
	}
	return f ;
}
/* end subroutine (ipasswd_keymatchlast) */

#endif /* COMMENT */

static int ipasswd_keymatchall(ipasswd *op,int opts,int ri,realname *rp) noex {
	int		si ;
	int		f = true ;
	cchar		*s = op->stab ;
	cchar		*sp ;
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

static int ipaswd_mapcheck(ipasswd *op,time_t dt) noex {
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
	cchar		*suf = IPASSWD_SUF ;
	cchar		*endstr = ENDIANSTR ;
	if (detOurSuf(suf,dbname,-1) > 0) {
	    rs = mkpath1(dbfname,dbname) ;
	} else {
	    rs = mkfnamesuf2(dbfname,dbname,suf,endstr) ;
	}
	return rs ;
}
/* end subroutine (mkourfname) */

static int detOurSuf(cchar *suf,cchar *fname,int fl) noex {
	int		len = 0 ;
	int		cl ;
	cchar		*cp ;
	if ((cl = sfbasename(fname,fl,&cp)) > 0) {
	    cchar	*tp ;
	    if ((tp = strnrchr(cp,cl,'.')) != nullptr) {
	        cint	suflen = lenstr(suf) ;
	        if (strncmp((tp+1),suf,suflen) == 0) {
	            len = intconv(tp - fname) ;
	        }
	    }
	}
	return len ;
}
/* end subroutine (detOurSuf) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) {
	    var.usernamelen = rs ;
	}
	return rs ;
}
/* end subroutine (mkvars) */


