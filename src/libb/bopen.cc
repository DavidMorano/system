/* bopen SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1	/* debugging */
#define	CF_MAPABLE	0	/* allow mapped files */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

	= 1999-01-10, David A­D­ Morano
	I added the little extra code to allow for memory mapped
	I-O.  It is all a waste because it manual memory mapping
	is less performatice than the regular pre-reading done by
	the operating system itself.  This should teach me to leave
	old programs alone!

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	bfile

	Description:
	This code piece provides for the basic "open" and "close"
	functions for the BFILE I-O library.

	The following global functions are made available from this
	code piece:

	- bopen
	- bopene
	- bopenmod
	- bclose

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<sysval.hh>		/* LIBUC */
#include	<stdfnames.h>		/* LIBU */
#include	<snx.h>			/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<conallof.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<intceil.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"bfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	BOM_READ	(1<<0)
#define	BOM_WRITE	(1<<1)
#define	BOM_APPEND	(1<<2)
#define	BOM_FILEDESC	(1<<3)

#ifndef	CF_MAPABLE
#define	CF_MAPABLE	0	/* allow mapped files */
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0	/* debugging */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::mem ;			/* variable */


/* local typedefs */

typedef bfile_map *	maper ;


/* external subroutines */

extern "C" {
    extern int	findfilepath(cchar *,char *,cchar *,int) noex ;
}


/* external variables */


/* local structures */

namespace {
   struct sub_bopen ;
   struct sub_isreadonly {
	sub_bopen	*sop ;
	void operator () (sub_bopen *p) noex {
	   sop = p ;
	} ;
	operator bool () noex ;
   } ; /* end struct */
   struct sub_bopen {
	bfile		*op ;
	cchar		*fn ;
	cchar		*os ;
	mainv		argv = nullptr ;
	mainv		envv = nullptr ;
	int		to ;
	int		bsz = 0 ;
	sub_isreadonly	isreadonly ;
	sub_bopen(bfile *aop,cc *afn,cc *aos,mode_t aom,int ato) noex {
	    isreadonly(this) ;
	    op = aop ;
	    fn = afn ;
	    os = aos ;
	    aop->om = aom ;
	    to = ato ;
	} ; /* end ctor */
	int operator () (mainv av,mainv ev) noex ;
	int mkoflags	() noex ;
	int getfile	() noex ;
	int openfd	(int) noex ;
	int openreg	() noex ;
	int openadj	() noex ;
	int openoffset	() noex ;
	int bufsize	() noex ;
	int iclose	() noex ;
   } ; /* end struct (sub_bopen) */
} /* end namespace */


/* forward references */

local int	bfile_bufbegin	(bfile *,int) noex ;
local int	bfile_bufend	(bfile *) noex ;
local int	bfile_opts	(bfile *) noex ;
local int	bfile_mapbegin	(bfile *) noex ;
local int	bfile_mapend	(bfile *) noex ;


/* local variables */


static sysval		pagesz		(sysval_ps) ;
static bufsizevar	maxlinelen	(bufsize_ml) ;
constexpr bool		f_debug		= CF_DEBUG ;
constexpr bool		f_mapable	= CF_MAPABLE ;


/* exported variables */


/* exported subroutines */

