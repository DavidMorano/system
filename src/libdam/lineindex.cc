/* lineindex SUPPORT */
/* lang=C++20 */

/* line indexing object */
/* version %I% last-modified %G% */

#define	CF_SAFE		1		/* safe mode */
#define	CF_MEMSYNC	1		/* use memory synchronization? */
#define	CF_FILEMAP	1		/* use FILEMAP object */
#define	CF_TESTMODE	0		/* for testing */

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
	super-optimize anything here is not really work it.

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
#include	<mallocstuff.h>
#include	<bfile.h>
#include	<filemap.h>
#include	<intceil.h>
#include	<intsat.h>
#include	<localmisc.h>		/* |MODP2| */

#include	"lineindex.h"


/* local defines */

#define	TO_FILECOME	2
#define	TO_OPEN		(60 * 60)
#define	TO_MAP		(1 * 60)
#define	TO_CHECK	4
#define	TO_ACCESS	(1 * 60)

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	2048
#endif

#define	FILEMAPSIZE	(500 * 1024 * 1024)
#define	NRECS		(2 * 1024)


/* imported namespaces */


/* local typedefs */

typedef mode_t		om_t ;		/* open-mode */


/* external subroutines */


/* forward references */

static int	lineindex_mkindex(lineindex *) noex ;
static int	lineindex_fileheader(lineindex *) noex ;
static int	lineindex_filemap(lineindex *,time_t) noex ;
static int	lineindex_fileunmap(lineindex *) noex ;

static int	lineindex_fileopen(lineindex *,time_t) noex ;
static int	lineindex_fileclose(lineindex *) noex ;


/* local variables */

enum headers {
	header_rectab,
	header_wtime,
	header_lines,
	header_overlast
} ;


/* exported variables */


/* exported subroutines */

int lineindex_open(lineindex *op,cc *ifn,int of,om_t om,cc *tfn) noex {
	USTAT		sb ;

	time_t	dt = time(NULL) ;

	int	rs ;
	int	amode ;
	int	sz ;


	if (op == NULL)
	    return SR_FAULT ;

	if (ifn == NULL)
	    return SR_FAULT ;

	if (ifn[0] == '\0')
	    return SR_INVALID ;

	memclear(op) ;

	op->pagesize = getpagesize() ;
	op->fd = -1 ;
	op->of = of ;
	op->om = om ;

	amode = (of & O_ACCMODE) ;
	op->f.wantwrite = ((amode == O_WRONLY) || (amode == O_RDWR)) ;

/* store filename away */

	op->ifn = mallocstr(ifn) ;

	if (op->ifn == NULL) {
	    rs = SR_NOMEM ;
	    goto bad0 ;
	}

	if ((tfn != NULL) && (tfn[0] != '\0')) {
	    if ((op->tfn = mallocstr(tfn)) == NULL) {
	        rs = SR_NOMEM ;
	        goto bad1 ;
	    }

	}

/* try to open the file */

#if	CF_TESTMODE
	dt = 0 ;
#endif

	rs = lineindex_fileopen(op,dt) ;
	if (rs < 0)
	    goto bad2 ;

/* wait for the file to come in if it is not yet available */

	sz = 16 + 4 + (header_overlast * sizeof(int)) ;

	rs = u_fstat(op->fd,&sb) ;

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
	    op->f.fileinit = TRUE ;

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
	if (op->tfn != NULL)
	    uc_free(op->tfn) ;

bad1:
	uc_free(op->ifn) ;

bad0:
	goto ret0 ;

}
/* end subroutine (lineindex_open) */

int lineindex_count(lineindex *op) noex {
	if (op == NULL) return SR_FAULT ;

	if (op->magic != LINEINDEX_MAGIC) return SR_NOTOPEN ;

	return op->lines ;
}
/* end subroutine (lineindex_count) */

int lineindex_curbegin(lineindex *op,lineindex_cur *curp) noex {
	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != LINEINDEX_MAGIC) return SR_NOTOPEN ;

	op->cursors += 1 ;
	op->f.cursorlockbroken = FALSE ;
	op->f.cursoracc = FALSE ;
	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (lineindex_curbegin) */

int lineindex_curend(lineindex *op,lineindex_cur *curp) noex {
	const time_t	dt = time(NULL) ;

	if (op == NULL) return SR_FAULT ;
	if (curp == NULL) return SR_FAULT ;

	if (op->magic != LINEINDEX_MAGIC) return SR_NOTOPEN ;

#if	CF_TESTMODE
	dt = 0 ;
#endif

	if (op->f.cursoracc)
	    op->ti_access = dt ;

	if (op->cursors > 0)
	    op->cursors -= 1 ;

	curp->i = -1 ;

	return SR_OK ;
}
/* end subroutine (lineindex_curend) */

