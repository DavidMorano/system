/* bopen SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package */
/* version %I% last-modified %G% */

#define	CF_UNIXAPPEND	1	/* BRAINDAMAGED_IN_UNIX (on NFS mounted FSs) */
#define	CF_MAPABLE	0	/* allow mapped files */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

	= 1999-01-10, David A­D­ Morano
	I added the little extra code to allow for memory mapped
	I-O. It is all a waste because it is way slower than without
	it! This should teach me to leave old programs alone!!!

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
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
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* strchr(3c)| */
#include	<usystem.h>
#include	<sysval.h>
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

#define	BO_READ		(1<<0)
#define	BO_WRITE	(1<<1)
#define	BO_APPEND	(1<<2)
#define	BO_FILEDESC	(1<<3)

#undef	FLBUFLEN
#define	FLBUFLEN	100

#ifndef	CF_MAPABLE
#define	CF_MAPABLE	0	/* allow mapped files */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	findfilepath(cchar *,char *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	bfile_bufbegin(bfile *,int) noex ;
static int	bfile_bufend(bfile *) noex ;
static int	bfile_opts(bfile *,int,mode_t) noex ;
static int	bfile_mapbegin(bfile *) noex ;
static int	bfile_mapend(bfile *) noex ;

static int	mkoflags(cchar *,int *) noex ;
static int	extfd(cchar *) noex ;


/* local variables */

constexpr bool		f_mapable = CF_MAPABLE ;

static sysval		pagesize(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int bopene(bfile *fp,cchar *fn,cchar *os,mode_t om,int to) noex {
	USTAT		sb ;
	off_t		soff ;
	cint		maxopenfile = getdtablesize() ;
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

	if (fp == nullptr) return SR_FAULT ;
	if (os == nullptr) return SR_FAULT ;

	if (os[0] == '\0') return SR_INVALID ;

	memset(fp,0,sizeof(bfile)) ;
	fp->fd = -1 ;

	oflags = mkoflags(os,&boflags) ;

/* we make "exclusive" imply "create" also! */

	if (oflags & O_EXCL)
	    oflags |= O_CREAT ;

	if ((oflags & O_ACCMODE) == O_RDONLY) f_readonly = true ;

#ifdef	COMMENT
	if ((boflags & BO_READ) && (boflags & BO_WRITE)) {
	    oflags |= O_RDWR ;
	} else if (boflags & BO_WRITE) {
	    oflags |= O_WRONLY ;
	} else
	    oflags |= O_RDONLY ;
#endif /* COMMENT */

#ifdef	COMMENT
	if ((oflags & O_WRONLY) && (! (oflags & O_APPEND)))
	    oflags |= (O_CREAT | O_TRUNC) ;
#endif

	fp->pagesize = getpagesize() ;
	bsize = (fp->pagesize * BFILE_BUFPAGES) ;

	f_lessthan = (((ulong) fn) < maxopenfile) ;
	if (f_lessthan)
	    fd_existing = ((int) fn) ;

	if (f_lessthan && (! (boflags & BO_FILEDESC))) {
	    rs = SR_FAULT ;
	    goto bad0 ;
	}

	if (! f_lessthan) {
	    int		fd ;
	    if ((fd = getfdfile(fn,-1)) >= 0) {
		boflags |= BO_FILEDESC ;
		fd_existing = fd ;
	    } else if (fd == SR_EMPTY) {
		fp->f.nullfile = true ;
	    } else if (fd != SR_DOM) {
		rs = fd ;
	    }
	} /* end if */

	if (fp->f.nullfie) goto ret1 ;

	if (boflags & BO_READ) {
	    boflags &= (~ BO_APPEND) ;
	    oflags &= (~ O_APPEND) ;
	}

/* OK, go! */

	if (boflags & BO_FILEDESC) {
	    int	ooflags ;
	    int	f_setflags = false ;
	    int	f_addappend = false ;

	    rs = u_dup(fd_existing) ;
	    fp->fd = rs ;
	    if (rs < 0) goto bad0 ;

	    rs = u_fcntl(fp->fd,F_GETFL,0) ;
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
	    }

/* if caller asked for "append", give it to her -- if even artificially */

	    f_addappend = (boflags & BO_APPEND) && (! (boflags & BO_READ)) ;

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
		rs = u_fcntl(fp->fd,F_SETFL,ooflags) ;
	    }

	    oflags = ooflags ;

	} else {

	    if (oflags & O_CREAT) {
	        rs = u_stat(fn,0,&sb) ;
	        if (rs == SR_NOENT) {
		    rs = SR_OK ;
		    fp->f.created = true ;
		}
	    }

	    oflags |= O_LARGEFILE ;

	    if ((rs = uc_opene(fn,oflags,om,to)) >= 0) {
	        fp->fd = rs ;
		rs = bfile_opts(fp,oflags,om) ;
	        if (rs < 0) {
		    u_close(fp->fd) ;
		    fp->fd = -1 ;
	        }
	    }

	    if (rs < 0) goto bad0 ;

	} /* end if (opening the new FD) */

	if (rs < 0) goto bad1 ;

