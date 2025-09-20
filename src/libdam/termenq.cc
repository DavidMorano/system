/* termenq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage reading or writing of the TERMENQ database */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	termenq

  	Description:
	This code is used to access the TERMENQ database.  This
	database holds information that connects terminal lines
	with the terminal devices.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memcpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<intceil.h>
#include	<intfloor.h>
#include	<localmisc.h>

#include	"termenq.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	TERMENQ_INTOPEN		30	/* seconds */
#define	TERMENQ_INTCHECK	3	/* seconds */
#define	TERMENQ_NENTS		32

#define	TE		termenq
#define	TE_ENT		termenq_ent
#define	TE_CUR		termenq_cur


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */


/* forward references */

template<typename ... Args>
static int termenq_ctor(termenq *op,Args ... args) noex {
    	TERMENQ		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termenq_ctor) */

static int termenq_dtor(termenq *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termenq_dtor) */

template<typename ... Args>
static inline int termenq_magic(termenq *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TERMENQ_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (termenq_magic) */

static int	termenq_initstuff(TE *,int) noex ;
static int	termenq_filesz(TE *,time_t = 0L) noex ;
static int	termenq_fileopen(TE *,time_t = 0L) noex ;
static int	termenq_fileclose(TE *) noex ;
static int	termenq_mapents(TE *,int,TE_ENT **) noex ;
static int	termenq_mapper(TE *,int,uint,uint) noex ;

static bool	isproctype(int) noex ;


/* local variables */

constexpr int		proctypes[] = {
	TERMENT_TLOGINPROC,
	TERMENT_TUSERPROC,
	TERMENT_TDEADPROC,
	-1
} ;


/* exported variables */


/* exported subroutines */

int termenq_open(TE *op,cchar *dbfn,int oflags) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = termenq_ctor(op,dbfn)) >= 0) {
	    if ((rs = ucpagesize) >= 0) {
	        op->pagesz = rs ;
	        op->fd = -1 ;
	        op->fname = nullptr ;
		if ((rs = termenq_initstuff(op,oflags)) >= 0) {
                    if (cchar *cp ; (rs = libmem.strw(dbfn,-1,&cp)) >= 0) {
                        op->fname = cp ;
                        if ((rs = termenq_fileopen(op,dt)) >= 0) {
                            op->ti_check = dt ;
                            if (USTAT sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
                                op->fsize = size_t(sb.st_size & INT_MAX) ;
                                op->ti_mod = sb.st_mtime ;
                                op->magic = TERMENQ_MAGIC ;
                            } /* end if (stat) */
                            if (rs < 0) {
                                termenq_fileclose(op) ;
                            }
                        } /* end if (termenq-fileopen) */
                        if (rs < 0) {
			    void *vp = voidp(op->fname) ;
                            libmem.free(vp) ;
                            op->fname = nullptr ;
                        }
                    } /* end if (memory-allocation) */
		} /* end if (termenq_initstuff) */
	    } /* end if (pagesize) */
	    if (rs < 0) {
		termenq_dtor(op) ;
	    }
	} /* end if (termenq_ctor) */
	return rs ;
}
/* end subroutine (termenq_open) */

