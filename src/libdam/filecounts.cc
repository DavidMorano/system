/* filecounts SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage a file-based counter database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	filecounts

	Description:
	This module is a manager for a counter data-base maintained
	in a file.

	Synopsis:
	int filecounts_open(FC *op,cchar *fname) noex
	int filecounts_close(FC *op) noex
	int filecounts_process(FC *op,FC_N *list) noex

	= Notes
	Different processing occurs for each variable depending on how
	the variable 'value' in a list-item is set:
	value		action
	-1		retrieve value only (do not add if not present)
	0		create as necessary and retrieve value only
	1		create as necessary, retreive value, and increment
	<v>		create as necessary, and set to value <v>

	Format of file records:
	<count> <date> <name> 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<linebuffer.h>
#include	<filer.h>
#include	<lockfile.h>
#include	<vecobj.h>
#include	<storebuf.h>
#include	<dater.h>
#include	<snx.h>
#include	<rmx.h>
#include	<cfdec.h>
#include	<ctdec.h>
#include	<nleadstr.h>
#include	<timestr.h>
#include	<satarith.h>
#include	<char.h>
#include	<localmisc.h>		/* |DIGBUFLEN| + |TIMEBUFLEN| */

#include	"filecounts.h"

import libutil ;

/* local defines */

#define	FC		filecounts
#define	FC_N		filecounts_n
#define	FC_II		filecounts_ii
#define	FC_I		filecounts_info
#define	FC_CUR		filecounts_cur

#define	WORKER		worker
#define	WORKER_ENT	worker_ent
#define	WORKER_CMDNUL	0
#define	WORKER_CMDINC	1
#define	WORKER_CMDSET	2

#define	DEFENTS	10

#define	TO_LOCK		4		/* seconds */

#define	UENTADDER	(FILECOUNTS_NUMDIGITS + 1 + FILECOUNTS_LOGZLEN + 1 + 1)


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct worker {
	FC_N		*nlp ;
	vecobj		wlist ;
	int		nremain ;	/* number of entries */
	int		vall ;
} ;

struct worker_ent {
	cchar		*name ;		/* name of DB entry */
	uint		ovalue ;	/* old-value */
	uint		avalue ;	/* action-value */
	int		eoff ;		/* offset in file */
	int		action ;	/* action to perform */
	int		ni ;		/* name-index */
} ;