/* OK, we had our fun, now set all of the proper other modes for this file */

	fp->of = oflags ;
	rs = u_fstat(fp->fd,&sb) ;
	if (rs < 0) goto bad1 ;

	fp->fsize = 0 ;
	fp->fm = sb.st_mode ;
	fp->ino = sb.st_ino ;
	fp->dev = sb.st_dev ;
	f_notseek = true ;
	f = false ;
	f = f || S_ISREG(sb.st_mode) ;
	f = f || S_ISDIR(sb.st_mode) ;
	f = f || S_ISBLK(sb.st_mode) ;
	if (f) {
	    if (f_readonly) {
	        size_t	ps = fp->pagesize ;
	        size_t	fs = sb.st_size ;
	        size_t	cs ;
		if (fs == 0) fs = 1 ;
	        cs = llceil(fs,512) ;
		bsize = MIN(cs,ps) & INT_MAX ;
	    }
	    f_notseek = false ;
	} else if (S_ISFIFO(sb.st_mode)) {
	    bsize = MIN(LINEBUFLEN,2048) ;
	    fp->bm = bfilebm_line ;
	} else if (S_ISCHR(sb.st_mode)) {
	    if (isatty(fp->fd)) {
	        bsize = MIN(LINEBUFLEN,2048) ;
	        fp->f.terminal = true ;
	        fp->bm = bfilebm_line ;
	    } /* end if (is a terminal) */
	} else if (S_ISSOCK(sb.st_mode)) {
	    fp->f.network = true ;
	    bsize = (64*1024) ;
	    fp->bm = bfilebm_line ;
	}

	if (! f_notseek) {
	    rs1 = u_seeko(fp->fd,0L,SEEK_CUR,&soff) ;
	    fp->offset = (BFILE_OFF) soff ;
	    f_notseek = (rs1 < 0) ;
	}

	if (f_notseek) {
	    fp->f.notseek = true ;
	    fp->offset = 0 ;
	}

	if ((oflags & O_APPEND) && (! fp->f.notseek)) {
	    rs = u_seeko(fp->fd,0L,SEEK_END,&soff) ;
	    fp->offset = (BFILE_OFF) soff ;
	} /* end if (append mode) */

	fp->maps = nullptr ;

#if	CF_MAPABLE
	{
	    int		f_mapable = (! fp->f.notseek) && (! fp->f.terminal) ;
	    if (f_mapable) {
	        fp->f.mappable = true ;
	    }
	    if (f_mapable && (! (oflags & O_WRONLY))) {
	        rs = bfile_mapbegin(fp) ;
	        if (rs < 0) goto bad1 ;
	    } /* end if (file object is mappable) */
	}
#else /* CF_MAPABLE */
	rs = bfile_bufbegin(fp,bsize) ;
#endif /* CF_MAPABLE */

	if (rs < 0) goto bad1 ;

	if ((oflags & O_CREAT) && fp->f.created) f_created = true ;

ret1:
	fp->magic = BFILE_MAGIC ;		/* set magic number */

ret0:
	return (rs >= 0) ? f_created : rs ;

/* bad things come here */
bad1:
	if (fp->fd >= 0) {
	    u_close(fp->fd) ;
	    fp->fd = -1 ;
	}
	if (fp->maps != nullptr) {
	    uc_free(fp->maps) ;
	    fp->maps = nullptr ;
	}

bad0:
	goto ret0 ;
}
/* end subroutine (bopene) */

int bopen(bfile *fp,cchar *fn,cchar *os,mode_t om) noex {
	return bopene(fp,fn,os,om,-1) ;
}
/* end subroutine (bopen) */

int bopenprog(bfile *fp,cc *pname,cc *os,mainv argv,mainv envv) noex {
	int		rs = SR_OK ;
	int		oflags = 0 ;
	int		boflags = 0 ;
	cchar		*args[2] ;
	char		progfname[MAXPATHLEN+1] ;

	if (fp == nullptr) return SR_FAULT ;
	if (pname == nullptr) return SR_FAULT ;
	if (os == nullptr) return SR_FAULT ;

	if (pname[0] == '\0') return SR_INVALID ;
	if (os[0] == '\0') return SR_INVALID ;

	memset(fp,0,sizeof(bfile)) ;
	fp->pagesize = getpagesize() ;
	fp->fd = -1 ;
	fp->f.notseek = true ;

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
	    fp->fd = rs ;
	    if ((rs = bfile_opts(fp,oflags,0)) >= 0) {
		    cint	bsize = MIN(LINEBUFLEN,2048) ;
	            if ((rs = bfile_bufbegin(fp,bsize)) >= 0) {
			fp->of = oflags ;
		        fp->bm = bfilebm_line ;
	                fp->magic = BFILE_MAGIC ;
	            } /* end if (buffer-allocation) */
	    }
	    if (rs < 0) {
		u_close(fp->fd) ;
		fp->fd = -1 ;
	    }
	} /* end if (opened) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (bopenprog) */