int lineindex_enum(lineindex *op,lineindex_cur *curp,off_t *rp) noex {
	int		rs = SR_OK ;
	int		ri ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;

	if (op->magic != LINEINDEX_MAGIC) return SR_NOTOPEN ;
#endif

	if (cup == NULL) return SR_FAULT ;

	if (op->cursors == 0) return SR_INVALID ;

	ri = (cup->i < 0) ? 0 : (cup->i + 1) ;

/* is the file mapped? */

	if (op->mapbuf == NULL) {
	    time_t	dt = time(NULL) ;

#if	CF_TESTMODE
	    dt = 0 ;
#endif

	    rs = lineindex_filemap(op,dt) ;

	    if (rs > 0)
	        rs = lineindex_fileheader(op) ;

	} /* end if (checking if file is mapped) */

	if (rs < 0)
	   goto ret0 ;

/* ok, we're good to go */

	if (ri >= op->lines)
	    return SR_NOTFOUND ;

	if (rp != NULL) {
	    *rp = ntohl(op->rectab[ri]) ;
	}

/* update the cursor */

	cup->i = ri ;

ret0:
	return (rs >= 0) ? ri : rs ;
}
/* end subroutine (lineindex_enum) */

int lineindex_lookup(lineindex *op,uint ri,off_t *rp) noex {
	int		rs = SR_OK ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
	if (op->magic != LINEINDEX_MAGIC) return SR_NOTOPEN ;
#endif

	if (rp == NULL) return SR_FAULT ;

	if (ri >= op->lines) return SR_NOTFOUND ;

/* is the file mapped? */

	if (op->mapbuf == NULL) {
	    time_t	dt = time(NULL) ;

#if	CF_TESTMODE
	    dt = 0 ;
#endif

	    rs = lineindex_filemap(op,dt) ;
	    if (rs > 0)
	        rs = lineindex_fileheader(op) ;

	} /* end if (checking if file is mapped) */

	if (rs < 0)
	    goto ret0 ;

/* do the real work */

	if (ri > 0)
	    *rp = ntohl(op->rectab[ri]) ;

ret0:
	return (rs >= 0) ? ri : rs ;
}
/* end subroutine (lineindex_lookup) */

int lineindex_check(lineindex *op,time_t dt) noex {
	USTAT		sb ;
	int		rs = SR_OK ;
	int		f_changed = FALSE ;
	cchar	*cp ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LINEINDEX_MAGIC) return SR_NOTOPEN ;

/* check things */

	if (op->cursors > 0)
	    goto ret0 ;

	if (op->mapbuf == NULL)
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

	    if (op->tfn != NULL) {

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
	f_changed = TRUE ;

closeit:
	op->ti_check = dt ;
	rs = lineindex_fileunmap(op) ;

	goto ret0 ;
}
/* end subroutine (lineindex_check) */


/* free up this lineindex object */
int lineindex_close(op)
LINEINDEX	*op ;
{
	int		rs = SR_BADFMT ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LINEINDEX_MAGIC) return SR_NOTOPEN ;

	if (op->mapbuf != NULL) {
	    rs = u_munmap(op->mapbuf,(size_t) op->mapsize) ;
	}

	if (op->fd >= 0) {
	    u_close(op->fd) ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (lineindex_close) */


/* private subroutines */


/* read the file header and check it out */
static int lineindex_fileheader(op)
LINEINDEX	*op ;
{
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


/* acquire access to the file (mapped memory) */
static int lineindex_filemap(op,dt)
LINEINDEX	*op ;
time_t		dt ;
{
	int		rs = SR_OK ;

	if (op->mapbuf == NULL) {

	    if ((rs = lineindex_fileopen(op,dt)) >= 0) {
		caddr_t	p ;
		size_t	ms ;
		int	mp, mf ;

	        op->mapsize = uceil(op->filesize,op->pagesize) ;

	        if (op->mapsize == 0)
	            op->mapsize = op->pagesize ;

		ms = (size_t) op->mapsize ;
	        mp = PROT_READ ;
	        mf = MAP_SHARED ;
	        if ((rs = u_mmap(NULL,ms,mp,mf,op->fd,0L,&p)) >= 0) {
		    op->mapbuf = p ;
	            op->ti_map = dt ;
		}

	        lineindex_fileclose(op) ;
	    } /* end if (opened the file) */

	} /* end if (mapping file) */

	return rs ;
}
/* end subroutine (lineindex_filemap) */


/* release our hold on the filemap */
static int lineindex_fileunmap(op)
LINEINDEX	*op ;
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->mapbuf != NULL) {
	    rs1 = u_munmap(op->mapbuf,(size_t) op->mapsize) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapbuf = NULL ;
	    op->mapsize = 0 ;
	} /* end if (checking existing map) */

	return rs ;
}
/* end subroutine (lineindex_fileunmap) */


