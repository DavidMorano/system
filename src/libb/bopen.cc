/* bopen SUPPORT */
/* lang=C++20 */

/* "Basic I-O" package */
/* version %I% last-modified %G% */

#define	CF_UNIXAPPEND	1	/* BRAINDAMAGED_IN_UNIX (on NFS mounted FSs) */
#define	CF_MAPABLE	0	/* allow mapped files */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

	= 1999-01-10, David A­D­ Morano
	I added the little extra code to allow for memory mapped
	I-O. It is all a waste because it is way slower than without
	it! This should teach me to leave old programs alone!!!

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This code piece provides for the basic "open" and "close"
	functions for the BFILE I-O library.

	The following global functions are made available from this
	code piece:

	- bopen
	- bopenprog
	- bclose

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<sysval.hh>
#include	<stdfnames.h>
#include	<getfdfile.h>
#include	<snx.h>
#include	<cfdec.h>
#include	<intceil.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#define	BOM_READ	(1<<0)
#define	BOM_WRITE	(1<<1)
#define	BOM_APPEND	(1<<2)
#define	BOM_FILEDESC	(1<<3)

#ifndef	CF_MAPABLE
#define	CF_MAPABLE	0	/* allow mapped files */
#endif


/* imported namespaces */


/* local typedefs */

typedef bfile_map *	maper ;


/* external subroutines */

extern "C" {
    extern int	findfilepath(cchar *,char *,cchar *,int) noex ;
}


/* external variables */


/* local structures */

namespace {
   struct bopenmgr {
	bfile		*op ;
	cchar		*fn ;
	cchar		*os ;
	int		to ;
	cint		maxopenfile = getdtablesize() ;
	mode_t		om ;
	bopenmgr(bfile *aop,cchar *afn,cchar *aos,mode_t aom,int ato) noex {
	    op = aop ;
	    fn = afn ;
	    os = aos ;
	    om = aom ;
	    to = ato ;
	} ;
	operator int () noex ;
	int mkoflags(cchar *) noex ;
	int getfile() noex ;
	int openfd(int) noex ;
	int openadj() noex ;
	int openreg() noex ;
   } ;
}


/* forward references */

static int	bfile_bufbegin(bfile *,int) noex ;
static int	bfile_bufend(bfile *) noex ;
static int	bfile_opts(bfile *,int,mode_t) noex ;
static int	bfile_mapbegin(bfile *) noex ;
static int	bfile_mapend(bfile *) noex ;


/* local variables */

constexpr bool		f_mapable = CF_MAPABLE ;