namespace {
    struct vars {
	int		maxnamelen ;
	int		uentlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int filecounts_ctor(filecounts *op,Args ... args) noex {
    	FILECOUNTS	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filecounts_ctor) */

static int filecounts_dtor(filecounts *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filecounts_dtor) */

template<typename ... Args>
static inline int filecounts_magic(filecounts *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FILECOUNTS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (filecounts_magic) */

static int	filecounts_proclist(FC *,FC_N *) noex ;
static int	filecounts_scan(FC *,WORKER *,filer *) noex ;
static int	filecounts_update(FC *,WORKER *) noex ;
static int 	filecounts_fins(FC *,WORKER *) noex ;
static int	filecounts_procline(FC *,WORKER *,int,cchar *,int) noex ;
static int	filecounts_updateone(FC *,cchar *,WORKER_ENT *) noex ;
static int	filecounts_append(FC *,cchar *,WORKER_ENT *) noex ;
static int	filecounts_snaper(FC *,vecobj *) noex ;
static int	filecounts_snaperline(FC *,dater *,vecobj *,char *,int) noex ;
static int	filecounts_lockbegin(FC *) noex ;
static int	filecounts_lockend(FC *) noex ;

static int	worker_start(WORKER *,FC_N *) noex ;
static int	worker_match(WORKER *,cchar *,int) noex ;
static int	worker_record(WORKER *,int,int,uint) noex ;
static int	worker_remaining(WORKER *) noex ;
static int	worker_sort(WORKER *) noex ;
static int	worker_get(WORKER *,int,WORKER_ENT **) noex ;
static int	worker_finish(WORKER *) noex ;

static int	mkentry(char *,int,uint,int,cchar *,int,cchar *) noex ;
static int	actioncmd(int) noex ;

static int	cmpoff(WORKER_ENT *,WORKER_ENT *) noex ;

extern "C" {
    static int	vcmpoff(cvoid **,cvoid **) noex ;
}


/* local variables */

static vars		var ;

constexpr int		diglen = DIGBUFLEN ;

constexpr char		total[] = "TOTAL" ;


/* exported variables */


/* exported subroutines */

int filecounts_open(FC *op,cchar *fn,int of,mode_t om) noex {
	int		rs ;
	if ((rs = filecounts_ctor(op,fn)) >= 0) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        op->fl.rdonly = ((of & O_ACCMODE) == O_RDONLY) ;
	        if ((rs = u_open(fn,of,om)) >= 0) {
	            op->fd = rs ;
	            if (cchar *cp{} ; (rs = uc_mallocstrw(fn,-1,&cp)) >= 0) {
		        op->fname = cp ;
	                op->magic = FILECOUNTS_MAGIC ;
	            } /* end if (memory-allocation) */
	            if (rs < 0) {
	                u_close(op->fd) ;
	                op->fd = -1 ;
	            }
	        } /* end if (open) */
	    } /* end if (vars) */
	    if (rs < 0) {
		filecounts_dtor(op) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filecounts_open) */

int filecounts_close(FC *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = filecounts_magic(op)) >= 0) {
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    if (op->fd >= 0) {
	        rs1 = u_close(op->fd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fd = -1 ;
	    }
	    {
	        rs1 = filecounts_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filecounts_close) */

int filecounts_process(FC *op,FC_N *nlp) noex {
	int		rs ;
	if ((rs = filecounts_magic(op,nlp)) >= 0) {
	    rs = filecounts_proclist(op,nlp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filecounts_process) */

int filecounts_curbegin(FC *op,FC_CUR *curp) noex {
	int		rs ;
	if ((rs = filecounts_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    curp->magic = FILECOUNTS_MAGIC ;
	    op->ncursors += 1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filecounts_curbegin) */

int filecounts_curend(FC *op,FC_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = filecounts_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == FILECOUNTS_MAGIC) {
	        if ((curp->listn > 0) && (curp->listp != nullptr)) {
	            FC_II	*rlp = curp->listp ;
	            for (int i = 0 ; i < curp->listn ; i += 1) {
	                if (rlp[i].name != nullptr) {
	                    rs1 = uc_free(rlp[i].name) ;
	                    if (rs >= 0) rs = rs1 ;
	                }
	            } /* end for */
	            curp->listn = 0 ;
	        } /* end if */
	        if (curp->listp != nullptr) {
	            rs1 = uc_free(curp->listp) ;
	            if (rs >= 0) rs = rs1 ;
	            curp->listp = nullptr ;
	        }
	        op->ncursors -= 1 ;
	        curp->i = 0 ;
	        curp->magic = 0 ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (filecounts_curend) */

/* take a "snapshot" of all of the counters */
int filecounts_cursnap(FC *op,FC_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = filecounts_magic(op,curp)) >= 0) {
	    rs = SR_BADSLOT ;
	    if (op->ncursors > 0) {
	        cint	iisz = szof(FC_II) ;
	        cint	to = TO_LOCK ;
	        int	vn = DEFENTS ;
	        int	vo = 0 ;
	        if (vecobj tlist ; (rs = tlist.start(iisz,vn,vo)) >= 0) {
	            if ((rs = lockfile(op->fd,F_RLOCK,0z,0z,to)) >= 0) {
		        {
	                    rs = filecounts_snaper(op,&tlist) ;
		        }
	                rs1 = lockfile(op->fd,F_ULOCK,0z,0z,0) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (locked-DB) */
	            if ((rs >= 0) && (rs = tlist.count) >= 0) {
	                cint	sz = ((rs + 1) * iisz) ;
	                if (char *cp{} ; (rs = uc_malloc(sz,&cp)) >= 0) {
	                    FC_II	*ilist = (FC_II *) cp ;
		            void	*vp{} ;
		            auto	vog = vecobj_get ;
	                    for (int i = 0 ; vog(&tlist,i,&vp) >= 0 ; i += 1) {
	                        FC_II	*ep = (FC_II *) vp ;
	                        if (vp) {
	                            ilist[c++] = *ep ;
	                        }
	                    } /* end for */
	                    ilist[c].name = nullptr ;
	                    ilist[c].utime = 0 ;
	                    ilist[c].value = -1 ;
	                    curp->listp = ilist ;
	                    curp->listn = c ;
	                } /* end if (memory allocation) */
	            } /* end if (ok) */
	            rs1 = tlist.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (tlist) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (filecounts_cursnap) */

/* read (get info on) a counter by name */
int filecounts_curread(FC *op,FC_CUR *curp,FC_I *fcip,
		char *nbuf,int nlen) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = filecounts_magic(op,curp,fcip,nbuf)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == FILECOUNTS_MAGIC) {
	        int	ei = (curp->i >= 0) ? curp->i : 0 ;
		rs = SR_OK ;
	        if (ei < curp->listn) {
	            cchar	*sp = curp->listp[ei].name ;
	            fcip->utime = curp->listp[ei].utime ;
	            fcip->value = curp->listp[ei].value ;
	            nbuf[0] = '\0' ;
	            if (sp) {
	                rs = sncpy(nbuf,nlen,sp) ;
	                rl = rs ;
	            }
	            if (rs >= 0) {
	                curp->i = (ei + 1) ;
	            }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (filecounts_curread) */


/* private subroutines */

static int filecounts_proclist(FC *op,FC_N *nlp) noex {
	int		rs ;
	int		rs1 ;
	int		vo = 0 ;
	if (WORKER work ; (rs = worker_start(&work,nlp)) >= 0) {
	    if ((rs = filecounts_lockbegin(op)) >= 0) {
		cint	fd = op->fd ;
	        if (filer fb ; (rs = fb.start(fd,0z,0,vo)) >= 0) {
	            if ((rs = filecounts_scan(op,&work,&fb)) >= 0) {
	                if (! op->fl.rdonly) {
	                    rs = filecounts_update(op,&work) ;
	                } /* end if */
	                if (rs >= 0) {
	                    rs = filecounts_fins(op,&work) ;
			}
	            } /* end if (search) */
	            rs1 = fb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (filer) */
	        rs1 = filecounts_lockend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (locked-DB) */
	    rs1 = worker_finish(&work) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (worker) */
	return rs ;
}
/* end subroutine (filecounts_proclist) */

static int filecounts_scan(FC *op,WORKER *wp,filer *fbp) noex {
	int		rs ;
	int		rs1 ;
	if (linebuffer lb ; (rs = lb.start) >= 0) {
	    cint	to = TO_LOCK ;
	    int		foff = 0 ;
	    int		rn = 1 ;
	    while ((rs = fbp->readln(lb.lbuf,lb.llen,to)) > 0) {
	        cint	len = rs ;
	        if ((rs = filecounts_procline(op,wp,foff,lb.lbuf,len)) >= 0) {
	            foff += len ;
	            rn = worker_remaining(wp) ;
	        }
	        if (rn <= 0) break ;
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = lb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (linebuffer) */
	return rs ;
}
/* end subroutine (filecounts_scan) */

static int filecounts_procline(FC *op,WORKER *wp,int eo,
		cchar *lbuf,int len) noex {
	cint		llen = FILECOUNTS_NUMDIGITS ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && wp) {
	    cint si = (FILECOUNTS_NUMDIGITS + 1 + FILECOUNTS_LOGZLEN + 1) ;
	    rs = SR_OK ;
	    /* calulate the "skip" index */
	    if (len >= (si+1)) {
	        cchar	*sp = (lbuf + si) ;
	        cint	sl = rmeol((lbuf + si),(len - si)) ;
	        /* we should be at the counter name within the entry */
	        if (uint v{} ; (rs1 = cfdecui(lbuf,llen,&v)) >= 0) {
	            wp->vall += v ;
	            if ((rs1 = worker_match(wp,sp,sl)) >= 0) {
	                cint	ei = rs1 ;
	                rs = worker_record(wp,ei,eo,v) ;
	            } else if (rs1 != SR_NOTFOUND) {
	                rs = rs1 ;
	            }
	        } /* end if (valid number) */
	    } /* end if (valid entry) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filecounts_procline) */

static int filecounts_update(FC *op,WORKER *wp) noex {
	custime		dt = getustime ;
	int		rs ;
	int		c = 0 ;
	char		tbuf[TIMEBUFLEN + 1] ;
	/* create the time-string to put in the DB file */
	timestr_logz(dt,tbuf) ;
	/* sort the entries by offset (w/ new ones at the rear) */
	if ((rs = worker_sort(wp)) >= 0) {
	    FC_N	*nlp = wp->nlp ;
	    WORKER_ENT	*wep{} ;
	    for (int i = 0 ; worker_get(wp,i,&wep) >= 0 ; i += 1) {
	        if (wep) {
	            if (wep->action >= 0) {
	                if (wep->eoff >= 0) {
	                    if (wep->action > 0) {
	                        c += 1 ;
	                        rs = filecounts_updateone(op,tbuf,wep) ;
	                    }
	                } else {
	                    c += 1 ;
	                    if (strcmp(wep->name,"TOTAL") == 0) {
	                        cint	ni = wep->ni ;
	                        wep->action = WORKER_CMDSET ;
	                        nlp[ni].value += wp->vall ;
	                    }
	                    rs = filecounts_append(op,tbuf,wep) ;
	                }
	            }
	        } /* end if (non-null) */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (worker_sort) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (filecounts_update) */

static int filecounts_updateone(FC *op,cchar *tbuf,WORKER_ENT *wep) noex {
    	cnullptr	np{} ;
	uint		nv = wep->ovalue ; /* default is same as old value */
	cint		na = wep->action ;
	cint		nd = FILECOUNTS_NUMDIGITS ;
	cint		tlen = FILECOUNTS_LOGZLEN ;
	int		rs = SR_OK ;
	int		ni = wep->ni ;
	int		wlen = 0 ;
	switch (na) {
	case WORKER_CMDINC:
	    if (ni >= 0) {
	        nv = addsat(wep->ovalue,wep->avalue) ;
	    } else {
	        nv = (wep->ovalue + 1) ;
	    }
	    break ;
	case WORKER_CMDSET:
	    nv = wep->avalue ;
	    break ;
	} /* end switch */
	if (char *ubuf{} ; (rs = uc_malloc(var.uentlen,&ubuf)) >= 0) {
	    cint	ulen = rs ;
	    if ((rs = mkentry(ubuf,ulen,nv,nd,tbuf,tlen,np)) >= 0) {
	        coff	uoff = wep->eoff ;
	        wlen = rs ;
	        rs = u_pwrite(op->fd,ubuf,wlen,uoff) ;
	    }
	    rs = rsfree(rs,ubuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filecounts_updateone) */

static int filecounts_append(FC *op,cchar *tbuf,WORKER_ENT *wep) noex {
	uint		nv = 0 ;
	cint		nvsz = FILECOUNTS_NUMDIGITS ;
	cint		tlen = FILECOUNTS_LOGZLEN ;
	cint		na = wep->action ;
	int		rs ;
	int		wlen = 0 ;
	switch (na) {
	case WORKER_CMDINC:
	    nv = (wep->avalue+1) ;
	    break ;
	case WORKER_CMDSET:
	    nv = wep->avalue ;
	    break ;
	} /* end switch */
	if (char *ubuf{} ; (rs = uc_malloc(var.uentlen,&ubuf)) >= 0) {
	    cint	ulen = rs ;
	    cchar	*nn = wep->name ;
	    if ((rs = mkentry(ubuf,ulen,nv,nvsz,tbuf,tlen,nn)) >= 0) {
	        cint	w = SEEK_END ;
	        wlen += rs ;
	        if (off_t eoff{} ; (rs = u_seeko(op->fd,0z,w,&eoff)) >= 0) {
	            rs = u_write(op->fd,ubuf,wlen) ;
	            wep->eoff = intconv(eoff) ;
	        }
	    } /* end if (mkentry) */
	    rs = rsfree(rs,ubuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filecounts_append) */

static int filecounts_fins(FC *op,WORKER *wp) noex {
	FC_N		*nlp = wp->nlp ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    WORKER_ENT	*wep{} ;
	    rs = SR_OK ;
	    for (int i = 0 ; (rs1 = worker_get(wp,i,&wep)) >= 0 ; i += 1) {
	        if (wep) {
	            if (wep->eoff < 0) {
	                cint	ni = wep->ni ;
	                if (ni >= 0) nlp[ni].value = -1 ;
	            }
	        }
	    } /* end for */
	    if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filecounts_fins) */

static int filecounts_snaper(FC *op,vecobj *ilp) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	if (dater dm ; (rs = dater_start(&dm,np,np,0)) >= 0) {
	    cint	fd = op->fd ;
	    if (linebuffer lb ; (rs = lb.start) >= 0) {
	        cint	vo = 0 ;
	        if (filer fb ; (rs = fb.start(fd,0z,0,vo)) >= 0) {
	            cint	to = -1 ;
	            cint	llen = lb.llen ;
	            char	*lbuf = lb.lbuf ;
	            while ((rs = fb.readln(lbuf,llen,to)) > 0) {
	                rs = filecounts_snaperline(op,&dm,ilp,lbuf,rs) ;
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = fb.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (filer) */
	        rs1 = lb.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (linebuffer) */
	    rs1 = dater_finish(&dm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dater) */
	return rs ;
}
/* end subroutine (filecounts_snaper) */

static int filecounts_snaperline(FC *op,dater *dmp,vecobj *ilp,
		char *lbuf,int llen) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint si = (FILECOUNTS_NUMDIGITS + 1 + FILECOUNTS_LOGZLEN + 1) ;
	    if (llen >= (si + 1)) {
	        int	sl = llen ;
	        int	cl ;
	        cchar	*cp ;
	        cchar	*sp = lbuf ;
	        if (sl && (sp[sl-1] == '\n')) {
	            sl -= 1 ;
	        }
	        while (sl && CHAR_ISWHITE(sp[sl-1])) /* EOL removal */ {
	            sl -= 1 ;
	        }
	        cp = sp ;
	        cl = FILECOUNTS_NUMDIGITS ;
	        if (uint v{} ; (rs = cfdecui(cp,cl,&v)) >= 0) {
	            {
	                sl -= intconv((cp + cl + 1) - sp) ;
	                sp = (cp + cl + 1) ;
	            }
	            cp = sp ;
	            cl = FILECOUNTS_LOGZLEN ;
	            if ((rs = dater_setlogz(dmp,cp,cl)) >= 0) {
	                time_t	utime ;
	                if ((rs = dater_gettime(dmp,&utime)) >= 0) {
		            {
	                        sl -= intconv((cp + cl + 1) - sp) ;
	                        sp = (cp + cl + 1) ;
		            }
		            {
	                        cp = sp ;
	                        cl = sl ;
		            }
			    auto ucmstr = uc_mallocstrw ;
	                    if (cc *nnp{} ; (rs = ucmstr(cp,cl,&nnp)) >= 0) {
	                        FC_II	ii{} ;
	                        ii.name = nnp ;
	                        ii.value = v ;
	                        ii.utime = utime ;
	                        rs = vecobj_add(ilp,&ii) ;
	                        if (rs < 0) {
			            uc_free(nnp) ;
			        }
	                    } /* end if (m-a) */
	                } /* end if (dater_gettime) */
	            } /* end if (dater_setlogz) */
	        } /* end if (cfdecui) */
	    } /* end if (zero or positive) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filecounts_snaperline) */

static int filecounts_lockbegin(FC *op) noex {
	cint		to = TO_LOCK ;
	cint		cmd = (op->fl.rdonly) ? F_RLOCK : F_WLOCK ;
	return lockfile(op->fd,cmd,0z,0z,to) ;
}
/* end subroutine (filecounts_lockbegin) */

static int filecounts_lockend(FC *op) noex {
	cint		cmd = F_ULOCK ;
	return lockfile(op->fd,cmd,0z,0z,0) ;
}
/* end subroutine (filecounts_lockend) */

static int worker_start(WORKER *wp,FC_N *nlp) noex  {
	cint		wesz = szof(WORKER_ENT) ;
	cint		vn = DEFENTS ;
	int		rs ;
	int		vo = vecobjm.compact ;
	memclear(wp) ;
	wp->nlp = nlp ;
	if ((rs = vecobj_start(&wp->wlist,wesz,vn,vo)) >= 0) {
	    WORKER_ENT	we{} ;
	    int		i ; /* used-afterwards */
	    int		na ;
	    int		vadding = 0 ;
	    int		ti = -1 ;
	    cchar	*tnp = total ;
	    for (i = 0 ; nlp[i].name != nullptr ; i += 1) {
	        cchar	*sp = nlp[i].name ;
	        if ((sp[0] != 'T') || (strcmp(sp,tnp) != 0)) {
	            cint	nv = nlp[i].value ;
	            na = actioncmd(nv) ;
	            if (na == WORKER_CMDINC) vadding += nv ;
	            we.eoff = -1 ;
	            we.ni = i ;
	            we.name = sp ;
	            we.action = na ;
	            we.avalue = nv ;
	            rs = vecobj_add(&wp->wlist,&we) ;
	        } else {
	            ti = i ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	    if (rs >= 0) {
		we = {} ;
	        we.eoff = -1 ;
	        we.ni = ti ;
	        we.name = tnp ;
	        we.action =  WORKER_CMDINC ;
	        we.avalue = vadding ;
	        rs = vecobj_add(&wp->wlist,&we) ;
	        wp->nremain = (i+1) ; /* number of entries */
	    } /* end if (ok) */
	    if (rs < 0) {
	        vecobj_finish(&wp->wlist) ;
	    }
	} /* end if (vecobj_start) */
	return rs ;
}
/* end subroutine (worker_start) */

static int worker_finish(WORKER *wp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vecobj_finish(&wp->wlist) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (worker_finish) */

static int worker_match(WORKER *wp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		i ; /* used-afterwards */
	void		*vp{} ;
	for (i = 0 ; (rs = vecobj_get(&wp->wlist,i,&vp)) >= 0 ; i += 1) {
	    WORKER_ENT	*wep = (WORKER_ENT *) vp ;
	    if (vp) {
	        cint	m = nleadstr(wep->name,sp,sl) ;
	        if ((m > 0) && (wep->name[m] == '\0')) break ;
	    }
	} /* end for */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (worker_match) */

static int worker_remaining(WORKER *wp) noex {
	return wp->nremain ;
}
/* end subroutine (worker_remaining) */

static int worker_record(WORKER *wp,int ei,int eoff,uint v) noex {
    	vecobj		*wlp = &wp->wlist ;
	FC_N		*nlp = wp->nlp ;
	int		rs ;
	if (void *vp{} ; (rs = wlp->get(ei,&vp)) >= 0) {
	    WORKER_ENT	*wep = (WORKER_ENT *) vp ;
	    if (vp) {
	        cint	ni = wep->ni ;
	        if (wp->nremain > 0) {
		    wp->nremain -= 1 ;
		}
	        wep->eoff = eoff ;
	        wep->ovalue = v ;
	        if (ni >= 0) {
		    nlp[ni].value = v ; /* return previous value */
	        }
	    } /* end if (non-null) */
	} /* end if (vecobj_get) */
	return rs ;
}
/* end subroutine (worker_record) */

static int worker_sort(WORKER *wp) noex {
    	vecobj		*wlp = &wp->wlist ;
	return wlp->sort(vcmpoff) ;
}
/* end subroutine (worker_sort) */

static int worker_get(WORKER *wp,int i,WORKER_ENT **rpp) noex {
    	int		rs = SR_FAULT ;
	if (wp && rpp) {
    	    vecobj	*wlp = &wp->wlist ;
	    if (void *vp{} ; (rs = wlp->get(i,&vp)) >= 0) {
		*rpp = (WORKER_ENT *) vp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (worker_get) */

/* make either a partial (update) or full DB entry */
static int mkentry(char *rbuf,int rlen,uint nv,int nvs,
		cchar *tb,int ts,cc *name) noex {
	int		rs ;
	int		idx = 0 ;
	if (char digbuf[diglen + 1] ; (rs = ctdecui(digbuf,diglen,nv)) >= 0) {
	    storebuf	sb(rbuf,rlen) ;
	    int		n = rs ;
	    int		nr ;
	    if (rs >= 0) {
	        nr = max((nvs - n),0) ;
	        rs = sb.blanks(nr) ;
	    }
	    if (rs >= 0) {
	        rs = sb.strw(digbuf,n) ;
	    }
	    if (rs >= 0) {
	        rs = sb.chr(' ') ;
	    }
	    if (rs >= 0) {
	        rs = sb.strw(tb,ts) ;
	        n = rs ;
	    }
	    if (rs >= 0) {
	        nr = max((ts - n),0) ;
	        rs = sb.blanks(nr) ;
	    }
	    if ((rs >= 0) && name) {
	        if (rs >= 0) {
	            rs = sb.chr(' ') ;
	        }
	        if (rs >= 0) {
	            rs = sb.strw(name) ;
	        }
	    } /* end if (name) */
	    if (rs >= 0) {
	        rs = sb.chr('\n') ;
	    }
	    idx = sb.idx ;
	} /* end if (ctdecui) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (mkentry) */

static int actioncmd(int nv) noex {
	int		na = 0 ;
	switch (nv) {
	case 1:
	    na = WORKER_CMDINC ;
	    break ;
	case 2:
	    na = WORKER_CMDSET ;
	    break ;
	} /* end switch */
	return na ;
}
/* end subroutine (actioncmd) */

static int cmpoff(WORKER_ENT *e1p,WORKER_ENT *e2p) noex {
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
		    rc = 0 ;
	            if ((e2p->eoff >= 0) || (e1p->eoff >= 0)) {
	                if ((rc = (e1p->eoff < 0) ? 1 : 0) == 0) {
	    	            if ((rc = (e2p->eoff < 0) ? -1 : 0) == 0) {
	    	                rc = (e1p->eoff - e2p->eoff) ;
		            }
	                }
	            }
	        }
	    }
	}
	return rc ;
}
/* end subroutine (cmpoff) */

static int vcmpoff(cvoid **v1pp,cvoid **v2pp) noex {
	WORKER_ENT	*e1p = (WORKER_ENT *) *v1pp ;
	WORKER_ENT	*e2p = (WORKER_ENT *) *v2pp ;
	return cmpoff(e1p,e2p) ;
}
/* end subroutine (vcmpoff) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	    maxnamelen = rs ;
	    uentlen = (rs + UENTADDER) ;
	}
	return rs ;
}
/* end method (vars::operator) */


