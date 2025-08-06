/* tmpx_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage access of the [UW]TMP database */
/* version %I% last-modified %G% */

#define	CF_DYNENTS	1		/* dynamic entries per read */
#define	CF_UTMPACC	1		/* use UTMPACC */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine module was adopted for use from some previous
	code that performed similar sorts of functions.

	= 2025-01-09, David A­D­ Morano
	I changed the code slightly (maybe that is debatable) to
	only retrieved records from the UTMPACC facility rather
	than from the raw file (as before).  Accessing the raw file
	was always really non-portable, so I convinved myself to
	make this facility object (TMPX) portable by using UTMPACC,
	which itself eventually makes calls into the system-supplied
	subroutine for UTMPX access.  I may enhance this in the
	future, but we have gone now for at least 25 years (or
	longer, whatever was before this present code) and the need
	to do anything other than read some UTMPX entries (lile
	writing them) has not really been important.  The change
	was amazingly rather simple.  Just look at the boolen
	variable |f_utmpacc| variable and where it is used to see
	the quite small change of how to open a file for mapping.
	The function |tmpx_write()| will succed but the entry gord
	nowhere (to a TMP file that vanishes after use).

*/

/* Copyright © 2000,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	tmpx

	Description:
	This code is used to access the UTMPX (or WTMPX) database.
	Those databases constitute the login-time accounting
	information for the system.  The motivation for this object
	was to get a thread-safe access facility to the system UTMPX
	dataase.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<utmpacc.h>
#include	<getfdfile.h>
#include	<opentmp.h>
#include	<sysval.hh>
#include	<intfloor.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"tmpx.h"


/* local defines */

#define	TMPX_INTOPEN	30	/* seconds */
#define	TMPX_INTCHECK	3	/* seconds */
#define	TMPX_NENTS	32	/* default n-entries (most systems "small") */
#define	TMPX_MAPSIZE	(2 * 1024 * 1024)

#ifndef	CF_DYNENTS
#define	CF_DYNENTS	0		/* safety default */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int	tmpx_writable(tmpx *,int) noex ;
static int	tmpx_openbegin(tmpx *,cchar *) noex ;
static int	tmpx_openend(tmpx *) noex ;
static int	tmpx_filesize(tmpx *,time_t) noex ;
static int	tmpx_fileopen(tmpx *,time_t) noex ;
static int	tmpx_fileopener(tmpx *op) noex ;
static int	tmpx_fileclose(tmpx *) noex ;
static int	tmpx_mapents(tmpx *,int,int,tmpx_ent **) noex ;
static int	tmpx_mapper(tmpx *,int,uint,uint) noex ;

static int	isproctype(int) noex ;


/* local variables */

constexpr int		proctypes[] = {
	TMPX_TPROCINIT,
	TMPX_TPROCLOGIN,
	TMPX_TPROCUSER,
	TMPX_TPROCDEAD,
	-1
} ;

static sysval		pagesize(sysval_ps) ;

constexpr bool		f_dynents = CF_DYNENTS ;
constexpr bool		f_utmpacc = CF_UTMPACC ;


/* exported variables */


/* exported subroutines */