int termenq_close(TE *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = termenq_magic(op)) >= 0) {
	    if ((op->mapdata != nullptr) && (op->mapsize > 0)) {
	        rs1 = u_mmapend(op->mapdata,op->mapsize) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fd >= 0) {
	        rs1 = termenq_fileclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
		void *vp = voidp(op->fname) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (termenq_close) */

int termenq_write(TE *op,int ei,TE_ENT *ep) noex {
	int		rs ;
	if ((rs = termenq_magic(op,ep)) >= 0) {
	    cint	am = (op->oflags & O_ACCMODE) ;
	    if ((am == SR_WRONLY) || (am == O_RDWR)) {
	        cint	esz = szof(TE_ENT) ;
	        if (op->fd < 0) {
	            rs = termenq_fileopen(op,0L) ;
	        }
	        if (rs >= 0) {
	            coff	poff = off_t(ei * esz) ;
	            rs = u_pwrite(op->fd,ep,esz,poff) ;
	        }
	    } else {
	        rs = SR_BADF ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (termenq_write) */

int termenq_check(TE *op,time_t dt) noex {
	int		rs ;
	int		fch = false ;
	if ((rs = termenq_magic(op)) >= 0) {
	    cint	intcheck = TERMENQ_INTCHECK ;
	    cint	intopen = TERMENQ_INTOPEN ;
	    if (dt == 0) dt = getustime ;
	    if (op->ncursors >= 0) {
	        if (op->fd >= 0) {
	            bool	f = ((dt - op->ti_check) < intcheck) ;
	            op->ti_check = dt ;
	            if (! f) {
	                bool f_close = ((dt - op->ti_open) >= intopen) ;
	                if (! f_close) {
	                    if (USTAT sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
		                csize	fsize = size_t(sb.st_size) ;
	                        fch = (fsize != op->fsize) ;
	                        fch = fch || (sb.st_mtime > op->ti_mod) ;
	                        op->fsize = fsize ;
	                        op->ti_mod = sb.st_mtime ;
	                    } else {
	                        f_close = true ;
	                    }
	                } /* end if */
	                if (f_close) {
	                    u_close(op->fd) ;
	                    op->fd = -1 ;
	                }
	            } /* end if */
	        } /* end if (good-FD) */
	    } /* end if (cursor-available) */
	} /* end if (magic) */
	return (rs >= 0) ? fch : rs ;
}
/* end subroutine (termenq_check) */

int termenq_curbegin(TE *op,TE_CUR *curp) noex {
	int		rs ;
	if ((rs = termenq_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    if (op->ncursors == 0) {
	        if (op->fd < 0) {
	            custime	dt = getustime ;
	            rs = termenq_filesz(op,dt) ;
	        } /* end if (opened the file) */
	    }
	    if (rs >= 0) {
	        op->ncursors += 1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (termenq_curbegin) */

int termenq_curend(TE *op,TE_CUR *curp) noex {
    	int		rs ;
	if ((rs = termenq_magic(op,curp)) >= 0) {
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (termenq_curend) */

int termenq_curenum(TE *op,TE_CUR *curp,TE_ENT *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = termenq_magic(op,curp)) >= 0) {
	    ei = (curp->i < 0) ? 0 : (curp->i + 1) ;
	    if (TE_ENT *bp ; (rs = termenq_mapents(op,ei,&bp)) >= 0) {
		cint	n = rs ;
	        if ((n > 0) && bp) {
	            if (ep) {
	        	memcpy(ep,bp,sizeof(TE_ENT)) ;
	    	    }
	            curp->i = ei ;
	        } else {
	            rs = SR_EOF ;
		}
	    }
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (termenq_curenum) */

int termenq_fetchsid(TE *op,TE_ENT *ep,pid_t pid) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = termenq_magic(op)) >= 0) {
	    if (op->ncursors == 0) {
	        rs = termenq_filesz(op,0L) ;
	    }
	    if (rs >= 0) {
	        TE_ENT	*up = nullptr ;
	        bool	f = false ;
	        while ((rs = termenq_mapents(op,ei,&up)) > 0) {
	            cint	n = rs ;
	            for (int i = 0 ; i < n ; i += 1) {
	                f = (up->type == TERMENT_TUSERPROC) ;
	                f = f && (up->sid == pid) ;
	                if (f) break ;
	                ei += 1 ;
	                up += 1 ;
	            } /* end for */
	            if (f) break ;
	        } /* end while */
	        if ((rs >= 0) && f && ep && up) {
	            memcpy(ep,up,sizeof(TE_ENT)) ;
	        }
	        if ((rs == SR_OK) && (! f)) {
	            rs = SR_SEARCH ;
	        }
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (termenq_fetchsid) */

int termenq_curfetchln(TE *op,TE_CUR *curp,TE_ENT *ep,cchar *name) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = termenq_magic(op,curp,name)) >= 0) {
	    ei = (curp->i < 0) ? 0 : (curp->i + 1) ;
	    if (op->ncursors == 0) {
	        rs = termenq_filesz(op,0L) ;
	    }
	    if (rs >= 0) {
	        TE_ENT		*up ;
	        cint		esz = szof(TE_ENT) ;
	        bool		f = false ;
	        while ((rs = termenq_mapents(op,ei,&up)) > 0) {
		    cint	lline = TERMENT_LLINE ;
	            cint	n = rs ;
	            for (int i = 0 ; i < n ; i += 1) {
	                f = isproctype(up->type) ;
	                f = f && (strncmp(name,up->line,lline) == 0) ;
	                if (f) break ;
	                ei += 1 ;
	                up += 1 ;
	            } /* end for */
	            if (f) break ;
	        } /* end while */
	        if ((rs >= 0) && f && ep) {
	            memcpy(ep,up,esz) ;
	        }
	        if ((rs == SR_OK) && (! f)) {
	            rs = SR_NOTFOUND ;
	        }
	        if ((rs >= 0) && curp) {
	            curp->i = ei ;
	        }
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (termenq_fetchline) */

int termenq_read(TE *op,int ei,TE_ENT *ep) noex {
	int		rs = SR_OK ;
	int		f = false ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TERMENQ_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAGE */

	if (ei < 0) return SR_INVALID ;

	if (op->ncursors == 0) {
	    rs = termenq_filesz(op,0L) ;
	}

	if (rs >= 0) {
	    cint	esz = szof(TE_ENT) ;
	    if (TE_ENT *up ; (rs = termenq_mapents(op,ei,&up)) > 0) {
		if (up) {
		    f = true ;
	            if (ep) {
	                memcpy(ep,up,esz) ;
		    }
		} /* end if (non-null) */
	    } /* end if */
	} /* end if */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (termenq_read) */

int termenq_nactive(TE *op) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = termenq_magic(op)) >= 0) {
	    if (op->ncursors == 0) {
	        rs = termenq_filesz(op,0L) ;
	    }
	    if (rs >= 0) {
	        TE_ENT	*ep ;
	        int	ei = 0 ;
		int	i ; /* used-afterwards */
	        bool	f ;
	        while ((rs = termenq_mapents(op,ei,&ep)) > 0) {
	            n = rs ;
	            for (i = 0 ; i < n ; i += 1) {
	                f = (ep->type == TERMENT_TUSERPROC) ;
	                f = f && (ep->line[0] != '.') ;
	                if (f) n += 1 ;
	                ep += 1 ;
	            } /* end for */
	            ei += i ;
	        } /* end while */
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (termenq_nactive) */


/* private subroutines */

static int termenq_initstuff(TE *op,int of) noex {
    	int		rs = SR_OK ;
        int		am = (of & O_ACCMODE) ;
	op->oflags = of ;
        switch (am) {
            break ;
        case O_WRONLY:
            am = O_RDWR ;
            op->oflags = ((of & (~ O_ACCMODE)) | am) ;
            break ;
        case O_RDONLY:
        case O_RDWR:
            break ;
        default:
            rs = SR_INVALID ;
            break ;
        } /* end switch */
	op->fl.writable = ((am == O_WRONLY) || (am == O_RDWR)) ;
	return rs ;
}
/* end subroutine (termenq_initstuff) */

static int termenq_filesz(TE *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    rs = termenq_fileopen(op,dt) ;
	}
	if (rs >= 0) {
	    if (USTAT sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	        op->ti_mod = sb.st_mtime ;
	        op->fsize = sb.st_size ;
	    }
	}
	return rs ;
}
/* end subroutine (termenq_filesz) */

static int termenq_fileopen(TE *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    if ((rs = u_open(op->fname,op->oflags,0660)) >= 0) {
	        op->fd = rs ;
	        if ((rs = uc_closeonexec(op->fd,true)) >= 0) {
	            if (dt == 0) dt = getustime ;
	            op->ti_open = dt ;
	        }
		if (rs < 0) {
		    u_close(op->fd) ;
		    op->fd = -1 ;
		}
	    }
	} /* end if (open) */
	return rs ;
}
/* end subroutine (termenq_fileopen) */

static int termenq_fileclose(TE *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	} /* end if (was open) */
	return rs ;
}
/* end subroutine (termenq_fileclose) */

static int termenq_mapents(TE *op,int ei,TE_ENT **rpp) noex {
	cint		esz = szof(TE_ENT) ;
	int		en ;
	int		rs = SR_OK ;
	int		n = 0 ;

	{
	    csize	esize = size_t(esz) ;
	    cint	nents = int((op->fsize / esize) + 1) ;
	    en = min(nents,TERMENQ_NENTS) ;
	}

	if (en != 0) {

	if (op->mapdata) {
	    if ((ei >= op->mapei) && (ei < (op->mapei + op->mapen))) {
	        n = ((op->mapei + op->mapen) - ei) ;
	    } /* end if */
	} /* end if (old map was sufficient) */

	if ((rs >= 0) && (n == 0)) {
	    uint	fsz = uint(op->fsize) ;
	    uint	eoff = (ei * esz) ;
	    uint	elen = (en * esz) ;
	    uint	eext = (eoff + elen) ;

	    if (eext > fsz) eext = fsz ;

	    n = (eext - eoff) / esz ;

	    if (n > 0) {
	        uint	wsize ;
	        uint	woff = ufloor(eoff,op->pagesz) ;
	        uint	wext = uceil(eext,op->pagesz) ;
	        wsize = (wext - woff) ;
	        if (wsize > 0) {
	            bool	f = (woff < op->mapoff) ;
	            f = f || (woff >= (op->mapoff + op->mapsize)) ;
	            if (! f) {
	                f = (((op->mapoff + op->mapsize) - eoff) < esz) ;
	            }
	            if (f) {
	                rs = termenq_mapper(op,ei,woff,wsize) ;
	                n = rs ;
	            } /* end if */
	        } else {
	            n = 0 ;
		}
	    } /* end if (need a map) */
	} /* end if (new map needed) */

	if (rpp) {
	    caddr_t	ep = nullptr ;
	    if ((rs >= 0) && (n > 0)) {
	        ep = (((ei * esz) - op->mapoff) + op->mapdata) ;
	    }
	    *rpp = (TE_ENT *) ep ;
	} /* end if */

	} /* end if (non-equal-zero) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (termenq_mapents) */

static int termenq_mapper(TE *op,int ei,uint woff,uint wsize) noex {
    	cnullptr	np{} ;
	coff		mo = off_t(woff) ;
	csize		ms = size_t(wsize) ;
	uint		eoff ;
	uint		eext ;
	uint		e ;
	cint		esz = szof(TE_ENT) ;
	cint		fd = op->fd ;
	cint		mp = PROT_READ ;
	cint		mf = MAP_SHARED ;
	int		rs = SR_OK ;
	int		n = 0 ;
	if (op->mapdata) {
	    u_mmapend(op->mapdata,op->mapsize) ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	    op->mapen = 0 ;
	}
	if (void *md ; (rs = u_mmapbegin(np,ms,mp,mf,fd,mo,&md)) >= 0) {
	    const caddr_t	ma = caddr_t(md) ;
	    cint		madv = MADV_SEQUENTIAL ;
	    if ((rs = u_madvise(ma,ms,madv)) >= 0) {
		{
	            op->mapdata = caddr_t(md) ;
	            op->mapsize = ms ;
	            op->mapoff = uint(mo) ;
		}
		{
		    csize	famount = size_t(woff + wsize) ;
	            eoff = uceil(woff,esz) ;
	            e = uint(min(famount,op->fsize)) ;
	            eext = ufloor(e,esz) ;
		}
		{
	            op->mapei = eoff / esz ;
	            op->mapen = (eext - eoff) / esz ;
		}
	        if (ei >= op->mapei) {
	            n = ((op->mapei + op->mapen) - ei) ;
		}
	    } /* end if (madvise) */
	} /* end if (mapped) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (termenq_mapper) */

static bool isproctype(int type) noex {
	bool		f = false ;
	for (int i = 0 ; proctypes[i] >= 0 ; i += 1) {
	    f = (type == proctypes[i]) ;
	    if (f) break ;
	} /* end for */
	return f ;
}
/* end subroutine (isproctype) */


