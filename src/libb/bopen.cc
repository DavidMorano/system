/* bopen SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package */
/* version %I% last-modified %G% */

#define	CF_MAPPABLE	0	/* allow mapped files */

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
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<sysval.hh>
#include	<stdfnames.h>
#include	<getfdfile.h>
#include	<snx.h>
#include	<cfdec.h>
#include	<conallof.h>
#include	<intsat.h>
#include	<intceil.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"bfile.h"

import libutil ;

/* local defines */

#define	BOM_READ	(1<<0)
#define	BOM_WRITE	(1<<1)
#define	BOM_APPEND	(1<<2)
#define	BOM_FILEDESC	(1<<3)

#ifndef	CF_MAPPABLE
#define	CF_MAPPABLE	0	/* allow mapped files */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


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
   } ;
   struct sub_bopen {
	bfile		*op ;
	cchar		*fn ;
	cchar		*os ;
	mainv		argv = nullptr ;
	mainv		envv = nullptr ;
	int		to ;
	int		bsize = 0 ;
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
	int mkoflags() noex ;
	int getfile() noex ;
	int openfd(int) noex ;
	int openadj() noex ;
	int openoffset() noex ;
	int bufsize() noex ;
	int openreg() noex ;
	int iclose() noex ;
   } ;
}


/* forward references */

static int	bfile_bufbegin(bfile *,int) noex ;
static int	bfile_bufend(bfile *) noex ;
static int	bfile_opts(bfile *) noex ;
static int	bfile_mapbegin(bfile *) noex ;
static int	bfile_mapend(bfile *) noex ;


/* local variables */

constexpr bool		f_mappable = CF_MAPPABLE ;

static sysval		pagesize(sysval_ps) ;
static bufsizevar	maxlinelen(getbufsize_ml) ;


/* exported variables */


/* exported subroutines */

int bopene(bfile *op,cchar *fn,cchar *os,mode_t om,int to) noex {
	int		rs = SR_FAULT ;
	if (op && fn && os) {
	    cnullptr	np{} ;
	    memclear(op) ;
	    op->fd = -1 ;
	    rs = SR_INVALID ;
	    if (fn[0]) {
		sub_bopen	bo(op,fn,os,om,to) ;
		rs = bo(np,np) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bopene) */

int bopen(bfile *op,cchar *fn,cchar *os,mode_t om) noex {
	return bopene(op,fn,os,om,-1) ;
}
/* end subroutine (bopen) */

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
	    }
	    if (rs < 0) {
	        bclose(fp) ;
	    }
	} /* end if (bopen) */
	return (rs >= 0) ? rv : rs ;
}
/* end routine (bopenmod) */