static bufsizevar	maxlinelen(getbufsize_ml) ;
static sysval		pagesize(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int bopene(bfile *op,cchar *fn,cchar *os,mode_t om,int to) noex {
	int		rs = SR_FAULT ;
	if (op && fn && os) {
	    memclear(op) ;
	    rs = SR_INVALID ;
	    if (fn[0]) {
		bopenmgr	bo(op,fn,os,om,to) ;
		rs = bo ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bopene) */

bopenmgr::operator int () noex {
	int		rs ;
	op->fd = -1 ;
	if ((rs = mkoflags(os)) >= 0) {
	    if ((rs = getfile()) > 0) {
	        rs = SR_OK ;
	    }
	} /* end if (mkoflags) */
	return rs ;
}
/* end method (bopenmgr::operator) */

int bopenmgr::getfile() noex {
	int		rs = SR_OK ;
	if ((rs = getfdfile(fn,-1)) >= 0) {	/* "standard" file */
	    op->f.filedesc = true ;
	    rs = openfd(rs) ;
	} else if (rs == SR_EMPTY) {		/* "null" file */
	    op->f.nullfile = true ;
	} else if (rs != SR_DOM) {
	    rs = openreg() ;
	}
	return rs ;
}
/* end method (bopenmgr::getfile) */

int bopenmgr::openfd(int idx) noex {
	int		rs = SR_OK ;
	if ((rs = uc_dupmince(idx,BFILE_MINFD)) >= 0) {
	    op->fd = rs ;
	    rs = openadj() ;
	} /* end if (uc_dupmin) */
	return rs ;
}
/* end method (bopenmgr::openfd) */

int bopenmgr::openadj() noex {
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
		op->f.append = true ;
	    }
	    if (nof) {
		op->of = nof ;
	    }
	} /* end if (uc_fcntl) */
	return rs ;
}
/* end method (bopenmgr::openadj) */

int bopenmgr::openreg() noex {
	return 0 ;
}
/* end method (bopenmgr::openreg) */

#ifdef	COMMENT
	USTAT		sb ;
	off_t		soff ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		fd_existing = -1 ;
	int		boflags = 0 ;
	int		oflags = 0 ;
	int		bsize = 0 ;
	int		f_readonly = false ;
	int		f_lessthan ;
	int		f_notseek ;
	int		f_created = false ;
	int		f ;

	op->fd = -1 ;

	oflags = mkoflags(os,&boflags) ;

/* we make "exclusive" imply "create" also! */

	if (oflags & O_EXCL)
	    oflags |= O_CREAT ;

	if ((oflags & O_ACCMODE) == O_RDONLY) f_readonly = true ;

	bsize = (op->pagesize * BFILE_BUFPAGES) ;

	f_lessthan = (((ulong) fn) < maxopenfile) ;
	if (f_lessthan)
	    fd_existing = ((int) fn) ;

	if (f_lessthan && (! (boflags & BOM_FILEDESC))) {
	    rs = SR_FAULT ;
	    goto bad0 ;
	}

	if (! f_lessthan) {
	    int		fd ;
	    if ((fd = getfdfile(fn,-1)) >= 0) {
		boflags |= BOM_FILEDESC ;
		fd_existing = fd ;
	    } else if (fd == SR_EMPTY) {
		op->f.nullfile = true ;
	    } else if (fd != SR_DOM) {
		rs = fd ;
	    }
	} /* end if */

	if (op->f.nullfie) goto ret1 ;

	if (boflags & BOM_READ) {
	    boflags &= (~ BOM_APPEND) ;
	    oflags &= (~ O_APPEND) ;
	}

/* OK, go! */

	if (boflags & BOM_FILEDESC) {
	    int	ooflags ;
	    int	f_setflags = false ;
	    int	f_addappend = false ;

	    rs = u_dup(fd_existing) ;
	    op->fd = rs ;
	    if (rs < 0) goto bad0 ;

	    rs = u_fcntl(op->fd,F_GETFL,0) ;
	    ooflags = rs ;

	    if (rs >= 0) {
		if ((oflags & O_NDELAY) && (! (ooflags & O_NDELAY))) {
		    ooflags |= O_NDELAY ;
		    f_setflags = true ;
		}
		if ((oflags & O_NONBLOCK) && (! (ooflags & O_NONBLOCK))) {
		    ooflags |= O_NONBLOCK ;
		    f_setflags = true ;
		}
	    if ((rs >= 0) && (f_addappend && (! (oflags & O_APPEND)))) {
	        ooflags |= O_APPEND ;
		f_setflags = true ;
	    }

/* if caller asked for "append", give it to her -- if even artificially */

	    f_addappend = (boflags & BOM_APPEND) && (! (boflags & BOM_READ)) ;

#if	CF_UNIXAPPEND
	    if ((rs >= 0) && (f_addappend && (! (oflags & O_APPEND)))) {
	        ooflags |= O_APPEND ;
		f_setflags = true ;
	    }
#else /* CF_UNIXAPPEND */
	    if ((rs >= 0) && f_addappend) {
	        ooflags |= O_APPEND ;
		f_setflags = true ;
	    }
#endif /* CF_UNIXAPPEND */

	    if ((rs >= 0) && f_setflags) {
		rs = u_fcntl(op->fd,F_SETFL,ooflags) ;
	    }

	    oflags = ooflags ;

	} else {

	    if (oflags & O_CREAT) {
	        rs = u_stat(fn,0,&sb) ;
	        if (rs == SR_NOENT) {
		    rs = SR_OK ;
		    op->f.created = true ;
		}
	    }

	    oflags |= O_LARGEFILE ;

	    if ((rs = uc_opene(fn,oflags,om,to)) >= 0) {
	        op->fd = rs ;
		rs = bfile_opts(op,oflags,om) ;
	        if (rs < 0) {
		    u_close(op->fd) ;
		    op->fd = -1 ;
	        }
	    }

	    if (rs < 0) goto bad0 ;

	} /* end if (opening the new FD) */

	if (rs < 0) goto bad1 ;

/* OK, we had our fun, now set all of the proper other modes for this file */

	op->of = oflags ;
	rs = u_fstat(op->fd,&sb) ;
	if (rs < 0) goto bad1 ;

	op->fsize = 0 ;
	op->fm = sb.st_mode ;
	op->ino = sb.st_ino ;
	op->dev = sb.st_dev ;
	f_notseek = true ;
	f = false ;
	f = f || S_ISREG(sb.st_mode) ;
	f = f || S_ISDIR(sb.st_mode) ;
	f = f || S_ISBLK(sb.st_mode) ;
	if (f) {
	    if (f_readonly) {
	        size_t	ps = op->pagesize ;
	        size_t	fs = sb.st_size ;
	        size_t	cs ;
		if (fs == 0) fs = 1 ;
	        cs = llceil(fs,512) ;
		bsize = MIN(cs,ps) & INT_MAX ;
	    }
	    f_notseek = false ;
	} else if (S_ISFIFO(sb.st_mode)) {
	    bsize = MIN(LINEBUFLEN,2048) ;
	    op->bm = bfilebm_line ;
	} else if (S_ISCHR(sb.st_mode)) {
	    if (isatty(op->fd)) {
	        bsize = MIN(LINEBUFLEN,2048) ;
	        op->f.terminal = true ;
	        op->bm = bfilebm_line ;
	    } /* end if (is a terminal) */
	} else if (S_ISSOCK(sb.st_mode)) {
	    op->f.network = true ;
	    bsize = (64*1024) ;
	    op->bm = bfilebm_line ;
	}

	if (! f_notseek) {
	    rs1 = u_seeko(op->fd,0L,SEEK_CUR,&soff) ;
	    op->offset = (BFILE_OFF) soff ;
	    f_notseek = (rs1 < 0) ;
	}

	if (f_notseek) {
	    op->f.notseek = true ;
	    op->offset = 0 ;
	}

	if ((oflags & O_APPEND) && (! op->f.notseek)) {
	    rs = u_seeko(op->fd,0L,SEEK_END,&soff) ;
	    op->offset = (BFILE_OFF) soff ;
	} /* end if (append mode) */

	op->maps = nullptr ;

#if	CF_MAPABLE
	{
	    int		f_mapable = (! op->f.notseek) && (! op->f.terminal) ;
	    if (f_mapable) {
	        op->f.mappable = true ;
	    }
	    if (f_mapable && (! (oflags & O_WRONLY))) {
	        rs = bfile_mapbegin(op) ;
	        if (rs < 0) goto bad1 ;
	    } /* end if (file object is mappable) */
	}
#else /* CF_MAPABLE */
	rs = bfile_bufbegin(op,bsize) ;
#endif /* CF_MAPABLE */

	if (rs < 0) goto bad1 ;

	if ((oflags & O_CREAT) && op->f.created) f_created = true ;

ret1:
	op->magic = BFILE_MAGIC ;		/* set magic number */

ret0:
	return (rs >= 0) ? f_created : rs ;

/* bad things come here */
bad1:
	if (op->fd >= 0) {
	    u_close(op->fd) ;
	    op->fd = -1 ;
	}
	if (op->maps != nullptr) {
	    uc_free(op->maps) ;
	    op->maps = nullptr ;
	}

bad0:
	goto ret0 ;
}
/* end subroutine (bopene) */
#endif /* COMMENT */

int bopen(bfile *op,cchar *fn,cchar *os,mode_t om) noex {
	return bopene(op,fn,os,om,-1) ;
}
/* end subroutine (bopen) */

#ifdef	COMMENT
int bopenprog(bfile *op,cc *pname,cc *os,mainv argv,mainv envv) noex {
	int		rs = SR_OK ;
	int		oflags = 0 ;
	int		boflags = 0 ;
	cchar		*args[2] ;
	char		progfname[MAXPATHLEN+1] ;

	if (op == nullptr) return SR_FAULT ;
	if (pname == nullptr) return SR_FAULT ;
	if (os == nullptr) return SR_FAULT ;

	if (pname[0] == '\0') return SR_INVALID ;
	if (os[0] == '\0') return SR_INVALID ;

	memclear(op) ;
	op->fd = -1 ;
	op->f.notseek = true ;

	oflags = mkoflags(os,&boflags) ;
	oflags |= O_CLOEXEC ;

	if (argv == nullptr) {
	    int	i = 0 ;
	    argv = args ;
	    args[i++] = pname ;
	    args[i] = nullptr ;
	}

	if (strchr(pname,'/') == nullptr) {
	    rs = findfilepath(nullptr,progfname,pname,X_OK) ;
	    if (rs > 0) pname = progfname ;
	}

	if (rs >= 0) {
	if ((rs = uc_openprog(pname,oflags,argv,envv)) >= 0) {
	    op->fd = rs ;
	    if ((rs = bfile_opts(op,oflags,0)) >= 0) {
		    cint	bsize = MIN(LINEBUFLEN,2048) ;
	            if ((rs = bfile_bufbegin(op,bsize)) >= 0) {
			op->of = oflags ;
		        op->bm = bfilebm_line ;
	                op->magic = BFILE_MAGIC ;
	            } /* end if (buffer-allocation) */
	    }
	    if (rs < 0) {
		u_close(op->fd) ;
		op->fd = -1 ;
	    }
	} /* end if (opened) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (bopenprog) */
#endif /* COMMENT */

int bclose(bfile *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bfile_magic(op)) >= 0) {
	        if (op->f.writing && (op->len > 0)) {
	            rs1 = bfile_flush(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (op->bdata) {
	            rs1 = bfile_bufend(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (op->maps) {
	            rs1 = bfile_mapend(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = uc_close(op->fd) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (bclose) */


/* private subroutines */

static int bfile_bufbegin(bfile *op,int bsize) noex {
	int		rs ;
	char		*p ;
	if (bsize == 0) bsize = op->pagesize ;
	if ((rs = uc_malloc(bsize,&p)) >= 0) {
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

static int bfile_opts(bfile *op,int oflags,mode_t om) noex {
	int		rs ;
	if ((rs = uc_closeonexec(op->fd,true)) >= 0) {
	    if ((oflags & O_MINMODE) && (om > 0)) {
	        rs = uc_fminmod(op->fd,om) ;
	    }
	}
	if (oflags & O_NETWORK) op->f.network = true ;
	return rs ;
}
/* end subroutine (bfile_opts) */

static int bfile_mapbegin(bfile *op) noex {
	int		rs ;
	cint		sz = (BFILE_NMAPS * sizeof(bfile_map)) ;
	void		*vp{} ;
	if ((rs = uc_malloc(sz,&vp)) >= 0) {
	    op->maps = maper(vp) ;
	    for (int i = 0 ; i < BFILE_NMAPS ; i += 1) {
	        op->maps[i].f.valid = false ;
	        op->maps[i].bdata = nullptr ;
	    } /* end for */
	    op->bp = nullptr ;
	    op->f.mapinit = true ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (bfile_mapbegin) */

static int bfile_mapend(bfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < BFILE_NMAPS ; i += 1) {
	    if ((op->maps[i].f.valid) && op->maps[i].bdata) {
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
	return rs ;
}
/* end subroutine (bfile_mapend) */

int bopenmgr::mkoflags(cchar *os) noex {
	int		rs = SR_OK ;
	int		of = 0 ;
	while (*os) {
	    cint	sc = mkchar(*os++) ;
	    switch (sc) {
	    case 'r':
	        op->f.rd = true ;
	        break ;
	    case 'w':
		op->f.wr = true ;
	        break ;
	    case 'm':
	    case '+':
	        op->f.rd = true ;
		op->f.wr = true ;
	        break ;
	    case 'a':
	        of |= O_APPEND ;
		op->f.append = true ;
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
	    case 'b': /* POSIX "binary" mode -- nothing on real UNIXes® */
	        break ;
	    case 'x':
	        of |= O_EXCL ;
		break ;
	    case 'F':
		of |= O_MINFD ;		/* minimum-file-descriptor */
		break ;
	    case 'N':
	        of |= O_NETWORK ;	/* "network" file */
		op->f.network = true ;
		break ;
	    case 'M':
	        of |= O_MINMODE ;	/* minimum file-permissions-mode */
		break ;
	    } /* end switch */
	} /* end while (open flags) */
	if (op->f.rd && op->f.wr) {
	    of |= O_RDWR ;
	} else if (op->f.wr) {
	    of |= O_WRONLY ;
	} else {
	    of |= O_RDONLY ;
	}
	op->of = of ;
	return rs ;
}
/* end method (bopenmgr::mkoflags) */


