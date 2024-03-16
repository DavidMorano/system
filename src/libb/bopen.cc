/* bopen SUPPORT */
/* lang=C++20 */

/* "Basic I/O" package */
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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<string.h>
#include	<usystem.h>
#include	<stdfnames.h>
#include	<getfdfile.h>
#include	<snx.h>
#include	<mkchar.h>
#include	<cfdec.h>
#include	<intceil.h>
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

#if	CF_MAPABLE
static int	bfile_mapbegin(bfile *) noex ;
static int	bfile_mapend(bfile *) noex ;
#endif /* CF_MAPABLE */

static int	mkoflags(cchar *,int *) noex ;
static int	extfd(cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int bopene(bfile *fp,cchar *name,cchar *os,mode_t perm,int to) noex {
	USTAT		sb ;
	off_t		soff ;
	cint		maxopenfile = getdtablesize() ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		fd_existing = -1 ;
	int		boflags = 0 ;
	int		oflags = 0 ;
	int		bsize = 0 ;
	int		f_readonly = FALSE ;
	int		f_lessthan ;
	int		f_notseek ;
	int		f_created = FALSE ;
	int		f ;

	if (fp == NULL) return SR_FAULT ;
	if (os == NULL) return SR_FAULT ;

	if (os[0] == '\0') return SR_INVALID ;

	memset(fp,0,sizeof(bfile)) ;
	fp->fd = -1 ;

	oflags = mkoflags(os,&boflags) ;

/* we make "exclusive" imply "create" also! */

	if (oflags & O_EXCL)
	    oflags |= O_CREAT ;

	if ((oflags & O_ACCMODE) == O_RDONLY) f_readonly = TRUE ;

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

	f_lessthan = (((ulong) name) < maxopenfile) ;
	if (f_lessthan)
	    fd_existing = ((int) name) ;

	if (f_lessthan && (! (boflags & BO_FILEDESC))) {
	    rs = SR_FAULT ;
	    goto bad0 ;
	}

	if (! f_lessthan) {
	    int		fd ;
	    if ((fd = getfdfile(name,-1)) >= 0) {
		boflags |= BO_FILEDESC ;
		fd_existing = fd ;
	    } else if (fd == SR_EMPTY) {
		fp->f.nullfile = TRUE ;
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
	    int	f_setflags = FALSE ;
	    int	f_addappend = FALSE ;

	    rs = u_dup(fd_existing) ;
	    fp->fd = rs ;
	    if (rs < 0) goto bad0 ;

	    rs = u_fcntl(fp->fd,F_GETFL,0) ;
	    ooflags = rs ;

	    if (rs >= 0) {
		if ((oflags & O_NDELAY) && (! (ooflags & O_NDELAY))) {
		    ooflags |= O_NDELAY ;
		    f_setflags = TRUE ;
		}
		if ((oflags & O_NONBLOCK) && (! (ooflags & O_NONBLOCK))) {
		    ooflags |= O_NONBLOCK ;
		    f_setflags = TRUE ;
		}
	    }

/* if caller asked for "append", give it to her -- if even artificially */

	    f_addappend = (boflags & BO_APPEND) && (! (boflags & BO_READ)) ;

#if	CF_UNIXAPPEND
	    if ((rs >= 0) && (f_addappend && (! (oflags & O_APPEND)))) {
	        ooflags |= O_APPEND ;
		f_setflags = TRUE ;
	    }

#else /* CF_UNIXAPPEND */

	    if ((rs >= 0) && f_addappend) {
	        ooflags |= O_APPEND ;
		f_setflags = TRUE ;
	    }

#endif /* CF_UNIXAPPEND */

	    if ((rs >= 0) && f_setflags) {
		rs = u_fcntl(fp->fd,F_SETFL,ooflags) ;
	    }

	    oflags = ooflags ;

	} else {

	    if (oflags & O_CREAT) {
	        rs = bfilestat(name,0,&sb) ;
	        if (rs == SR_NOENT) {
		    rs = SR_OK ;
		    fp->f.created = TRUE ;
		}
	    }

	    oflags |= O_LARGEFILE ;

	    if ((rs = uc_opene(name,oflags,perm,to)) >= 0) {
	        fp->fd = rs ;
		rs = bfile_opts(fp,oflags,perm) ;
	        if (rs < 0) {
		    u_close(fp->fd) ;
		    fp->fd = -1 ;
	        }
	    }

	    if (rs < 0) goto bad0 ;

	} /* end if (opening the new FD) */

	if (rs < 0) goto bad1 ;

/* OK, we had our fun, now set all of the proper other modes for this file */

	fp->oflags = oflags ;
	rs = bfilefstat(fp->fd,&sb) ;
	if (rs < 0) goto bad1 ;

	fp->fsize = 0 ;
	fp->mode = sb.st_mode ;
	fp->ino = sb.st_ino ;
	fp->dev = sb.st_dev ;
	f_notseek = TRUE ;
	f = FALSE ;
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
	    f_notseek = FALSE ;
	} else if (S_ISFIFO(sb.st_mode)) {
	    bsize = MIN(LINEBUFLEN,2048) ;
	    fp->bm = bfile_bmline ;
	} else if (S_ISCHR(sb.st_mode)) {
	    if (isatty(fp->fd)) {
	        bsize = MIN(LINEBUFLEN,2048) ;
	        fp->f.terminal = TRUE ;
	        fp->bm = bfile_bmline ;
	    } /* end if (is a terminal) */
	} else if (S_ISSOCK(sb.st_mode)) {
	    fp->f.network = TRUE ;
	    bsize = (64*1024) ;
	    fp->bm = bfile_bmline ;
	}

	if (! f_notseek) {
	    rs1 = u_seeko(fp->fd,0L,SEEK_CUR,&soff) ;
	    fp->offset = (BFILE_OFF) soff ;
	    f_notseek = (rs1 < 0) ;
	}

	if (f_notseek) {
	    fp->f.notseek = TRUE ;
	    fp->offset = 0 ;
	}

	if ((oflags & O_APPEND) && (! fp->f.notseek)) {
	    rs = u_seeko(fp->fd,0L,SEEK_END,&soff) ;
	    fp->offset = (BFILE_OFF) soff ;
	} /* end if (append mode) */

	fp->maps = NULL ;

#if	CF_MAPABLE
	{
	    int		f_mapable = (! fp->f.notseek) && (! fp->f.terminal) ;
	    if (f_mapable) {
	        fp->f.mappable = TRUE ;
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

	if ((oflags & O_CREAT) && fp->f.created) f_created = TRUE ;

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
	if (fp->maps != NULL) {
	    uc_free(fp->maps) ;
	    fp->maps = NULL ;
	}

bad0:
	goto ret0 ;
}
/* end subroutine (bopene) */

int bopen(bfile *fp,cchar *fname,cchar *os,mode_t perm) noex {
	return bopene(fp,fname,os,perm,-1) ;
}
/* end subroutine (bopen) */

int bopenprog(bfile *fp,cc *pname,cc *os,cc **argv,cc **envv) noex {
	int		rs = SR_OK ;
	int		oflags = 0 ;
	int		boflags = 0 ;
	cchar		*args[2] ;
	char		progfname[MAXPATHLEN+1] ;

	if (fp == NULL) return SR_FAULT ;
	if (pname == NULL) return SR_FAULT ;
	if (os == NULL) return SR_FAULT ;

	if (pname[0] == '\0') return SR_INVALID ;
	if (os[0] == '\0') return SR_INVALID ;

	memset(fp,0,sizeof(bfile)) ;
	fp->pagesize = getpagesize() ;
	fp->fd = -1 ;
	fp->f.notseek = TRUE ;

	oflags = mkoflags(os,&boflags) ;
	oflags |= O_CLOEXEC ;

	if (argv == NULL) {
	    int	i = 0 ;
	    argv = args ;
	    args[i++] = pname ;
	    args[i] = NULL ;
	}

	if (strchr(pname,'/') == NULL) {
	    rs = findfilepath(NULL,progfname,pname,X_OK) ;
	    if (rs > 0) pname = progfname ;
	}

	if (rs >= 0) {
	if ((rs = uc_openprog(pname,oflags,argv,envv)) >= 0) {
	    fp->fd = rs ;
	    if ((rs = bfile_opts(fp,oflags,0)) >= 0) {
		    cint	bsize = MIN(LINEBUFLEN,2048) ;
	            if ((rs = bfile_bufbegin(fp,bsize)) >= 0) {
			fp->oflags = oflags ;
		        fp->bm = bfile_bmline ;
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
	int		rs = SR_OK ;
	int		rs1 ;

	if (fp == NULL) return SR_FAULT ;

	if (fp->magic != BFILE_MAGIC) return SR_NOTOPEN ;

	if (fp->f.nullfile) goto ret1 ;

	if (fp->f.write && (fp->len > 0)) {
	    rs1 = bfile_flush(fp) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (fp->bdata != NULL) {
	    rs1 = bfile_bufend(fp) ;
	    if (rs >= 0) rs = rs1 ;
	}

#if	CF_MAPABLE
	if (fp->maps != NULL) {
	    rs1 = bfile_mapend(fp) ;
	    if (rs >= 0) rs = rs1 ;
	}
#endif /* CF_MAPABLE */

	rs1 = u_close(fp->fd) ;
	if (rs >= 0) rs = rs1 ;

ret1:
	fp->magic = 0 ;
	return rs ;
}
/* end subroutine (bclose) */


/* private subroutines */

static int bfile_bufbegin(bfile *fp,int bsize) noex {
	int		rs ;
	char		*p ;
	if (fp->pagesize == 0) fp->pagesize = getpagesize() ;
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
	if (fp->bdata != NULL) {
	    rs1 = uc_free(fp->bdata) ;
	    if (rs >= 0) rs = rs1 ;
	    fp->bdata = NULL ;
	    fp->bbp = NULL ;
	    fp->bp = NULL ;
	    fp->bsize = 0 ;
	}
	return rs ;
}
/* end subroutine (bfile_bufend) */

static int bfile_opts(bfile *fp,int oflags,mode_t om) noex {
	int		rs ;
	rs = uc_closeonexec(fp->fd,TRUE) ;
	if ((rs >= 0) && (oflags & O_MINMODE) && (om > 0)) {
	    rs = uc_fminmod(fp->fd,om) ;
	}
	if (oflags & O_NETWORK) fp->f.network = TRUE ;
	return rs ;
}
/* end subroutine (bfile_opts) */


#if	CF_MAPABLE

static int bfile_mapbegin(bfile *fp) noex {
	int		rs ;
	int		i ;
	int		size ;

/* allocate the map structures */

	size = BFILE_NMAPS * sizeof(struct bfile_map) ;
	if ((rs = uc_malloc(size,&fp->maps)) >= 0) {

/* initialize the maps, let them get paged in as needed! */

	for (i = 0 ; i < BFILE_NMAPS ; i += 1) {
	    fp->maps[i].f.valid = FALSE ;
	    fp->maps[i].buf = NULL ;
	} /* end for */

	fp->bp = NULL ;
	fp->f.mapinit = TRUE ;

	} /* end if (m-a) */

	return rs ;
}
/* end subroutine (bfile_mapbegin) */

static int bfile_mapend(bfile *fp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	for (i = 0 ; i < BFILE_NMAPS ; i += 1) {
	    if ((fp->maps[i].f.valid) && (fp->maps[i].buf != NULL)) {
	        rs1 = u_munmap(fp->maps[i].buf,(size_t) fp->pagesize) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	rs1 = uc_free(fp->maps) ;
	if (rs >= 0) rs = rs1 ;
	fp->maps = NULL ;

	return rs ;
}
/* end subroutine (bfile_mapend) */

#endif /* CF_MAPABLE */

static int mkoflags(cchar *os,int *bfp) noex {
	int		oflags = 0 ;
	int		bflags = 0 ;
	while (*os) {
	    cint	sc = (*os++ & 0xff) ;
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
	   cint		ch = MKCHAR(s[0]) ;
	   if (isdigitlatin(ch)) {
		rs = cfdeci(s,-1,&fd) ;
	    }
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (extfd) */