static int lineindex_fileopen(op,dt)
LINEINDEX	*op ;
time_t		dt ;
{
	USTAT		sb ;

	int	rs ;
	int	of ;
	int	f ;
	int	f_create ;
	int	f_changed = FALSE ;

	if (op->fd < 0) {

	of = op->of ;
	of &= (~ (O_TRUNC | O_CREAT)) ;
	rs = u_open(op->ifn,of,op->om) ;

	f_create = (op->of & O_CREAT) ;

	if ((rs == SR_NOENT) && f_create && (op->tfn != NULL)) {
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

static int lineindex_fileclose(lineindex *op) noex {
	int		rs = SR_OK ;
	if (op->fd >= 0) {
	    rs = u_close(op->fd) ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (lineindex_fileclose) */

static int lineindex_mkindex(lineindex *op) noex {
	filemap		lmap ;
	bfile		ifile ;
	off_t		headoff ;
	uint		recoff, lineoff ;
	uint		table[header_overlast + 1] ;
	uint		recs[NRECS + 1] ;
	int		rs = SR_OK ;
	int		i, cl, len ;
	int		headsize, lines ;
	int		size ;
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

	rs = perm(dfname,-1,-1,NULL,W_OK) ;
	if (rs < 0)
	    goto ret0 ;

#if	CF_FILEMAP
	rs = filemap_open(&lmap,op->tfn,FILEMAPSIZE) ;
#else
	rs = bopen(&lfile,op->tfn,"r",0666) ;

#endif /* CF_FILEMAP */

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
	    char	magicbuf[16 + 1], vetu[4] ;

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

#if	CF_FILEMAP
		rs = filemap_getln(&lmap,&lp) ;
	    len = rs ;
		if (rs <= 0)
			break ;

#else /* CF_FILEMAP */
	{
	    char	linebuf[LINEBUFLEN+1] ;

		rs = breadln(&lfile,linebuf,LINEBUFLEN) ;
	    len = rs ;
		if (rs <= 0)
			break ;

	    if (linebuf[len - 1] != '\n') {
	        rs = bwastelln(&lfile,linebuf,LINEBUFLEN) ;
	        if (rs > 0) len += rs ;
	    }
	    }
#endif /* CF_FILEMAP */

	    recs[i++] = htonl(lineoff) ;
	    if (i >= NRECS) {

	        size = i * sizeof(uint) ;
	        bwrite(&ifile,recs,size) ;

	        i = 0 ;
	    }

	    lineoff += len ;
	    lines += 1 ;

	} /* end while */

	if ((rs >= 0) && (i > 0)) {
	    size = i * sizeof(uint) ;
	    rs = bwrite(&ifile,recs,size) ;
	}

/* seek back and write the header */

	if (rs >= 0) {
	    time_t	dt = time(NULL) ;


#if	CF_TESTMODE
	    dt = 0 ;
#endif

	    memset(table,0,headsize) ;

	    table[header_rectab] = htonl(recoff) ;
	    table[header_lines] = htonl(lines) ;

#if	CF_TESTMODE
	    table[header_wtime] = 0 ;
#else
	    table[header_wtime] = htonl(dt) ;
#endif

	    if ((rs = bseek(&ifile,headoff,SEEK_SET)) >= 0)
	        rs = bwrite(&ifile,table,headsize) ;

	} /* end block */

ret3:
	bclose(&ifile) ;

	if (rs >= 0) {

	    rs = u_rename(tmpfname,op->ifn) ;

	    if (rs >= 0)
		tmpfname[0] = '\0' ;

	} /* end if */

ret2:
	if (tmpfname[0] != '\0')
	    u_unlink(tmpfname) ;

ret1:

#if	CF_FILEMAP
	filemap_close(&lmap) ;
#else
	bclose(&lfile) ;
#endif /* CF_FILEMAP */

ret0:
	return rs ;
}
/* end subroutine (lineindex_mkindex) */


