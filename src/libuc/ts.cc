/* ts SUPPORT (Time-Stamp) */
/* lang=C++20 */

/* time-stamp file manager */
/* version %I% last-modified %G% */

#define	CF_SAFE		1		/* safer? */
#define	CF_CREAT	0		/* always create the file? */
#define	CF_LOCKF	0		/* use 'lockf(3c)' */
#define	CF_SOLARISBUG	1		/* work around Solaris MMAP bug */
#define	CF_NIENUM	0		/* perform NI updates on ENUM */
#define	CF_NISEARCH	0		/* perform NI updates on SEARCH */

/* revision history:

	= 1991-06-01, David A­D­ Morano
	This subroutine was originally written.

	= 2003-06-26, David A­D­ Morano
	Although this object works, it was only a micracle that it
	did. There is a feature-bug in Solaris that doesn't allow
	a file to be both mapped and locked at the same time (in
	either order). But there seems to be a crack in the stupid
	Solaris implementation because it doesn't enforce its stupid
	bug carefully enough and this object here fell through the
	cracks and continued working by accident. We were locking
	the whole file beyond its end and that appears to get by
	the Solaris police-state bug-patrol and was accidentally
	being allowed.  I reworked a good bit of this code to
	eliminate any file mapping (so that we can continue to use
	file-record locks). This whole Solaris crap (this is being
	done on Solaris 8 right now) is really a pain and Sun should
	face punitive charges for inhumanity to the programmer
	community.  Solaris has some nice things since it was derived
	from the older (and better?) System V UNIX®, but has really
	messed it up by not allowing what used to be allowed in the
	old days with things like the old RFS facility. Oh, while
	we're on the subject: NFS sucks cock meat!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This subroutine maintains a TS file.  

	Format of file records:

	- (see the 'entry' structure in the header)

	Design note: 

	In summary, Solaris sucks cock meat! Solaris does not allow
	a file to be memory-mapped from an NFS remote server *and*
	also be file-locked at the same time. A lot of stupid Solaris
	documentation notes say something to the effect that the
	Solaris VM system cannot handle a remote file that is both
	mapped and subject to file-locking at the same time. They
	use some sort of stupid circular reasoning that if any file
	is being file-locked, then obviously it cannot be memory-mapped
	since the file locking indicates that file-locking is taking
	place, and that obviously any file that is being file-locked
	cannot therefore also be memory mapped. That is pretty much
	their reasoning -- I kid you not!

	Unfortunately, code, like this code here, that was first
	designed under System V UNIX® that used file-locking *and*
	memory mapping together really needs to be changed to
	eliminate either the file locking or the memory mapping.
	Remote files were cross mounted in the late 80s and very
	early 90s using RFS (not stupid NFS). The use of RFS provided
	many advantages not the least of them being full UFS
	file-system semantics, but it is not clear why Solaris took
	a step backward from simply allowing remote files to be
	both memory-mapped and file-locked at the same time. Some
	bright light-bulb of a software developer must have gotten
	his underwear in a bunch at some point and decided to
	disallow both of these from ever occurring at the same time
	in Solaris. We all have suffered from these dumb-butt Solaris
	developers since we have to take time out to re-debug-write
	old code (like this code here) to handle the case of stupid
	Solaris not allowing memory mapping for a file that is also
	file-locked.

	Implementation note:

	The code was actually running when files were being locked
	in their entirety and beyond their ends. There was some
	sort of loop-hole in the stupid Solaris code that allowed
	a file to be both file-locked and memory mapped at the same
	time under certain circumstances. However, there seemed to
	be problems with this code when other parties on other
	(remote) systems tried to do the same thing. They sometimes
	failed with dead-lock types of errors (I forget the details).
	As a result, I decided to change the code to fully comply
	with the stupid Solaris requirements that no remote file
	be both memory mapped and file locked at the same time. Any
	code that is here now that has to be with mapping of files
	is really just code that now allocates local private memory.
	This is done instead of using the process heap but was
	really done because it seemed to offer the minimal changes
	to the code to get a private memory access to the file while
	still retaining the ability to file-lock the file.  Finally,
	let me finish with the comment that Solaris sucks cock meat.

	Final note:

	Solaris sucks cock meat! Give me back simultaneous memory
	mapping and file locking. And while you're at it, give me
	back RFS also! And to you stupid Solaris VM developers, get
	out of Solaris development. Either get a new job somewhere
	else or think about committing suicide. Either way, we can
	all be happier with one (or more) of those alternatives.

	Anecdotal note:

	Hey, you stupid Solaris developers: give me back the ability
	to push SOCKMOD on a TPI endpoint also! Since you're so
	stupid, I know that you forgot that this was possible at
	one time. You hosed that ability away when you botched up
	making Solaris 2.6.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstdlib>
#include	<cinttypes>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<vecstr.h>
#include	<mapstrint.h>
#include	<stdorder.h>
#include	<localmisc.h>
#include	<hash.h>
#include	<strwcpy.h>
#include	<strdcpyx.h>
#include	<matxstr.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"ts.h"
#include	"tse.h"
#include	"ebuf.h"


/* local defines */