int bclose(bfile *fp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bfile_magic(fp)) >= 0) {
	    if ((rs = bfile_active(fp)) > 0) {
	        if (fp->f.write && (fp->len > 0)) {
	            rs1 = bfile_flush(fp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (fp->bdata) {
	            rs1 = bfile_bufend(fp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (fp->maps) {
	            rs1 = bfile_mapend(fp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = uc_close(fp->fd) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        fp->magic = 0 ;
	    } /* end if (active) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (bclose) */


/* private subroutines */

static int bfile_bufbegin(bfile *fp,int bsize) noex {
	int		rs ;
	char		*p ;
	if (bsize == 0) bsize = fp->pagesize ;
	if ((rs = uc_malloc(bsize,&p)) >= 0) {
	    fp->bdata = p ;
	    fp->bsize = bsize ;
	    fp->bbp = p ;
	    fp->bp = p ;
	}
	return rs ;
}
/* end subroutine (bfile_bufbegin) */

static int bfile_bufend(bfile *fp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (fp->bdata) {
	    rs1 = uc_free(fp->bdata) ;
	    if (rs >= 0) rs = rs1 ;
	    fp->bdata = nullptr ;
	    fp->bbp = nullptr ;
	    fp->bp = nullptr ;
	    fp->bsize = 0 ;
	}
	return rs ;
}
/* end subroutine (bfile_bufend) */

static int bfile_opts(bfile *fp,int oflags,mode_t om) noex {
	int		rs ;
	rs = uc_closeonexec(fp->fd,true) ;
	if ((rs >= 0) && (oflags & O_MINMODE) && (om > 0)) {
	    rs = uc_fminmod(fp->fd,om) ;
	}
	if (oflags & O_NETWORK) fp->f.network = true ;
	return rs ;
}
/* end subroutine (bfile_opts) */

static int bfile_mapbegin(bfile *fp) noex {
	typedef bfile_map *	maper ;
	int		rs ;
	cint		sz = = BFILE_NMAPS * sizeof(bfile_map) ;
	void		*vp{} ;
	if ((rs = uc_malloc(sz,&vp)) >= 0) {
	    fp->maps = maper(vp) ;
	    for (int i = 0 ; i < BFILE_NMAPS ; i += 1) {
	        fp->maps[i].f.valid = false ;
	        fp->maps[i].bdata = nullptr ;
	    } /* end for */
	    fp->bp = nullptr ;
	    fp->f.mapinit = true ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (bfile_mapbegin) */

static int bfile_mapend(bfile *fp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < BFILE_NMAPS ; i += 1) {
	    if ((fp->maps[i].f.valid) && fp->maps[i].bdata) {
		void	*md = fp->maps[i].bdata ;
		csize	ms = fp->maps[i].bsize ;
	        rs1 = u_mmapend(md,ms) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	{
	    rs1 = uc_free(fp->maps) ;
	    if (rs >= 0) rs = rs1 ;
	    fp->maps = nullptr ;
	}
	return rs ;
}
/* end subroutine (bfile_mapend) */

static int mkoflags(cchar *os,int *bfp) noex {
	int		oflags = 0 ;
	int		bflags = 0 ;
	while (*os) {
	    cint	sc = mkchar(*os++) ;
	    switch (sc) {
	    case 'd':
	        bflags |= BO_FILEDESC ;
	        break ;
	    case 'r':
	        bflags |= BO_READ ;
	        break ;
	    case 'w':
		bflags |= BO_WRITE ;
	        break ;
	    case 'm':
	    case '+':
	        bflags |= (BO_READ | BO_WRITE) ;
	        break ;
	    case 'a':
	        oflags |= O_APPEND ;
	        bflags |= BO_APPEND ;
	        break ;
	    case 'c':
	        oflags |= O_CREAT ;
	        break ;
	    case 'e':
	        oflags |= (O_CREAT | O_EXCL) ;
	        break ;
	    case 't':
	        oflags |= (O_CREAT | O_TRUNC) ;
	        break ;
	    case 'n':
	        oflags |= O_NDELAY ;
	        break ;
/* POSIX "binary" mode -- does nothing on real UNIXes® */
	    case 'b':
	        break ;
	    case 'x':
	        oflags |= O_EXCL ;
		break ;
	    case 'N':
	        oflags |= O_NETWORK ;
		break ;
	    case 'M':
	        oflags |= O_MINMODE ;
		break ;
	    } /* end switch */
	} /* end while (open flags) */
	if (bflags & BO_WRITE) {
	   oflags |= ((bflags & BO_READ) ? O_RDWR : O_WRONLY) ;
	}
	if (bfp) {
	    *bfp = bflags ;
	}
	return oflags ;
}
/* end subroutine (mkoflags) */

static int extfd(cchar *s) noex {
	int	rs = SR_INVALID ;
	int	fd = -1 ;
	if (*s++ == BFILE_FDCH) {
	   cint		ch = mkchar(s[0]) ;
	   if (isdigitlatin(ch)) {
		rs = cfdeci(s,-1,&fd) ;
	    }
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (extfd) */