int tmpx_open(tmpx *op,cchar *dbfn,int oflags) noex {
	int		rs = SR_FAULT ;
	if ((dbfn == nullptr) || (dbfn[0] == '\0') || (dbfn[0] == '-')) {
	    dbfn = TMPX_DEFUTMP ;
	}
	if (op) {
	    op->fname = nullptr ;
	    op->mapdata = 0 ;
	    op->ti_open = 0 ;
	    op->ti_mod = 0 ;
	    op->ti_check = 0 ;
	    op->mapsize = 0 ;
	    op->fsize = 0 ;
	    op->fl = {} ;
	    op->mapoff = 0 ;
	    op->magic = 0 ;
	    op->oflags = oflags ;
	    op->operms = 0 ;
	    op->fd = -1 ;
	    op->ncursors = 0 ;
	    op->mapei = 0 ;
	    op->mapen = 0 ;
	    if ((rs = pagesize) >= 0) {
		op->pagesize = rs ;
	        if ((rs = tmpx_writable(op,oflags)) >= 0) {
	            rs = tmpx_openbegin(op,dbfn) ;
	        }
	    } /* end if (pagesize) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmpx_open) */

int tmpx_close(tmpx *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = tmpx_magic(op)) >= 0) {
	    if (op->mapdata && (op->mapsize > 0)) {
		rs1 = u_mmapend(op->mapdata,op->mapsize) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = tmpx_openend(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (tmpx_close) */

int tmpx_write(tmpx *op,int ei,tmpx_ent *ep) noex {
	int		rs ;
	if ((rs = tmpx_magic(op,ep)) >= 0) {
            cint    am = (op->oflags & O_ACCMODE) ;
            rs = SR_BADF ;
            if ((am == SR_WRONLY) || (am == O_RDWR)) {
                if (op->fd < 0) {
                    rs = tmpx_fileopen(op,0L) ;
                }
                if (rs >= 0) {
                    off_t   poff ;
                    cint    esz = TMPX_ENTSIZE ;
                    poff = (off_t) (ei * esz) ;
                    rs = u_pwrite(op->fd,ep,esz,poff) ;
                }
            } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (tmpx_write) */

int tmpx_check(tmpx *op,time_t dt) noex {
	int		rs ;
	int		f_ch = false ;
	if ((rs = tmpx_magic(op)) >= 0) {
            if ((op->ncursors == 0) && (op->fd >= 0)) {
                if (dt == 0) dt = getustime ;
                if ((dt - op->ti_check) >= TMPX_INTCHECK) {
                    op->ti_check = dt ;
                    if ((dt - op->ti_open) < TMPX_INTOPEN) {
                        if (USTAT sb ; (rs = uc_fstat(op->fd,&sb)) >= 0) {
                            csize   fs = sb.st_size ;
                            f_ch = (fs != op->fsize) ;
                            f_ch = f_ch || (sb.st_mtime > op->ti_mod) ;
                            op->fsize = sb.st_size ;
                            op->ti_mod = sb.st_mtime ;
                        } else {
                            uc_close(op->fd) ;
                            op->fd = -1 ;
                        }
                    } /* end if */
                } /* end if (check time-out) */
            } /* end if (possible) */
	} /* end if (magic) */
	return (rs >= 0) ? f_ch : rs ;
}
/* end subroutine (tmpx_check) */

int tmpx_curbegin(tmpx *op,tmpx_cur *curp) noex {
	int		rs ;
	if ((rs = tmpx_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    if (op->ncursors == 0) {
	        if (op->fd < 0) {
	            custime	dt = getustime ;
	            rs = tmpx_filesize(op,dt) ;
	        } /* end if (opened the file) */
	    }
	    if (rs >= 0) op->ncursors += 1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (tmpx_curbegin) */

int tmpx_curend(tmpx *op,tmpx_cur *curp) noex {
	int		rs ;
	if ((rs = tmpx_magic(op,curp)) >= 0) {
	    if (op->ncursors > 0) op->ncursors -= 1 ;
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (tmpx_curend) */

int tmpx_curenum(tmpx *op,tmpx_cur *curp,tmpx_ent *ep) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = tmpx_magic(op,curp,ep)) >= 0) {
            int		en = TMPX_NENTS ;
            int		n ;
            ei = (curp->i < 0) ? 0 : (curp->i + 1) ;
            if (tmpx_ent *bp{} ; (rs = tmpx_mapents(op,ei,en,&bp)) >= 0) {
                n = rs ;
                if ((n > 0) && bp) {
                    if (ep) {
                        memcpy(ep,bp,TMPX_ENTSIZE) ;
                    }
                    curp->i = ei ;
                }
            } else {
                rs = SR_EOF ;
            }
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (tmpx_curenum) */

int tmpx_fetchpid(tmpx *op,tmpx_ent *ep,pid_t pid) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = tmpx_magic(op,ep)) >= 0) {
            if (op->ncursors == 0) {
                rs = tmpx_filesize(op,0L) ;
            }
            if (rs >= 0) {
                tmpx_ent    *up ;
                cint        en = TMPX_NENTS ;
                bool        f = false ;
                while ((rs = tmpx_mapents(op,ei,en,&up)) > 0) {
                    cint    n = rs ;
                    for (int i = 0 ; i < n ; i += 1) {
                        f = (up->ut_type == TMPX_TPROCUSER) ;
                        f = f && (up->ut_pid == pid) ;
                        if (f)
                            break ;
                        ei += 1 ;
                        up += 1 ;
                    } /* end for */
                    if (f) break ;
                } /* end while */
                if ((rs >= 0) && f && ep) {
                    memcpy(ep,up,TMPX_ENTSIZE) ;
                }
                if ((rs == SR_OK) && (! f)) {
                    rs = SR_SEARCH ;
                }
            } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (tmpx_fetchpid) */

int tmpx_fetchuser(tmpx *op,tmpx_cur *curp,tmpx_ent *ep,cchar *name) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = tmpx_magic(op,curp,ep,name)) >= 0) {
            if (curp) {
                ei = (curp->i < 0) ? 0 : (curp->i + 1) ;
            }
            if (op->ncursors == 0) {
                rs = tmpx_filesize(op,0L) ;
            }
            if (rs >= 0) {
                tmpx_ent    *up ;
                cint        esz = TMPX_ENTSIZE ;
                cint        en = TMPX_NENTS ;
                bool        f = false ;
                while ((rs = tmpx_mapents(op,ei,en,&up)) > 0) {
                    cint    n = rs ;
                    for (int i = 0 ; i < n ; i += 1) {
                        cint        ul = TMPX_LUSER ;
                        f = isproctype(up->ut_type) ;
                        f = f && (strncmp(name,up->ut_user,ul) == 0) ;
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
                if ((rs >= 0) && (curp != nullptr)) {
                    curp->i = ei ;
                }
            } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (tmpx_fetchuser) */

int tmpx_read(tmpx *op,int ei,tmpx_ent *ep) noex {
	int		rs ;
	int		f = false ;
	if ((rs = tmpx_magic(op,ep)) >= 0) {
            rs = SR_INVALID ;
            if (ei >= 0) {
                rs = SR_OK ;
                if (op->ncursors == 0) {
                    rs = tmpx_filesize(op,0L) ;
                }
                if (rs >= 0) {
                    tmpx_ent        *up ;
                    cint            en = TMPX_NENTS ;
                    cint            esz = TMPX_ENTSIZE ;
                    if ((rs = tmpx_mapents(op,ei,en,&up)) > 0) {
                        if (up != nullptr) {
                            f = true ;
                            if (ep != nullptr)
                                memcpy(ep,up,esz) ;
                        }
                    }
                } /* end if */
            } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (tmpx_read) */

int tmpx_nusers(tmpx *op) noex {
	int		rs ;
	int		nusers = 0 ;
	if ((rs = tmpx_magic(op)) >= 0) {
            int     en ;
            if_constexpr (f_dynents) {
                cint        esz = TMPX_ENTSIZE ;
                en = intconv((op->fsize / esz) + 1) ;
            } else {
                en = TMPX_NENTS ;
            } /* end if_constexpr (f_dynents) */
            if (op->ncursors == 0) {
                rs = tmpx_filesize(op,0L) ;
            }
            if (rs >= 0) {
                tmpx_ent    *ep ;
                int         ei = 0 ;
                while ((rs = tmpx_mapents(op,ei,en,&ep)) > 0) {
                    cint    n = rs ;
                    int     i ;
                    for (i = 0 ; i < n ; i += 1) {
                        bool        f = (ep->ut_type == TMPX_TPROCUSER) ;
                        f = f && (ep->ut_user[0] != '.') ;
                        if (f) nusers += 1 ;
                        ep += 1 ;
                    } /* end for */
                    ei += i ;
                } /* end while */
            } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? nusers : rs ;
}
/* end subroutine (tmpx_nusers) */


/* private subroutines */

static int tmpx_writable(tmpx *op,int oflags) noex {
	int		rs = SR_OK ;
	int		amode = (oflags & O_ACCMODE) ;
	switch (amode) {
	    case O_RDONLY:
	        break ;
	    case O_WRONLY:
	        amode = O_RDWR ;
	        oflags = ((oflags & (~ O_ACCMODE)) | amode) ;
		break ;
	    case O_RDWR:
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	} /* end switch */
	op->fl.writable = ((amode == O_WRONLY) || (amode == O_RDWR)) ;
	return rs ;
}
/* end subroutine (tmpx_writable) */

static int tmpx_openbegin(tmpx *op,cchar *dbfn) noex {
	int		rs ;
	if (cchar *cp ; (rs = uc_mallocstrw(dbfn,-1,&cp)) >= 0) {
	    custime	dt = getustime ;
	    op->fname = cp ;
	    if ((rs = tmpx_fileopen(op,dt)) >= 0) {
		op->ti_check = dt ;
		if (USTAT sb ; (rs = uc_fstat(op->fd,&sb)) >= 0) {
	    	    op->fsize = size_t(sb.st_size & SIZE_MAX) ;
	    	    op->ti_mod = sb.st_mtime ;
		    op->magic = TMPX_MAGIC ;
		} /* end if (stat) */
		if (rs < 0) {
		    tmpx_fileclose(op) ;
		}
	    } /* end if (tmpx-fileopen) */
	    if (rs < 0) {
		uc_free(op->fname) ;
		op->fname = nullptr ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
} 
/* end subroutine (tmpx_openbegin) */

static int tmpx_openend(tmpx *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = tmpx_fileclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->fname) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}
	return rs ;
} 
/* end subroutine (tmpx_openend) */

static int tmpx_filesize(tmpx *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    rs = tmpx_fileopen(op,dt) ;
	}
	if (rs >= 0) {
	    if (USTAT sb ; (rs = uc_fstat(op->fd,&sb)) >= 0) {
	        op->ti_mod = sb.st_mtime ;
	        op->fsize = sb.st_size ;
	    }
	}
	return rs ;
}
/* end subroutine (tmpx_filesize) */

static int tmpx_fileopen(tmpx *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    if ((rs = tmpx_fileopener(op)) >= 0) {
	        if ((rs = uc_closeonexec(op->fd,true)) >= 0) {
	            if (dt == 0) dt = getustime ;
	            op->ti_open = dt ;
	        }
		if (rs < 0) {
		    uc_close(op->fd) ;
		    op->fd = -1 ;
		}
	    }
	} /* end if (open) */
	return rs ;
}
/* end subroutine (tmpx_fileopen) */

/* this is where I modified some code (David Morano, 2025) */
static int tmpx_fileopener(tmpx *op) noex {
    	cnullptr	np{} ;
	int		rs ;
	if_constexpr (f_utmpacc) { /* new code */
	    cint	of = (O_CLOEXEC | O_MINFD) ;
	    cmode	om = 0664 ;
	    if ((rs = opentmp(np,of,om)) >= 0) {
		cint	fd = rs ;
		if ((rs = utmpacc_extract(fd)) >= 0) { /* <- money shot! */
		    op->fd = fd ;
		}
		if (rs < 0) {
		    uc_close(fd) ;
		}
	    } /* end if (opentmp) */
	} else { /* old code (still here) */
	    if ((rs = getfdfile(op->fname,-1)) >= 0) {
	        cint	fd = rs ;
	        cint	fdmin = 10 ;
	        if ((rs = uc_duper(fd,fdmin)) >= 0) {
	            op->fd = rs ;
	        }
	    } else if ((rs == SR_DOM) || (rs == SR_BADF)) {
	        cint	of = op->oflags ;
	        cmode	om = 0660 ;
	        if ((rs = uc_open(op->fname,of,om)) >= 0) {
	            op->fd = rs ;
	        }
	    }
	} /* end if_constexpr (f_utmpacc) */
	return rs ;
}
/* end subroutine (tmpx_fileopener) */

static int tmpx_fileclose(tmpx *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = uc_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	} /* end if (was open) */
	return rs ;
}
/* end subroutine (tmpx_fileclose) */

static int tmpx_mapents(tmpx *op,int ei,int en,tmpx_ent **rpp) noex {
	cint		esz = TMPX_ENTSIZE ;
	int		rs = SR_OK ;
	int		n = 0 ;
	if (en != 0) {
	    if (en < 0) en = TMPX_NENTS ;
	    if (op->mapdata != nullptr) {
	        if ((ei >= op->mapei) && (ei < (op->mapei + op->mapen))) {
	            n = ((op->mapei + op->mapen) - ei) ;
	        } /* end if */
	    } /* end if (old map was sufficient) */
	    if ((rs >= 0) && (n == 0)) {
	        uint	eoff = (ei * esz) ;
	        uint	elen = (en * esz) ;
	        uint	eext = (eoff + elen) ;
	        if (eext > op->fsize) eext = intsat(op->fsize) ;
	        n = (eext - eoff) / esz ;
	        if (n > 0) {
	            cuint	woff = ufloor(eoff,op->pagesize) ;
	            cuint	wext = uceil(eext,op->pagesize) ;
	            cuint	wsize = (wext - woff) ;
	            if (wsize > 0) {
	                bool	f = (woff < op->mapoff) ;
	                f = f || (woff >= (op->mapoff + op->mapsize)) ;
	                if (! f) {
	                    f = (((op->mapoff + op->mapsize) - eoff) < esz) ;
	                }
	                if (f) {
	                    rs = tmpx_mapper(op,ei,woff,wsize) ;
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
	        *rpp = (tmpx_ent *) ep ;
	    }
	} /* end if (non-equal-zero) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (tmpx_mapents) */

static int tmpx_mapper(tmpx *op,int ei,uint woff,uint wsz) noex {
    	cnullptr	np{} ;
	csize		ms = size_t(wsz) ;
	coff		mo = off_t(woff) ;
	cuint		esz = TMPX_ENTSIZE ;
	cuint		fsz = uint(op->fsize) ;
	cint		mp = PROT_READ ;
	cint		mf = MAP_SHARED ;
	cint		fd = op->fd ;
	int		rs ;
	int		n = 0 ;
	if (op->mapdata) {
	    u_mmapend(op->mapdata,op->mapsize) ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	    op->mapen = 0 ;
	}
	if (void *md{} ; (rs = u_mmapbegin(np,ms,mp,mf,fd,mo,&md)) >= 0) {
	    cuint	eoff = uceil(woff,esz) ;
	    cuint	e = min((woff + wsz),fsz) ;
	    cint	madv = MADV_SEQUENTIAL ;
	    if ((rs = u_madvise(md,ms,madv)) >= 0) {
	        cuint	eext = ufloor(e,esz) ;
	        op->mapdata = caddr_t(md) ;
	        op->mapsize = ms ;
	        op->mapoff = woff ;
	        op->mapei = eoff / esz ;
	        op->mapen = (eext - eoff) / esz ;
	        if (ei >= op->mapei) {
	            n = ((op->mapei + op->mapen) - ei) ;
		}
	    } /* end if (madvise) */
	} /* end if (mapped) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (tmpx_mapper) */

static int isproctype(int type) noex {
	int		f = false ;
	for (int i = 0 ; proctypes[i] >= 0 ; i += 1) {
	    f = (type == proctypes[i]) ;
	    if (f) break ;
	} /* end for */
	return f ;
}
/* end subroutine (isproctype) */