#define	TS_IDOFF	0
#define	TS_HEADTABOFF	(TS_IDOFF + (16 + sizeof(uint)))
#define	TS_TABOFF	(TS_HEADTABOFF + (3 * sizeof(uint)))

#define	TS_ENTSIZE	TSE_SIZE
#define	TS_MAXFILESIZE	(4 * 1024 * 1024)
#define	TS_NWAYS	4
#define	TS_NEPW		(((8*1024) / TS_ENTSIZE) - 1)
#define	TS_NIDXENT	100

#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		30		/* seconds */


/* external subroutines */

extern int	lockfile(int,int,off_t,off_t,int) ;
extern int	getfstype(char *,int,int) ;
extern int	isNotPresent(int) ;

extern char	*timestr_log(time_t,char *) ;


/* external variables */


/* forward references */

static int	ts_fileopen(TS *,time_t) ;
static int	ts_fileclose(TS *) ;
static int	ts_filesetinfo(TS *,time_t) ;
static int	ts_lockget(TS *,time_t,int) ;
static int	ts_lockrelease(TS *) ;
static int	ts_filebegin(TS *,time_t) ;
static int	ts_acquire(TS *,time_t,int) ;
static int	ts_filecheck(TS *,time_t) ;
static int	ts_ebufstart(TS *) ;
static int	ts_ebuffinish(TS *) ;

static int	ts_filetopwrite(TS *,time_t) ;
static int	ts_filetopread(TS *) ;
static int	ts_fileverify(TS *) ;
static int	ts_headtab(TS *,int) ;

static int	ts_findname(TS *,const char *,int,char **) ;
static int	ts_search(TS *,const char *,int,char **) ;
static int	ts_readentry(TS *,int,char **) ;

#if	CF_NISEARCH
static int	ts_index(TS *,const char *,int,int) ;
#endif

static int	ts_headwrite(TS *) ;

