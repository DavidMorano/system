/* lineindex SUPPORT */
/* lang=C++20 */

/* line indexing object */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-06-11, David A­D­ Morano
	I snarfed this file from the SS-Hammock crap since I thought
	it might be a bit similar.  We will see how it works out!

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	lineindex

	Description:
	This object module manages a line-index database.  It can
	also create such a database if it is opened with the O_CREAT
	option.  Note that line indexing is so fast that trying to
	super-optimize anything here is not really worth it.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>		/* Memory Management */
#include	<netinet/in.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cinttypes>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sysval.hh>
#include	<getbufsize.h>
#include	<mallocstuff.h>
#include	<bfile.h>
#include	<filemap.h>
#include	<intceil.h>
#include	<intsat.h>
#include	<localmisc.h>		/* |MODP2| */

#include	"lineindex.h"


/* local defines */

#define	LI		lineindex

#define	TO_FILECOME	2
#define	TO_OPEN		(60 * 60)
#define	TO_MAP		(1 * 60)
#define	TO_CHECK	4
#define	TO_ACCESS	(1 * 60)

#define	NRECS		(2 * 1024)


/* imported namespaces */


/* local typedefs */

typedef mode_t		om_t ;		/* open-mode */


/* external subroutines */


/* forward references */

template<typename ... Args>
static int lineindex_ctor(lineindex *op,Args ... args) noex {
    	LINEINDEX	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lineindex_ctor) */

static int lineindex_dtor(lineindex *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lineindex_dtor) */