int bopene(bfile *op,cchar *fn,cchar *os,mode_t om,int to) noex {
    	BFILE		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	DEBUGPRINTF("ent\n") ;
	if (op && fn && os) {
	    DEBUGPRINTF("fn=%s\n",fn) ;
	    memclear(hop) ;
	    op->fd = -1 ;
	    rs = SR_INVALID ;
	    if (fn[0]) {
		if (sub_bopen bo(op,fn,os,om,to) ; (rs = bo(np,np)) >= 0) {
		    op->magval = BFILE_MAGIC ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bopene) */

int bopen(bfile *op,cchar *fn,cchar *os,mode_t om) noex {
	return bopene(op,fn,os,om,-1) ;
} /* end subroutine (bopen) */

int bopenmod(bfile *fp,cchar *fname,cchar *of,mode_t om) noex {
	int		rs ;
	int		rv = 0 ;
	if ((rs = bopen(fp,fname,of,om)) >= 0) {
	    rv = rs ;
	    if (strchr(of,'M') == nullptr) { /* not already done! */
	        if ((rs = conallof(of,-1,"wc")) > 0) {
	            fp->of |= O_MINMODE ;
	            rs = uc_fminmod(fp->fd,om) ;
	        }
	    } /* end if (strchr) */
	    if (rs < 0) {
	        bclose(fp) ;
	    } /* end if (error) */
	} /* end if (bopen) */
	return (rs >= 0) ? rv : rs ;
}
/* end routine (bopenmod) */

int bclose(bfile *op) noex {
	int		rs ;
	int		rs1 ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = bfile_magic(op)) >= 0) {
	    rs = SR_OK ;
	DEBUGPRINTF("1 rs=%d\n",rs) ;
            if (op->fl.writing && (op->len > 0)) {
	        DEBUGPRINTF("flush\n") ;
                rs1 = bfile_flush(op) ;
                if (rs >= 0) rs = rs1 ;
            }
	DEBUGPRINTF("2 rs=%d\n",rs) ;
            if (op->maps) {
	        DEBUGPRINTF("mapend\n") ;
                rs1 = bfile_mapend(op) ;
                if (rs >= 0) rs = rs1 ;
            }
	DEBUGPRINTF("3 rs=%d\n",rs) ;
            if (op->bdata) {
	        DEBUGPRINTF("bufend\n") ;
                rs1 = bfile_bufend(op) ;
                if (rs >= 0) rs = rs1 ;
            }
	DEBUGPRINTF("4 rs=%d\n",rs) ;
            if (op->fd >= 0) {
	        DEBUGPRINTF("close\n") ;
                rs1 = uc_close(op->fd) ;
                if (rs >= 0) rs = rs1 ;
		op->fd = -1 ;
            }
            op->magval = 0 ;
	} /* end if (magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bclose) */


/* local subroutines */

int sub_bopen::operator () (mainv av,mainv ev) noex {
	int		rs ;
	argv = av ;
	envv = ev ;
	op->fd = -1 ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = mkoflags()) >= 0) {
	    DEBUGPRINTF("mkoflags() rs=%d\n",rs) ;
	    if ((rs = getfile()) > 0) {
	        DEBUGPRINTF("getfile() rs=%d\n",rs) ;
		if ((rs = bufsize()) >= 0) {
	    	    DEBUGPRINTF("bufsize() rs=%d\n",rs) ;
	            if ((rs = bfile_bufbegin(op,bsz)) >= 0) {
	    	        DEBUGPRINTF("bufbegin() rs=%d\n",rs) ;
			rs = bfile_mapbegin(op) ;
	    	        DEBUGPRINTF("mapbegin() rs=%d\n",rs) ;
		    } /* end if (bfile_bufbegin) */
		} /* end if (bufsize) */
		if (rs < 0) {
		    iclose() ;
		} /* end if (error) */
	    } /* end if (getfile) */
	} /* end if (mkoflags) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (sub_bopen::operator) */

int sub_bopen::getfile() noex {
	int		rs ;
	DEBUGPRINTF("ent fn=%s\n",fn) ;
	if ((rs = getfdfile(fn,-1)) >= 0) {	/* "standard" file */
	    DEBUGPRINTF("getfdfile() rs=%d\n",rs) ;
	    op->fl.filedesc = true ;
	    rs = openfd(rs) ;
	    DEBUGPRINTF("openfd() rs=%d\n",rs) ;
	} else if (rs == SR_EMPTY) {		/* "null" file */
	    DEBUGPRINTF("-EMPTY\n") ;
	    op->fl.nullfile = true ;
	    DEBUGPRINTF("nullfile\n") ;
	} else if (rs == SR_DOM) {
	    DEBUGPRINTF("-DOM\n") ;
	    rs = openreg() ;
	    DEBUGPRINTF("openreg() rs=%d\n",rs) ;
	} /* end if */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (sub_bopen::getfile) */

int sub_bopen::openfd(int idx) noex {
	int		rs ;
	int		fd = -1 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if ((rs = uc_dupmince(idx,BFILE_MINFD)) >= 0) {
	    op->fd = rs ;
	    fd = rs ;
	    if ((rs = openadj()) >= 0) {
		rs = openoffset() ;
	    } /* end if (openadj) */
	    if (rs < 0) {
		iclose() ;
	    } /* end if (error) */
	} /* end if (uc_dupmince) */
	DEBUGPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end method (sub_bopen::openfd) */

int sub_bopen::openreg() noex {
	cint		of = op->of ;
	int		rs ;
	int		fd = -1 ; /* return-value */
	cmode		om = op->om ;
	DEBUGPRINTF("ent fn=%s to=%d\n",fn,to) ;
	if ((rs = uc_opene(fn,of,om,to)) >= 0) {
	    DEBUGPRINTF("uc_opene() rs=%d\n",rs) ;
	    op->fd = rs ;
	    fd = rs ;
	    rs = bfile_opts(op) ;
	    DEBUGPRINTF("bfile_opts() rs=%d\n",rs) ;
	    if (rs < 0) {
		iclose() ;
	    } /* end if (error) */
	} /* end if (uc_opene) */
	DEBUGPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end method (sub_bopen::openreg) */

int sub_bopen::openadj() noex {
	int		rs ;
	if ((rs = uc_fcntl(op->fd,F_GETFL,0)) >= 0) {
	    cint	fl = rs ;
	    cint	of = op->of ;
	    int		nof = 0 ;
	    if ((fl & O_NDELAY) && (! (of & O_NDELAY))) {
		nof |= O_NDELAY ;
	    }
	    if ((fl & O_NONBLOCK) && (! (of & O_NONBLOCK))) {
		nof |= O_NONBLOCK ;
	    }
	    if ((fl & O_APPEND) && (! (of & O_APPEND))) {
	        nof |= O_APPEND ;
		op->fl.append = true ;
	    }
	    if (nof) {
		op->of = nof ;
	    }
	} /* end if (uc_fcntl) */
	return rs ;
} /* end method (sub_bopen::openadj) */

int sub_bopen::openoffset() noex {
	int		rs ;
	DEBUGPRINTF("ent\n") ;
	if (off_t fo{} ; (rs = uc_tell(op->fd,&fo)) >= 0) {
	    op->offset = size_t(fo) ;
	}
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (sub_bopen::openoffset) */

int sub_bopen::bufsize() noex {
	int		rs ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = pagesz) >= 0) {
	    op->pagesz = rs ;
	    if ((rs = maxlinelen) >= 0) {
	        cint	maxline = rs ;
	        if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
		    csize	fsize = size_t(sb.st_size) ;
	            bool	f = false ;
	            bool	f_notseek = true ;
	            op->fsize = 0 ;
	            op->fm = sb.st_mode ;	/* save file-mode */
	            op->ino = sb.st_ino ;
	            op->dev = sb.st_dev ;
	            f = f || S_ISREG(sb.st_mode) ;
	            f = f || S_ISDIR(sb.st_mode) ;
	            f = f || S_ISBLK(sb.st_mode) ;
	            if (f) {
	                if (isreadonly) {
	                    cint	ps = op->pagesz ;
	                    int		fs = intsat(fsize) ;
	                    int		cs ;
		            if (fs == 0) fs = 1 ;
	                    cs = ceil(fs,512) ;
		            bsz = min(cs,ps) ;
	                } /* end if (isreadonly) */
	                f_notseek = false ;
	            } else if (S_ISFIFO(sb.st_mode)) {
	                bsz = min(maxline,2048) ;
	                op->bm = bfilebm_line ;
	            } else if (S_ISCHR(sb.st_mode)) {
	                if (isatty(op->fd)) {
	                    bsz = min(maxline,2048) ;
	                    op->fl.terminal = true ;
	                    op->bm = bfilebm_line ;
	                } /* end if (is a terminal) */
	            } else if (S_ISSOCK(sb.st_mode)) {
	                op->fl.network = true ;
	                bsz = (64*1024) ;
	                op->bm = bfilebm_line ;
	            } /* end if */
		    op->fl.notseek = f_notseek ;
	        } /* end if (fstat) */
	    } /* end if (maxlinelen) */
	} /* end if (pagesize) */
	DEBUGPRINTF("ret rs=%d bsz=%d\n",rs,bsz) ;
	return rs ;
} /* end method (sub_bopen::bufsize) */

local int bfile_bufbegin(bfile *op,int bsz) noex {
	int		rs ;
	DEBUGPRINTF("ent bsz=%d\n",bsz) ;
	if (bsz == 0) bsz = op->pagesz ;
	if (char *p ; (rs = mem.mall(bsz,&p)) >= 0) {
	    op->bdata = p ;
	    op->bsz = bsz ;
	    op->bbp = p ;
	    op->bp = p ;
	} /* end if (memory-acquire) */
	DEBUGPRINTF("ret rs=%d bsz=%d\n",rs,bsz) ;
	return rs ;
} /* end subroutine (bfile_bufbegin) */

local int bfile_bufend(bfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->bdata) {
	    rs1 = mem.free(op->bdata) ;
	    if (rs >= 0) rs = rs1 ;
	    op->bdata = nullptr ;
	    op->bbp = nullptr ;
	    op->bp = nullptr ;
	    op->bsz = 0 ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (bfile_bufend) */

local int bfile_opts(bfile *op) noex {
	cint		of = op->of ;
	int		rs = SR_OK ;
	cmode		om = op->om ;
	if ((rs >= 0) & (of & O_MINMODE) && (om > 0)) {
	    rs = uc_fminmod(op->fd,om) ;
	}
	if ((rs >= 0) && (of & O_NETWORK)) {
	    op->fl.network = true ;
	}
	return rs ;
} /* end subroutine (bfile_opts) */

local int bfile_mapbegin(bfile *op) noex {
	cint		nm = BFILE_NMAPS ;
	int		rs = SR_OK ;
	if (op->fl.mapable) {
	    cint	sz = (nm * szof(bfile_map)) ;
	    if (void *vp ; (rs = mem.mall(sz,&vp)) >= 0) {
	        op->maps = maper(vp) ;
	        for (int i = 0 ; i < nm ; i += 1) {
	            op->maps[i].fl.valid = false ;
	            op->maps[i].bdata = nullptr ;
	        } /* end for */
	        op->bp = nullptr ;
	        op->fl.mapinit = true ;
	    } /* end if (m-a) */
	} /* end if (fl.mapable) */
	return rs ;
} /* end subroutine (bfile_mapbegin) */

local int bfile_mapend(bfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->maps) {
	    for (int i = 0 ; i < BFILE_NMAPS ; i += 1) {
	        if ((op->maps[i].fl.valid) && op->maps[i].bdata) {
		    void	*md = op->maps[i].bdata ;
		    csize	ms = op->maps[i].bsize ;
	            rs1 = u_mmapend(md,ms) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    {
	        rs1 = mem.free(op->maps) ;
	        if (rs >= 0) rs = rs1 ;
	        op->maps = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bfile_mapend) */

int sub_bopen::mkoflags() noex {
	int		rs = SR_OK ;
	int		of = O_CLOEXEC ;
	DEBUGPRINTF("ent\n") ;
	if (os) {
	    cchar	*osp = os ;
	    DEBUGPRINTF("os=%s\n",os) ;
	    for (int sc ; ((sc = mkchar(*osp))) ; osp += 1) {
	        switch (sc) {
	        case 'r':
	            op->fl.rd = true ;
	            break ;
	        case 'w':
		    op->fl.wr = true ;
	            break ;
	        case 'm':
	        case '+':
	            op->fl.rd = true ;
		    op->fl.wr = true ;
	            break ;
	        case 'a':
	            of |= O_APPEND ;
		    op->fl.append = true ;
	            break ;
	        case 'b': /* POSIX "binary" mode -- nothing on real UNIXes® */
	            break ;
	        case 'c':
	            of |= O_CREAT ;
	            break ;
	        case 'e':
	            of |= (O_CREAT | O_EXCL) ;
	            break ;
	        case 't':
	            of |= (O_CREAT | O_TRUNC) ;
	            break ;
	        case 'n':
	            of |= O_NDELAY ;
	            break ;
	        case 'p':
	            op->fl.program = true ;
	            break ;
	        case 'x':
	            of |= O_EXCL ;
		    break ;
	        case 'F':
		    of |= O_MINFD ;		/* minimum-file-descriptor */
		    break ;
	        case 'N':
	            of |= O_NETWORK ;	/* "network" file */
		    op->fl.network = true ;
		    break ;
	        case 'M':
	            of |= O_MINMODE ;	/* minimum file-permissions-mode */
		    break ;
	        } /* end switch */
	    } /* end for (open flags) */
	} /* end if (non-null) */
	if (op->fl.rd && op->fl.wr) {
	    of |= O_RDWR ;
	} else if (op->fl.wr) {
	    of |= O_WRONLY ;
	} else {
	    of |= O_RDONLY ;
	} /* end if */
	op->of = of ;
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (sub_bopen::mkoflags) */

int sub_bopen::iclose() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = uc_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
} /* end method (sub_bopen::iclose) */

sub_isreadonly::operator bool () noex {
	bfile		*op = sop->op ;
	return (op->fl.rd && (! op->fl.wr)) ;
} /* end method */