static int	namematch(const char *,const char *,int) ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ts_open(TS *op,cchar *fname,int oflags,mode_t operm) noex {
	time_t		dt = time(nullptr) ;
	int		rs ;
	int		f_created = false ;
	cchar		*cp ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
#endif /* CF_SAFE */

	if (fname == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	memclear(op) ;
	op->pagesize = getpagesize() ;
	op->fd = -1 ;
	op->oflags = oflags ;
	op->operm = operm ;

#if	CF_CREAT
	oflags |= O_CREAT ;
#endif
	oflags = (oflags &= (~ O_TRUNC)) ;
	if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	    op->fname = cp ;
/* try to open the file */
	    if ((rs = ts_fileopen(op,dt)) >= 0) {
		if ((rs = ts_filebegin(op,dt)) >= 0) {
		    if ((rs = mapstrint_start(&op->ni,TS_NIDXENT)) >= 0) {
			op->magic = TS_MAGIC ;
		    }
		}
		if (rs < 0) {
		    ts_fileclose(op) ;
		}
	    }
	    if (rs < 0) {
	        uc_free(op->fname) ;
	        op->fname = nullptr ;
	    }
	}

	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (ts_open) */


int ts_close(TS *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	rs1 = mapstrint_finish(&op->ni) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = ts_fileclose(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->fname != nullptr) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (ts_close) */


/* get a count of the number of entries */
int ts_count(TS *op)
{
	int		rs = SR_OK ;
	int		c ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	c = (op->filesize - TS_TABOFF) / TS_ENTSIZE ;

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ts_count) */


/* initialize a cursor */
int ts_curbegin(TS *op,TS_CUR *cp)
{

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
	if (cp == nullptr) return SR_FAULT ;

	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	op->ncursors += 1 ;

	op->f.cursorlockbroken = false ;
	op->f.cursoracc = false ;
	cp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (ts_curbegin) */


/* free up a cursor */
int ts_curend(TS *op,TS_CUR *cp)
{
	int		rs = SR_OK ;
	int		rs1 ;
	time_t		dt = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (cp == nullptr) return SR_FAULT ;

	if (op->f.cursoracc) {
	    dt = time(nullptr) ;
	    op->ti_access = dt ;
	} /* end if */

	if (op->ncursors > 0)
	    op->ncursors -= 1 ;

	if ((op->ncursors == 0) && (op->f.lockedread || op->f.lockedwrite)) {
	    rs1 = ts_lockrelease(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	cp->i = -1 ;
	return rs ;
}
/* end subroutine (ts_curend) */


/* enumerate the entries */
int ts_enum(TS *op,TS_CUR *curp,TS_ENT *ep)
{
	time_t		dt = 0 ;
	const int	ebs = TS_ENTSIZE ;
	int		rs = SR_OK ;
	int		ei ;
	char		*bp ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (curp == nullptr) return SR_FAULT ;

	if (dt == 0) dt = time(nullptr) ;

	rs = ts_acquire(op,dt,1) ;

	if (rs < 0)
	     goto ret0 ;

/* OK, give an entry back to caller */

	ei = (curp->i < 0) ? 0 : curp->i + 1 ;

	rs = ts_readentry(op,ei,&bp) ;

/* copy entry to caller buffer */

	if ((rs >= 0) && (ep != nullptr) && (bp != nullptr)) {
	    rs = tse_all(ep,1,bp,ebs) ;
	} /* end if */

/* commit the cursor movement? */

	if (rs >= 0)
	    curp->i = ei ;

	op->f.cursoracc = true ;

ret0:

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_enum) */


/* match on a key-name */
int ts_match(op,dt,nnp,nnl,ep)
TS		*op ;
time_t		dt ;
const char	nnp[] ;
int		nnl ;
TS_ENT	*ep ;
{
	const int	ebs = TS_ENTSIZE ;
	int		rs = SR_OK ;
	int		i ;
	int		ei = 0 ;
	char		*bp ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (nnp == nullptr) return SR_FAULT ;

	i = TS_KEYNAMELEN ;
	if (nnl >= 0) {
	    i = MIN(nnl,TS_KEYNAMELEN) ;
	}
	nnl = strnlen(nnp,i) ;

	if (dt == 0) dt = time(nullptr) ;

	rs = ts_acquire(op,dt,1) ;
	if (rs < 0)
	     goto ret0 ;

	rs = ts_findname(op,nnp,nnl,&bp) ;
	ei = rs ;

	if ((rs >= 0) && (ep != nullptr)) {
	    rs = tse_all(ep,1,bp,ebs) ;
	} /* end if */

/* optionally release our lock if we didn't have a cursor outstanding */

	if (op->ncursors == 0) {
	    ts_lockrelease(op) ;
	}

/* update access time as appropriate */

	if (op->ncursors == 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    op->ti_access = dt ;
	} else {
	    op->f.cursoracc = true ;
	}

/* we're out of here */
done:
ret0:

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_match) */


/* write an entry (match on a key-name) */
int ts_write(op,dt,nnp,nnl,ep)
TS		*op ;
time_t		dt ;
const char	nnp[] ;
int		nnl ;
TS_ENT	*ep ;
{
	const int	ebs = TS_ENTSIZE ;

	int		rs = SR_OK ;
	int		i ;
	int		ei ;
	int		f_newentry = false ;
	char		*bp ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (nnp == nullptr) return SR_FAULT ;

#ifdef	COMMENT
	if (ep == nullptr) return SR_FAULT ;
#endif

	i = TS_KEYNAMELEN ;
	if (nnl >= 0) {
	    i = MIN(nnl,TS_KEYNAMELEN) ;
	}
	nnl = strnlen(nnp,i) ;

	rs = ts_acquire(op,dt,1) ;
	if (rs < 0)
	     goto ret0 ;

	rs = ts_findname(op,nnp,nnl,&bp) ;
	ei = rs ;

/* write the entry */

	if (dt == 0) dt = time(nullptr) ;

	if (rs >= 0) {
	    TS_ENT	ew ;

  	    if (ep != nullptr) {
		ew = *ep ;
	    } else {
		memset(&ew,0,sizeof(TS_ENT)) ;
	    }

	    if (ew.count == 0) ew.count = 1 ;
	    if (ew.utime == 0) ew.utime = dt ;
	    if (ew.ctime == 0) ew.ctime = dt ;
	    if (ew.keyname[0] == '\0') {
		strdcpy1w(ew.keyname,TSE_LKEYNAME,nnp,nnl) ;
	    }
	    if (ew.hash == 0) {
		ew.hash = hash_elf(ew.keyname,-1) ;
	    }

	    tse_all(&ew,0,bp,ebs) ;

	    rs = ebuf_write(&op->ebm,ei,nullptr) ; /* sync */

	} else if (rs == SR_NOTFOUND) {
	    TS_ENT	ew ;
	    char	ebuf[TS_ENTSIZE + 2] ;

	    f_newentry = true ;
  	    if (ep != nullptr) {
		ew = *ep ;
	    } else {
		memset(&ew,0,sizeof(TS_ENT)) ;
	    }

	    if (ew.count == 0) ew.count = 1 ;
	    if (ew.utime == 0) ew.utime = dt ;
	    if (ew.ctime == 0) ew.ctime = dt ;
	    if (ew.keyname[0] == '\0') {
		strdcpy1w(ew.keyname,TSE_LKEYNAME,nnp,nnl) ;
	    }
	    if (ew.hash == 0) ew.hash = hash_elf(ew.keyname,-1) ;

	    tse_all(&ew,0,ebuf,ebs) ;

	    ei = op->h.nentries ;
	    rs = ebuf_write(&op->ebm,ei,ebuf) ;

	} /* end if (existing or new entry) */

/* update the file header-table (for a write) */

	if ((rs >= 0) && op->f.writable) {

#ifdef	COMMENT
	    if (dt == 0) dt = time(nullptr) ;
#endif

	    op->h.wcount += 1 ;
	    op->h.wtime = dt ;
	    if (f_newentry) {
	        op->h.nentries += 1 ;
	        op->filesize += ebs ;
	    }
	    if ((rs = ts_headwrite(op)) >= 0) {
		rs = ebuf_sync(&op->ebm) ;
	    }

	} /* end if (updating header-table) */

/* optionally release our lock if we didn't have a cursor outstanding */

	if (op->ncursors == 0)
	    ts_lockrelease(op) ;

/* update access time as appropriate */

	if (op->ncursors == 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    op->ti_access = dt ;
	} else {
	    op->f.cursoracc = true ;
	}

/* we're out of here */
done:
ret0:

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_write) */


/* update an entry */
int ts_update(op,dt,ep)
TS		*op ;
time_t		dt ;
TS_ENT		*ep ;
{
	const int	ebs = TS_ENTSIZE ;

	int		rs = SR_OK ;
	int		rs1 ;
	int		nnl ;
	int		ei ;
	int		f_newentry = false ;
	const char	*nnp ;
	char		ebuf[TS_ENTSIZE + 2] ;
	char		*bp ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (ep == nullptr) return SR_FAULT ;

	if (ep->keyname[0] == '\0') return SR_INVALID ;

	rs = ts_acquire(op,dt,1) ;

	if (rs < 0)
	     goto ret0 ;

/* do the search */

	nnp = ep->keyname ;
	nnl = strnlen(nnp,TS_KEYNAMELEN) ;

	if (nnl == 0) {
	    rs = SR_INVALID ;
	    goto ret2 ;
	}

	rs = ts_findname(op,nnp,nnl,&bp) ;
	ei = rs ;

/* update the entry that we found and write it back */

	if (rs >= 0) {
	    TS_ENT	ew ;

	    tse_all(&ew,1,bp,ebs) ;

	    ew.utime = dt ;
	    ew.count += 1 ;
	    tse_all(&ew,0,bp,ebs) ;

	    rs = ebuf_write(&op->ebm,ei,nullptr) ; /* sync */

	} else if (rs == SR_NOTFOUND) {
	    TS_ENT	ew = *ep ;

	    f_newentry = true ;
	    if (ew.count == 0) ew.count = 1 ;
	    if (ew.utime == 0) ew.utime = dt ;
	    if (ew.ctime == 0) ew.ctime = dt ;
	    ew.hash = hash_elf(ew.keyname,-1) ;

	    tse_all(&ew,0,ebuf,ebs) ;

	    ei = op->h.nentries ;
	    rs = ebuf_write(&op->ebm,ei,ebuf) ;

	} /* end if (entry update) */

/* update the file header */

	if (rs >= 0) {

	    if (dt == 0)
	        dt = time(nullptr) ;

	    op->h.wcount += 1 ;
	    op->h.wtime = dt ;
	    if (f_newentry) {
	        op->h.nentries += 1 ;
	        op->filesize += ebs ;
	    }

	    rs = ts_headwrite(op) ;

	    if (rs >= 0)
		rs = ebuf_sync(&op->ebm) ;

	} /* end if (updating header-table) */

/* optionally release our lock if we didn't have a cursor outstanding */
ret2:

	if (op->ncursors == 0) {
	    rs1 = ts_lockrelease(op) ;
	} /* end if */

/* update access time as appropriate */
ret1:
	if (op->ncursors == 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    op->ti_access = dt ;
	} else
	    op->f.cursoracc = true ;

/* we're out of here */
done:
ret0:

	return (rs >= 0) ? ei : rs ;

/* bad stuff */
bad1:
	if (op->ncursors == 0)
	    ts_lockrelease(op) ;

bad0:
	goto done ;
}
/* end subroutine (ts_update) */


/* do some checking */
int ts_check(TS *op,time_t dt)
{
	int		rs = SR_OK ;
	int		f = false ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TS_MAGIC) return SR_NOTOPEN ;
#endif /* CF_SAFE */

	if (op->fd < 0) return SR_OK ;

	if ((! op->f.lockedread) && (! op->f.lockedwrite)) {
	    if (dt == 0) dt = time(nullptr) ;
	    f = ((dt - op->ti_access) >= TO_ACCESS) ;
	    f = f || ((dt - op->ti_open) >= TO_OPEN) ;
	    if (f) {
	        rs = ts_fileclose(op) ;
	    }
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ts_check) */


/* private subroutines */


static int ts_findname(op,nnp,nnl,rpp)
TS		*op ;
const char	*nnp ;
int		nnl ;
char		**rpp ;
{
	int		rs ;
	int		ei = 0 ;
	char		*bp = nullptr ;
	char		*np ;

	rs = mapstrint_fetch(&op->ni,nnp,nnl,nullptr,&ei) ;

	if (rs >= 0) {

	    rs = ebuf_read(&op->ebm,ei,&bp) ;

	    if (rs > 0) {

	        np = bp + TSE_OKEYNAME ;
	        if (! namematch(np,nnp,nnl)) {

	            rs = SR_NOTFOUND ;
	            mapstrint_delkey(&op->ni,nnp,nnl) ;

	        } /* end if */

	    } else
	        rs = SR_NOTFOUND ;

	} /* end if (was in the index) */

	if (rs == SR_NOTFOUND) {

	    rs = ts_search(op,nnp,nnl,&bp) ;
	    ei = rs ;
	    if (rs >= 0) {
	        rs = mapstrint_add(&op->ni,nnp,nnl,ei) ;
	    }

	} /* end if */

	if (rpp != nullptr)
	    *rpp = (rs >= 0) ? bp : nullptr ;

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_findname) */


/* search for an entry */
static int ts_search(op,nnp,nnl,rpp)
TS		*op ;
const char	nnp[] ;
int		nnl ;
char		**rpp ;
{
	const int	ebs = TS_ENTSIZE ;
	int		rs = SR_OK ;
	int		i ;
	int		ne, ei ;
	int		f_found ;
	char		*bp ;
	char		*np = nullptr ;

	if (nnl < 0)
	    nnl = strlen(nnp) ;

	ei = 0 ;
	ne = 0 ;
	f_found = false ;
	while ((rs >= 0) && (! f_found)) {

	    rs = ebuf_read(&op->ebm,ei,&bp) ;
	    ne = rs ;

	    if (rs <= 0)
	        break ;

	    for (i = 0 ; (rs >= 0) && (i < ne) ; i += 1) {

	        np = bp + TSE_OKEYNAME ;

/* is this a match for what we want? */

	        if (namematch(np,nnp,nnl)) {
		    f_found = true ;
	            break ;
		}

		bp += ebs ;

	    } /* end for (looping through entries) */

	    ei += i ;

	    if (f_found) break ;
	} /* end while */

	if (rs >= 0) {
	    if ((ne != 0) && f_found) {
		if (rpp != nullptr)
		    *rpp = bp ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if */

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (ts_search) */


static int ts_acquire(op,dt,f_read)
TS		*op ;
int		f_read ;
time_t		dt ;
{
	const int	ebs = TS_ENTSIZE ;

	int		rs = SR_OK ;
	int		f_changed = false ;
	int		f ;

	if (dt == 0) dt = time(nullptr) ;

/* is the file open? */

	if (op->fd < 0) {
	    rs = ts_fileopen(op,dt) ;
	}

	if ((rs >= 0) && (! f_read) && op->f.lockedread) {
	    rs = ts_lockrelease(op) ;
	}

/* capture the lock if we do not already have it */

	f = (op->f.lockedread || op->f.lockedwrite) ;
	if ((rs >= 0) && (! f)) {
	    if ((rs = ts_lockget(op,dt,f_read)) >= 0) {
	        f_changed = (rs > 0) ;
	        rs = ts_filecheck(op,dt) ;
	        f_changed = f_changed || (rs > 0) ;
	        if ((rs >= 0) && f_changed) {
		    int n = (op->filesize - TS_TOPLEN) / ebs ;
	            rs = ebuf_invalidate(&op->ebm,n) ;
		}
	    }
	} /* end if (need lock) */

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (ts_acquire) */


/* initialize the file header (either read it only or write it) */
static int ts_filebegin(op,dt)
TS		*op ;
time_t		dt ;
{
	int		rs = SR_OK ;
	int		f_locked = false ;
	int		f ;

	if (op->filesize == 0) {

	    op->f.fileinit = false ;
	    if (op->f.writable) {

		if (op->f.lockedread) {
		    rs = ts_lockrelease(op) ;
		}

	        if ((rs >= 0) && (! op->f.lockedwrite)) {
	            f_locked = true ;
	            rs = ts_lockget(op,dt,0) ;
	        }

		if (rs >= 0) {
	            rs = ts_filetopwrite(op,dt) ;
	            f_locked = (rs > 0) ;
		}

	    } /* end if (writable) */

	} else if (op->filesize >= TS_TABOFF) {

/* read the file header */

	    f = (op->f.lockedread || op->f.lockedwrite) ;
	    if (! f) {
	        rs = ts_lockget(op,dt,1) ;
	        f_locked = (rs >= 0) ;
	    }

	    if (rs >= 0)
	        rs = ts_filetopread(op) ;

	    if (rs >= 0)
	        rs = ts_fileverify(op) ;

	    if (rs >= 0) {
	        rs = ts_headtab(op,1) ;
	        op->f.fileinit = (rs >= 0) ;
	    }

	} /* end if */

/* if we locked, we unlock it, otherwise leave it! */

	if (f_locked) {
	    ts_lockrelease(op) ;
	}

/* we're out of here */
ret0:

	return rs ;
}
/* end subroutine (ts_filebegin) */


static int ts_filecheck(op,dt)
TS		*op ;
time_t		dt ;
{
	int	rs = SR_OK ;
	int	f_changed = false ;


	if (op->filesize < TS_TABOFF) {

	    f_changed = true ;
	    if (op->f.writable)
		rs = ts_filetopwrite(op,dt) ;

	} else {

	    rs = ts_filetopread(op) ;

	    if (rs >= 0)
	        rs = ts_fileverify(op) ;

	    if (rs >= 0) {
	        rs = ts_headtab(op,1) ;
		f_changed = (rs > 0) ;
	        op->f.fileinit = (rs >= 0) ;
	    }

	} /* end if */

/* we're out of here */
ret0:
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (ts_filecheck) */


static int ts_filetopwrite(op,dt)
TS		*op ;
time_t		dt ;
{
	off_t	poff ;

	int	rs = SR_OK ;
	int	bl ;

	char	*bp ;


/* write the file header stuff */

	        bp = op->topbuf ;

	        bp = strwcpy(bp,TS_FILEMAGIC,15) ;
	        *bp++ = '\n' ;

	        bl = (16 - (bp - op->topbuf)) ;
	        memset(bp,0,bl) ;
	        bp += bl ;

	        *bp++ = TS_FILEVERSION ;
	        *bp++ = TS_ENDIAN ;
	        *bp++ = 0 ;		/* file type */
	        *bp++ = 0 ;		/* unused */

/* next is the header (we just write zeros here) */

	        memset(bp,0,TS_HEADTABLEN) ;
	        bp += TS_HEADTABLEN ;

	        bl = bp - op->topbuf ;
	        op->fileversion = TS_FILEVERSION ;
	        op->filetype = 0 ;
	        memset(&op->h,0,sizeof(struct ts_h)) ;

	poff = 0L ;
	rs = u_pwrite(op->fd,op->topbuf,bl,poff) ;
	op->filesize = rs ;
	op->topsize = rs ;
	op->f.fileinit = (rs >= 0) ;

ret0:
	return rs ;
}
/* end subroutine (ts_filetopwrite) */


static int ts_filetopread(op)
TS		*op ;
{
	off_t	poff ;

	int	rs ;


	poff = 0L ;
	rs = u_pread(op->fd,op->topbuf,TS_TOPLEN,poff) ;
	op->topsize = rs ;

	return rs ;
}
/* end subroutine (ts_filetopread) */

static int ts_fileverify(TS *op) noex {
	static cint	magl = strlen(TS_FILEMAGIC) ;
	int		rs = SR_OK ;
	int		f ;
	cchar		*magp = TS_FILEMAGIC ;
	cchar		*cp ;

	if (op->topsize < TS_TOPLEN) {
	    rs = SR_INVALID ;
	    goto ret0 ;
	}

	cp = op->topbuf ;
	f = (strncmp(cp,magp,magl) == 0) ;
	f = f && (*(cp + magl) == '\n') ;

	if (! f) {

	    rs = SR_BADFMT ;
	    goto ret0 ;
	}

	cp += 16 ;
	if (cp[0] > TS_FILEVERSION) {
	    rs = SR_NOTSUP ;
	    goto ret0 ;
	}

	op->fileversion = cp[0] ;

	if (cp[1] != TS_ENDIAN) {
	    rs = SR_NOTSUP ;
	    goto ret0 ;
	}

	op->filetype = cp[2] ;

ret0:

	return rs ;
}
/* end subroutine (ts_fileverify) */


/* read or write the file header */
static int ts_headtab(op,f_read)
TS		*op ;
int		f_read ;
{
	int	rs = SR_OK ;
	int	f_changed = false ;

	char	*bp = (op->topbuf + TS_HEADTABOFF) ;


	if (op->topbuf == nullptr)
	    return SR_BADFMT ;

	if (f_read) {
	    struct ts_h	h ;

	    int	hsize = sizeof(struct ts_h) ;

	    stdorder_rui(bp,&h.nentries) ;
	    bp += sizeof(uint) ;

	    stdorder_rui(bp,&h.wtime) ;
	    bp += sizeof(uint) ;

	    stdorder_rui(bp,&h.wcount) ;
	    bp += sizeof(uint) ;

	    f_changed = (memcmp(&h,&op->h,hsize) != 0) ;
	    op->h = h ;

	} else {

	    stdorder_wui(bp,op->h.nentries) ;
	    bp += sizeof(uint) ;

	    stdorder_wui(bp,op->h.wtime) ;
	    bp += sizeof(uint) ;

	    stdorder_wui(bp,op->h.wcount) ;
	    bp += sizeof(uint) ;

	} /* end if */

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (ts_headtab) */


/* acquire access to the file */
static int ts_lockget(op,dt,f_read)
TS		*op ;
int		f_read ;
time_t		dt ;
{
	USTAT	sb ;

	int	rs = SR_OK ;
	int	lockcmd ;
	int	f_already = false ;
	int	f_changed = false ;


	if (op->fd < 0) {

	    rs = ts_fileopen(op,dt) ;

	    if (rs < 0)
	        goto bad0 ;

	} /* end if (needed to open the file) */

/* acquire a file record lock */

	if (f_read || (! op->f.writable)) {

	    f_already = op->f.lockedread ;
	    op->f.lockedread = true ;
	    op->f.lockedwrite = false ;
	    lockcmd = F_RLOCK ;

	} else {

	    f_already = op->f.lockedwrite ;
	    op->f.lockedread = false ;
	    op->f.lockedwrite = true ;
	    lockcmd = F_WLOCK ;

	} /* end if */

/* get out if we have the lock that we want already */

	if (f_already)
	    goto ret0 ;

/* we need to actually do the lock */

#if	CF_LOCKF
	rs = uc_lockf(op->fd,F_LOCK,0L) ;
#else /* CF_LOCKF */
#if	CF_SOLARISBUG
	rs = lockfile(op->fd,lockcmd,0L,0L,TO_LOCK) ;
#else
	{
	    off_t	fs = op->filesize ;
	    rs = lockfile(op->fd,lockcmd,0L,fs,TO_LOCK) ;
	}
#endif /* CF_SOLARISBUF */
#endif /* CF_LOCKF */

	if (rs < 0)
	    goto bad2 ;

/* has the file changed at all? */

	rs = u_fstat(op->fd,&sb) ;

#ifdef	COMMENT
	if (rs == SR_NOENT)
	    rs = SR_OK ;
#endif /* COMMENT */

	if (rs < 0)
	    goto bad2 ;

	f_changed = 
	    (sb.st_size != op->filesize) ||
	    (sb.st_mtime != op->ti_mod) ;

	if (f_changed) {

	    if (op->f.bufvalid)
	        op->f.bufvalid = false ;

	    op->filesize = sb.st_size ;
	    op->ti_mod = sb.st_mtime ;

	} /* end if */

	if (op->filesize < TS_TABOFF)
	    op->f.fileinit = false ;

ret0:

	return (rs >= 0) ? f_changed : rs ;

/* bad stuff */
bad2:
	op->f.fileinit = false ;

#if	CF_LOCKF
	rs = uc_lockf(op->fd,F_ULOCK,0L) ;
#else /* CF_LOCKF */
#if	CF_SOLARISBUG
	lockfile(op->fd,F_ULOCK,0L,0L,TO_LOCK) ;
#else
	{
	    off_t	fs = op->filesize ;
	    lockfile(op->fd,F_ULOCK,0L,fs,TO_LOCK) ;
	}
#endif /* CF_SOLARISBUF */
#endif /* CF_LOCKF */

bad1:
	op->f.lockedread = false ;
	op->f.lockedwrite = false ;

bad0:
	goto ret0 ;
}
/* end subroutine (ts_lockget) */


static int ts_lockrelease(op)
TS		*op ;
{
	int	rs = SR_OK ;


	if ((op->f.lockedread || op->f.lockedwrite)) {

	    if (op->fd >= 0) {

#if	CF_LOCKF
	        rs = uc_lockf(op->fd,F_ULOCK,0L) ;
#else /* CF_LOCKF */
#if	CF_SOLARISBUG
	        rs = lockfile(op->fd,F_ULOCK,0L,0L,TO_LOCK) ;
#else
		{
		    off_t	fs = op->filesize ;
	            rs = lockfile(op->fd,F_ULOCK,0L,fs,TO_LOCK) ;
		}
#endif /* CF_SOLARISBUF */
#endif /* CF_LOCKF */

	    } /* end if (file was open) */

	    op->f.lockedread = false ;
	    op->f.lockedwrite = false ;

	} /* end if (there was a possible lock set) */

	return rs ;
}
/* end subroutine (ts_lockrelease) */


static int ts_fileopen(op,dt)
TS		*op ;
time_t		dt ;
{
	int	rs = SR_OK ;
	int	oflags ;
	int	f_created = false ;
	if (op->fd >= 0)
	    goto ret0 ;

	op->f.fileinit = false ;
	op->f.lockedread = false ;
	op->f.lockedwrite = false ;

	oflags = (op->oflags & (~ O_CREAT)) ;
	rs = u_open(op->fname,oflags,op->operm) ;
	op->fd = rs ;

	if (isNotPresent(rs) && (op->oflags & O_CREAT)) {

	    f_created = true ;
	    oflags = op->oflags ;
	    rs = u_open(op->fname,oflags,op->operm) ;
	    op->fd = rs ;

	} /* end if (creating file) */

	if (rs >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    op->ti_open = dt ;
	    rs = uc_closeonexec(op->fd,true) ;
	} /* end if */

	if (rs >= 0)
	    rs = ts_filesetinfo(op,dt) ;

	if (rs >= 0)
	    rs = ts_ebufstart(op) ;

ret0:
	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (ts_fileopen) */


int ts_fileclose(op)
TS		*op ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (op->f.ebuf) {
	    rs1 = ts_ebuffinish(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (op->fd >= 0) {
	    op->f.lockedread = false ;
	    op->f.lockedwrite = false ;
	    rs1 = u_close(op->fd) ;
	    op->fd = -1 ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (ts_fileclose) */


static int ts_filesetinfo(op,dt)
TS		*op ;
time_t		dt ;
{
	USTAT	sb ;

	int	rs ;
	int	amode ;


	amode = (op->oflags & O_ACCMODE) ;
	op->f.writable = ((amode == O_WRONLY) || (amode == O_RDWR)) ;

	rs = u_fstat(op->fd,&sb) ;
	if (rs < 0) goto ret0 ;

	op->ti_mod = sb.st_mtime ;
	op->filesize = sb.st_size ;

/* is the file on a local or remote filesystem? */

	{
	    char	fstype[USERNAMELEN + 1] ;
	    int		fslen = USERNAMELEN ;
	    if ((rs = getfstype(fstype,fslen,op->fd)) >= 0) {
	        cint	fsl = rs ;
		cbool	f = (matlocalfs(fstype,fsl) >= 0) ;
	        op->f.remote = (! f) ; /* remote if not local! */
	    }

	} /* end block */

ret0:
	return rs ;
}
/* end subroutine (ts_filesetinfo) */


static int ts_ebufstart(op)
TS		*op ;
{
	uint	soff ;

	int	rs = SR_OK ;
	int	esize, nways, n ;


	if (! op->f.ebuf) {
	    if (op->fd >= 0) {

		soff = TS_TABOFF ;
		esize = TS_ENTSIZE ;
		nways = TS_NWAYS ;
		n = TS_NEPW ;
		rs = ebuf_start(&op->ebm,op->fd,soff,esize,nways,n) ;
		op->f.ebuf = (rs >= 0) ;

	    } else
		rs = SR_NOANODE ;
	}

	return rs ;
}
/* end subroutine (ts_ebufstart) */


static int ts_ebuffinish(op)
TS		*op ;
{
	int		rs = SR_OK ;
	in		rs1 ;

	if (op->f.ebuf) {
	    op->f.ebuf = false ;
	    rs1 = ebuf_finish(&op->ebm) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (ts_ebuffinish) */


static int ts_readentry(op,ei,rpp)
TS		*op ;
int		ei ;
char		**rpp ;
{
	int	rs ;

	char	*bp ;


	rs = ebuf_read(&op->ebm,ei,&bp) ;

	if (rs < 0)
	    goto ret0 ;

	if (rs == 0) rs = SR_NOTFOUND ;

	if (rpp != nullptr)
	    *rpp = (rs >= 0) ? bp : nullptr ;

/* enter into the index */

#if	CF_NIENUM
	if (rs >= 0) {
	    int		nl ;
	    char	*np ;

	    np = bp + TSE_OKEYNAME ;
	    nl = strnlen(np,TSE_LKEYNAME) ;

	    rs = ts_index(op,np,nl,ei) ;

	} /* end if (got an entry) */
#endif /* CF_NIENUM */

ret0:
	return rs ;
}
/* end subroutine (ts_readentry) */


#if	CF_NISEARCH

/* add to the name-index if necessary */
static int ts_index(op,np,nl,ei)
TS		*op ;
const char	nodename[] ;
int		nl ;
int		ei ;
{
	int	rs = SR_OK ;
	int	rs1 ;
	int	ei2 ;


	if (nl < 0)
	    nl = strlen(np) ;

	        rs1 = mapstrint_fetch(&op->ni,np,nl,nullptr,&ei2) ;

	        if ((rs1 >= 0) && (ei != ei2)) {

	            rs1 = SR_NOTFOUND ;
	            mapstrint_delkey(&op->ni,np,nl) ;

	        }

	        if (rs1 == SR_NOTFOUND) {

	            int	nl2 ;


	            nl2 = strnlen(np,TSE_LKEYNAME) ;

	            mapstrint_add(&op->ni,np,nl2,ei) ;

	        } /* end if (not found) */

	return rs ;
}
/* end subroutine (ts_index) */

#endif /* CF_NISEARCH */


static int ts_headwrite(op)
TS		*op ;
{
	off_t	poff ;

	int	rs ;
	int	toff = TS_TABOFF ;
	int	htoff = TS_HEADTABOFF ;
	int	bl ;

	const char	*bp ;


	rs = ts_headtab(op,0) ;	/* format into buffer */

	if (rs >= 0) {
	    bp = (op->topbuf + htoff) ;
	    bl = (toff - htoff) ;
	    poff = htoff ;
	    rs = u_pwrite(op->fd,bp,bl,poff) ;
	}

	return rs ;
}
/* end subroutine (ts_headwrite) */


static int namematch(np,nnp,nnl)
const char	np[] ;
const char	nnp[] ;
int		nnl ;
{
	int	f = false ;

	if (nnl > TSE_LKEYNAME)
	    goto ret0 ;

	f = (strncmp(np,nnp,nnl) == 0) ;
	f = f && (np[nnl] == '\0') ;

ret0:
	return f ;
}
/* end subroutine (namematch) */