template<typename ... Args>
static inline int lineindex_magic(lineindex *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LINEINDEX_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (lineindex_magic) */

static int	lineindex_stbegin(LI *,cc *,cc *) noex ;
static int	lineindex_stend(LI *) noex ;

static int	lineindex_mkindex(lineindex *) noex ;
static int	lineindex_fileheader(lineindex *) noex ;
static int	lineindex_filembegin(lineindex *,time_t) noex ;
static int	lineindex_filemend(lineindex *) noex ;

static int	lineindex_fileopen(lineindex *,time_t) noex ;
static int	lineindex_fileclose(lineindex *) noex ;


/* local variables */

enum headers {
	header_rectab,
	header_wtime,
	header_lines,
	header_overlast
} ;

static sysval		pagesize(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int lineindex_open(LI *op,cc *ifn,int of,om_t om,cc *tfn) noex {
	int		rs ;
	if ((rs = lineindex_ctor(op,ifn)) >= 0) {
	    rs = SR_INVALID ;
	    if (ifn[0]) {
	        op->fd = -1 ;
	        op->of = of ;
	        op->om = om ;
		if ((rs = pagesize) >= 0) {
		    cint	am = (of & O_ACCMODE) ;
		    op->pagesize = rs ;
		    op->f.wantwrite = ((am == O_WRONLY) || (am == O_RDWR)) ;
		    if ((rs = lineindex_stbegin(op,ifn,tfn)) >= 0) {
		    	{
		    	    rs = lineindex_opener(op) ;
			}
			if (rs < 0) {
			    lineindex_stend(op) ;
			}
		    } /* end if (store-files) */
		} /* end if (pagesize) */
	    } /* end if (valid) */
	    if (rs < 0) {
		lineindex_dtor(op) ;
	    }
	} /* end if (lineindex_ctor) */
	return rs ;
}
/* end subroutine (lineindex_opne) */

namespace {
}

static int lineindex_opener(LI *op) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = lineindex_fileopen(op,dt)) >= 0) {
	    cint	sz = 16 + 4 + (header_overlast * sizeof(int)) ;
	    if (USTAT sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	size_t	fsz = size_t(sb.st_size) ;
	csize	hsz = size_t(sz) ;
	int	to = TO_FILECOME ;
	for (int i = 0 ; (i < to) && (rs >= 0) && (fsz < hsz) ; i += 1) {
	    sleep(1) ;
	    rs = u_fstat(op->fd,&sb) ;
	    fsz = size_t(sb.st_size) ;
	} /* end while */

	if (rs < 0)
	    goto bad2 ;

	if (fsz < hsz) {
	    rs = SR_TIMEDOUT ;
	    goto bad2 ;
	}

	op->mtime = sb.st_mtime ;
	op->filesize = intsat(sb.st_size) ;

/* OK, continue on */

	if (op->filesize >= sz) {

/* map it */
	    cnullptr	np{} ;
	    int mp = PROT_READ ;
	    int mf = MAP_SHARED ;
	    csize	ms = op->mapsize ;
	    op->mapsize = op->filesize ;
	    rs = u_mmapbegin(np,ms,mp,mf,op->fd,0L,&op->mapbuf) ;

	    if (rs < 0)
	        goto bad3 ;

	    op->ti_map = dt ;

/* OK, check it out */

	    rs = lineindex_fileheader(op) ;
	    if (rs < 0)
	        goto bad4 ;

/* ok, we're good (?) */

	    op->ti_access = dt ;
	    op->f.fileinit = true ;

	} /* end if (file had some data) */

/* we should be good */

	op->magic = LINEINDEX_MAGIC ;

ret1:
	lineindex_fileclose(op) ;

ret0:
	return rs ;

/* we're out of here */
bad4:
	u_munmap(op->mapbuf,(size_t) op->filesize) ;

bad3:
	lineindex_fileclose(op) ;

bad2:
	if (op->tfn != nullptr)
	    uc_free(op->tfn) ;

bad1:
	uc_free(op->ifn) ;

bad0:
	goto ret0 ;

}
/* end subroutine (lineindex_open) */

int lineindex_close(LI *op) noex {
	int		rs ;
	if ((rs = lineindex_magic(op)) >= 0) {
	    if (op->mapbuf) {
		csize	ms = op->mapsize ;
		void	*md = op->mapbuf ;
	        rs = u_mmapend(md,ms) ;
	    }
	    if (op->fd >= 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	    {
		rs1 = lineindex_stend(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = lineindex_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lineindex_close) */

int lineindex_count(LI *op) noex {
    	int		rs ;
	if ((rs = lineindex_magic(op)) >= 0) {
	    rs = op->lines ;
	}
	return rs ;
}
/* end subroutine (lineindex_count) */

int lineindex_curbegin(LI *op,lineindex_cur *curp) noex {
    	int		rs ;
	if ((rs = lineindex_magic(op,curp)) >= 0) {
	    op->cursors += 1 ;
	    op->f.cursorlockbroken = false ;
	    op->f.cursoracc = false ;
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lineindex_curbegin) */

int lineindex_curend(LI *op,lineindex_cur *curp) noex {
    	int		rs ;
	if ((rs = lineindex_magic(op,curp)) >= 0) {
	    if (op->f.cursoracc) {
	        op->ti_access = getustime ;
	    }
	    if (op->cursors > 0) {
	        op->cursors -= 1 ;
	    }
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lineindex_curend) */

int lineindex_curenum(LI *op,lineindex_cur *curp,off_t *rop) noex {
	int		rs ;
	int		ri ;
	if ((rs = lineindex_magic(op,curp,rop)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->cursors > 0) {
		rs = SR_OK ;
		ri = (curp->i < 0) ? 0 : (curp->i + 1) ;
	        if (op->mapbuf) {
	            custime	dt = getustime ;
	            if ((rs = lineindex_filembegin(op,dt)) >= 0) {
	                rs = lineindex_fileheader(op) ;
        	    }
	        } /* end if (checking if file is mapped) */
		if (rs >= 0) {
	            if (ri < op->lines) {
	                *rop = ntohl(op->rectab[ri]) ;
	                curp->i = ri ;
		    } else {
	    	        rs = SR_NOTFOUND ;
	            }
		} /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ri : rs ;
}
/* end subroutine (lineindex_enum) */

int lineindex_lookup(LI *op,uint ri,off_t *rop) noex {
	int		rs ;
	if ((rs = lineindex_magic(op,rop)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (ri < op->lines) {
	        if (op->mapbuf) {
	            csutime	dt = getustime ;
	            if ((rs = lineindex_filembegin(op,dt)) > 0) {
	                rs = lineindex_fileheader(op) ;
	            }
	        } /* end if (checking if file is mapped) */
		if ((rs >= 0) && (ri > 0)) {
	    	    *rop = ntohl(op->rectab[ri]) ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	if ((rs < 0) && rop) {
	    *rop = 0z ;
	}
	return (rs >= 0) ? ri : rs ;
}
/* end subroutine (lineindex_lookup) */

int lineindex_check(LI *op,time_t dt) noex {
	USTAT		sb ;
	int		rs = SR_OK ;
	int		f_changed = false ;
	cchar	*cp ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != LINEINDEX_MAGIC) return SR_NOTOPEN ;

/* check things */

	if (op->cursors > 0)
	    goto ret0 ;

	if (op->mapbuf == nullptr)
	    goto ret0 ;

/* check for "unused" */

	cp = (cchar *) (op->mapbuf + 16 + 3) ;
	if (*cp)
	    goto closeit ;

/* check for time limits */

	if ((dt - op->ti_map) > TO_MAP)
	    goto closeit ;

	if ((dt - op->ti_check) > TO_CHECK) {

	    op->ti_check = dt ;
	    if ((rs = u_stat(op->ifn,&sb)) >= 0) {

	        if ((sb.st_mtime > op->mtime) ||
	            (sb.st_size > op->filesize))
	            goto changed ;

	    }

	    if (op->tfn != nullptr) {

	        if ((rs = u_stat(op->tfn,&sb)) >= 0) {

	            if (sb.st_mtime > op->mtime)
	                goto changed ;

	        }
	    }

	} /* end if (check) */

ret0:
	return (rs >= 0) ? f_changed : rs ;

/* handle a close out */
changed:
	f_changed = true ;

closeit:
	op->ti_check = dt ;
	rs = lineindex_filemend(op) ;

	goto ret0 ;
}
/* end subroutine (lineindex_check) */


/* private subroutines */

static int lineindex_stbegin(LI *op,cc *ifn,cc *tfn) noex {
    	int		rs ;
	if (char *cp ; (rs = uc_mallocstrw(ifn,-1,&cp)) >= 0) {
	    op->ifn = cp ;
	    if (tfn) {
	        rs = SR_INVALID ;
		if (tfn[0]) {
		    if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	    		op->tfn = cp ;
		    } /* end if (memory-alloation) */
		}
	    } /* end if (non-null) */
	    if (rs < 0) {
		uc_free(op->ifn) ;
		op->ifn = nullptr ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (lineindex_stbegin) */

static int lineindex_stend(LI *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->tfn) {
	    rs1 = uc_free(op->tfn) ;
	    if (rs >= 0) rs = rs1 ;
	    op->tfn = nullptr ;
	}
	if (op->ifn) {
	    rs1 = uc_free(op->ifn) ;
	    if (rs >= 0) rs = rs1 ;
	    op->ifn = nullptr ;
	}
	return rs ;
}
/* end subroutine (lineindex_stend) */

static int lineindex_fileheader(LI *op) noex {
	uint		*table ;
	uint		recoff ;
	int		rs = SR_OK ;
	int		f ;
	cchar	*cp = (cchar *) op->mapbuf ;

	f = (strncmp(cp,LINEINDEX_FILEMAGIC,LINEINDEX_FILEMAGICLEN) == 0) ;

	f = f && (*(cp + LINEINDEX_FILEMAGICLEN) == '\n') ;

	if (! f) {

	    rs = SR_BADFMT ;
	    goto bad3 ;
	}

	cp += 16 ;
	if (cp[0] > LINEINDEX_FILEVERSION) {

	    rs = SR_NOTSUP ;
	    goto bad3 ;
	}

#ifdef	COMMENT
	if (cp[1] != ENDIAN) {

	    rs = SR_NOTSUP ;
	    goto bad3 ;
	}
#else
	if (cp[1] != LINEINDEX_ENDIAN) {

	    rs = SR_NOTSUP ;
	    goto bad3 ;
	}
#endif /* COMMENT */

/* the recorder options */

	op->ropts = cp[2] ;

/* if looks good, read the header stuff */

	table = (uint *) (op->mapbuf + 16 + 4) ;

	recoff = ntohl(table[header_rectab]) ;
	op->lines = ntohl(table[header_lines]) ;
	op->wtime = ntohl(table[header_wtime]) ;

	op->rectab = (uint *) (op->mapbuf + recoff) ;

ret0:
	return rs ;

/* bad stuff comes here */
bad3:
	goto ret0 ;
}
/* end subroutine (lineindex_fileheader) */

static int lineindex_filembegin(LI *op,custime dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapbuf) {
	    xnullptr	np{} ;
	    if ((rs = lineindex_fileopen(op,dt)) >= 0) {
		csize	ms ->mapsize = uceil(op->filesize,op->pagesize) ;
		cint	mp = PROT_READ ;
		cint	mf = MAP_SHARED ;
		void	*md{} ;
		ms = uceil(op->filesize,op->pagesize) ;
	        if ((rs = u_mmap(np,ms,mp,mf,op->fd,0z,&md)) >= 0) {
		    op->mapbuf = md ;
		    op->mapsze = ms ;
	            op->ti_map = dt ;
		}
	        rs1 = lineindex_fileclose(op) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (opened the file) */
	} /* end if (mapping file) */
	return rs ;
}
/* end subroutine (lineindex_filembegin) */

static int lineindex_filemend(LI *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapbuf) {
	    csize	ms = op->mapsize ;
	    void	*md = op->mapbuf ;
	    rs1 = u_munmap(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapbuf = nullptr ;
	    op->mapsize = 0 ;
	} /* end if (checking existing map) */
	return rs ;
}
/* end subroutine (lineindex_filemend) */

static int lineindex_fileopen(LI *op,custime dt) noex {
	USTAT		sb ;
	int	rs = SR_OK ;
	int	of ;
	int	f ;
	int	f_create ;
	int	f_changed = false ;

	if (op->fd < 0) {

	of = op->of ;
	of &= (~ (O_TRUNC | O_CREAT)) ;
	rs = u_open(op->ifn,of,op->om) ;

	f_create = (op->of & O_CREAT) ;

	if ((rs == SR_NOENT) && f_create && (op->tfn != nullptr)) {
	    if ((rs = lineindex_mkindex(op)) >= 0) {
	        rs = u_open(op->ifn,of,op->om) ;
	    }
	}

	if (rs < 0)
	    goto bad0 ;

	op->fd = rs ;
	op->ti_open = dt ;

/* local or remote */

	rs = isfsremote(op->fd) ;
	f = (rs > 0) ;
	if (rs < 0)
	    goto bad1 ;

	f_changed = (! LEQUIV(f,op->f.remote)) ;
	if (f_changed)
	    op->f.remote = f ;

/* had the file itself changed? */

	if (rs >= 0) {

	    if ((rs = u_fstat(op->fd,&sb)) >= 0) {

	        if (! f_changed)
	            f_changed = (op->filesize != sb.st_size) ||
	                (op->mtime != sb.st_mtime) ;

	        op->filesize = (uint) sb.st_size ;
	        op->mtime = sb.st_mtime ;
	    }
	}

	} 

ret0:
	return (rs >= 0) ? f_changed : rs ;

/* bad things */
bad1:
	u_close(op->fd) ;
	op->fd = -1 ;

bad0:
	goto ret0 ;
}
/* end subroutine (lineindex_fileopen) */

static int lineindex_fileclose(LI *op) noex {
	int		rs = SR_OK ;
	if (op->fd >= 0) {
	    rs = u_close(op->fd) ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (lineindex_fileclose) */

static int lineindex_mkindex(LI *op) noex {
	filemap		lmap ;
	bfile		ifile ;
	off_t		headoff ;
	uint		recoff, lineoff ;
	uint		table[header_overlast + 1] ;
	uint		recs[NRECS + 1] ;
	int		rs = SR_OK ;
	int		i, cl, len ;
	int		headsize, lines ;
	int		sz ;
	cchar		*cp ;
	char		dfname[MAXPATHLEN + 1] ;
	char		pat[MAXPATHLEN + 1] ;
	char		tmpfname[MAXPATHLEN + 1] ;

/* determine if the directory is writable */

	if ((cl = sfdirname(op->ifn,-1,&cp)) > 0) {
	    mkpath1w(dfname,cp,cl) ;
	} else {
	    mkpath1(dfname,".") ;
	}

	rs = perm(dfname,-1,-1,nullptr,W_OK) ;
	if (rs < 0)
	    goto ret0 ;

	rs = filemap_open(&lmap,op->tfn,0uz) ;

	if (rs < 0)
	    goto ret0 ;

/* make a temporary file for the index file */

	mkpath2(pat,dfname,"liXXXXXXXXXXXX") ;

	rs = mktmpfile(tmpfname,op->om,pat) ;

	if (rs < 0)
	    goto ret1 ;

	rs = bopen(&ifile,tmpfname,"wct",op->om) ;

	if (rs < 0)
	    goto ret2 ;

/* write the header on the index file */

	{
	    int		fml = MIN(LINEINDEX_FILEMAGICLEN,15) ;
	    char	magicbuf[16 + 1] ;
	    char	vetu[4] ;

	    strncpy(magicbuf,LINEINDEX_FILEMAGIC,16) ;
	    magicbuf[fml] = '\n' ;

	    vetu[0] = LINEINDEX_FILEVERSION ;
	    vetu[1] = LINEINDEX_ENDIAN ;
	    vetu[2] = LINEINDEX_FILETYPE ;
	    vetu[3] = 0 ;

	    rs = bwrite(&ifile,magicbuf,16) ;

	    if (rs >= 0)
	    rs = bwrite(&ifile,vetu,4) ;

	    headoff = 20 ;
	    headsize = header_overlast * sizeof(uint) ;
	    recoff = 20 + headsize ;

/* seek past the header */

	    bseek(&ifile,headsize,SEEK_CUR) ;

	} /* end block */

/* OK, start reading file and writing index */

	lines = 0 ;
	lineoff = 0 ;
	i = 0 ;
	while (rs >= 0) {
		cchar	*lp ;

		rs = filemap_getln(&lmap,&lp) ;
	    len = rs ;
		if (rs <= 0)
			break ;

	    recs[i++] = htonl(lineoff) ;
	    if (i >= NRECS) {
	        sz = i * sizeof(uint) ;
	        bwrite(&ifile,recs,sz) ;
	        i = 0 ;
	    }

	    lineoff += len ;
	    lines += 1 ;

	} /* end while */

	if ((rs >= 0) && (i > 0)) {
	    sz = i * sizeof(uint) ;
	    rs = bwrite(&ifile,recs,sz) ;
	}

/* seek back and write the header */

	if (rs >= 0) {
	    time_t	dt = time(nullptr) ;
	    memset(table,0,headsize) ;
	    table[header_rectab] = htonl(recoff) ;
	    table[header_lines] = htonl(lines) ;
	    table[header_wtime] = htonl(dt) ;
	    if ((rs = bseek(&ifile,headoff,SEEK_SET)) >= 0) {
	        rs = bwrite(&ifile,table,headsize) ;
	    }
	} /* end block */

ret3:
	bclose(&ifile) ;

	if (rs >= 0) {

	    rs = u_rename(tmpfname,op->ifn) ;

	    if (rs >= 0)
		tmpfname[0] = '\0' ;

	} /* end if */

ret2:
	if (tmpfname[0] != '\0') {
	    u_unlink(tmpfname) ;
	}

ret1:

	filemap_close(&lmap) ;
ret0:
	return rs ;
}
/* end subroutine (lineindex_mkindex) */