int bclose(bfile *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bfile_magic(op)) >= 0) {
            if (op->fl.writing && (op->len > 0)) {
                rs1 = bfile_flush(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            if (op->maps) {
                rs1 = bfile_mapend(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            if (op->bdata) {
                rs1 = bfile_bufend(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            if (op->fd >= 0) {
                rs1 = uc_close(op->fd) ;
                if (rs >= 0) rs = rs1 ;
		op->fd = -1 ;
            }
            op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (bclose) */


/* local subroutines */

int sub_bopen::operator () (mainv av,mainv ev) noex {
	int		rs ;
	argv = av ;
	envv = ev ;
	op->fd = -1 ;
	if ((rs = mkoflags()) >= 0) {
	    if ((rs = getfile()) > 0) {
		if ((rs = bufsize()) >= 0) {
	            if ((rs = bfile_bufbegin(op,bsize)) >= 0) {
			rs = bfile_mapbegin(op) ;
		    }
		} /* end if (bufsize) */
		if (rs < 0) {
		    iclose() ;
		}
	    } /* end if (getfile) */
	} /* end if (mkoflags) */
	return rs ;
}
/* end method (sub_bopen::operator) */

int sub_bopen::getfile() noex {
	int		rs = SR_OK ;
	if ((rs = getfdfile(fn,-1)) >= 0) {	/* "standard" file */
	    op->fl.filedesc = true ;
	    rs = openfd(rs) ;
	} else if (rs == SR_EMPTY) {		/* "null" file */
	    op->fl.nullfile = true ;
	} else if (rs != SR_DOM) {
	    rs = openreg() ;
	}
	return rs ;
}
/* end method (sub_bopen::getfile) */

int sub_bopen::openfd(int idx) noex {
	int		rs ;
	if ((rs = uc_dupmince(idx,BFILE_MINFD)) >= 0) {
	    op->fd = rs ;
	    if ((rs = openadj()) >= 0) {
		rs = openoffset() ;
	    }
	    if (rs < 0) {
		iclose() ;
	    }
	} /* end if (uc_dupmin) */
	return rs ;
}
/* end method (sub_bopen::openfd) */

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
}
/* end method (sub_bopen::openadj) */

int sub_bopen::openoffset() noex {
	int		rs ;
	off_t		fo{} ;
	if ((rs = uc_tell(op->fd,&fo)) >= 0) {
	    op->offset = size_t(fo) ;
	}
	return rs ;
}
/* end method (sub_bopen::openoffset) */

int sub_bopen::bufsize() noex {
	int		rs ;
	if ((rs = pagesize) >= 0) {
	    op->pagesize = rs ;
	    if ((rs = maxlinelen) >= 0) {
	        cint	maxline = rs ;
	        USTAT	sb ;
	        if ((rs = u_fstat(op->fd,&sb)) >= 0) {
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
	                    cint	ps = op->pagesize ;
	                    int		fs = intsat(sb.st_size) ;
	                    int		cs ;
		            if (fs == 0) fs = 1 ;
	                    cs = ceil(fs,512) ;
		            bsize = min(cs,ps) ;
	                }
	                f_notseek = false ;
	            } else if (S_ISFIFO(sb.st_mode)) {
	                bsize = min(maxline,2048) ;
	                op->bm = bfilebm_line ;
	            } else if (S_ISCHR(sb.st_mode)) {
	                if (isatty(op->fd)) {
	                    bsize = min(maxline,2048) ;
	                    op->fl.terminal = true ;
	                    op->bm = bfilebm_line ;
	                } /* end if (is a terminal) */
	            } else if (S_ISSOCK(sb.st_mode)) {
	                op->fl.network = true ;
	                bsize = (64*1024) ;
	                op->bm = bfilebm_line ;
	            }
		    op->fl.notseek = f_notseek ;
	        } /* end if (fstat) */
	    } /* end if (maxlinelen) */
	} /* end if (pagesize) */
	return rs ;
}
/* end method (sub_bopen::bufsize) */

int sub_bopen::openreg() noex {
	cint		of = op->of ;
	int		rs ;
	cmode		om = op->om ;
	if ((rs = uc_opene(fn,of,om,to)) >= 0) {
	    op->fd = rs ;
	    rs = bfile_opts(op) ;
	    if (rs < 0) {
		iclose() ;
	    }
	}
	return rs ;
}
/* end method (sub_bopen::openreg) */

static int bfile_bufbegin(bfile *op,int bsize) noex {
	int		rs ;
	if (bsize == 0) bsize = op->pagesize ;
	if (char *p ; (rs = uc_malloc(bsize,&p)) >= 0) {
	    op->bdata = p ;
	    op->bsize = bsize ;
	    op->bbp = p ;
	    op->bp = p ;
	}
	return rs ;
}
/* end subroutine (bfile_bufbegin) */

static int bfile_bufend(bfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->bdata) {
	    rs1 = uc_free(op->bdata) ;
	    if (rs >= 0) rs = rs1 ;
	    op->bdata = nullptr ;
	    op->bbp = nullptr ;
	    op->bp = nullptr ;
	    op->bsize = 0 ;
	}
	return rs ;
}
/* end subroutine (bfile_bufend) */

static int bfile_opts(bfile *op) noex {
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
}
/* end subroutine (bfile_opts) */

static int bfile_mapbegin(bfile *op) noex {
	cint		nm = BFILE_NMAPS ;
	int		rs = SR_OK ;
	if (op->fl.mappable) {
	    cint	sz = (nm * sizeof(bfile_map)) ;
	    if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
	        op->maps = maper(vp) ;
	        for (int i = 0 ; i < nm ; i += 1) {
	            op->maps[i].fl.valid = false ;
	            op->maps[i].bdata = nullptr ;
	        } /* end for */
	        op->bp = nullptr ;
	        op->fl.mapinit = true ;
	    } /* end if (m-a) */
	} /* end if (f_mappable) */
	return rs ;
}
/* end subroutine (bfile_mapbegin) */

static int bfile_mapend(bfile *op) noex {
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
	        rs1 = uc_free(op->maps) ;
	        if (rs >= 0) rs = rs1 ;
	        op->maps = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfile_mapend) */

int sub_bopen::mkoflags() noex {
	int		rs = SR_OK ;
	int		of = O_CLOEXEC ;
	cchar		*osp = os ;
	while (*os) {
	    cint	sc = mkchar(*osp++) ;
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
	} /* end while (open flags) */
	if (op->fl.rd && op->fl.wr) {
	    of |= O_RDWR ;
	} else if (op->fl.wr) {
	    of |= O_WRONLY ;
	} else {
	    of |= O_RDONLY ;
	}
	op->of = of ;
	return rs ;
}
/* end method (sub_bopen::mkoflags) */

int sub_bopen::iclose() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = uc_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end method (sub_bopen::iclose) */

sub_isreadonly::operator bool () noex {
	bfile		*op = sop->op ;
	return (op->fl.rd && (! op->fl.wr)) ;
}


