/* bibdb_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Bibliographical DataBase */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */
#define	CF_QUERY	1		/* bibdb_query */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

	= 2005-10-01, David A­D­ Morano
	This was changed to work in the MMCITE program.  The old
	REFERM program is really obsolete.  It used a database lookup
	strategy to remote databases.  The high-level problem is:
	what to do if the cited BIB entry is not found?  How does a
	maintainer of the present (local) document know what that
	BIB entry was?  The new strategy (implemented by the MMCITE
	program) is more like what is done with BibTeX in the TeX
	(or LaTeX) world.  All BIB databases are really expected to
	be maintained by the document creator -- not some centralized
	entity.  The older centralized model reflected more the use
	in the corporate world (where different people create BIB
	entries) than in the more "modern" personal-responsibility
	type of world! :-)  Anyway, this is the way the gods seem
	to now want to do things.  Deal with it!

*/

/* Copyright © 1998,2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	bibdb

	Description:
	This code object module implements a little DB query facility.
	The database is a set of files that contain bibliographical
	entries in the "REFER" format.  Queries to the database
	that succeed return a structure with the elements of the
	bibliographic entry.  All queries are to database files
	that were referenced last (most previously) in the processing
	of the document.  The idea is that a more recent reference
	to a database file means that it is the preferred one to
	use since it is closest to the user (or the most preferred
	by the user).  A small nice thing about this database is
	that database files (files containing "REFER"-formatted
	entries) are not indexed unless they are needed (due to a
	failure to find the query in existing indexed files).  Only
	after all database files have been indexed and scanned to
	try to satisfy the query (and the request os not found),
	does the query fail.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<memory>		/* C++STD |destroy_at(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<hdb.h>			/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */
#include	<bfile.h>		/* LIBB */

#include	"bibdb_bibentry.hh"
#include	"bibdb_entfile.hh"
#include	"bibdb_key.hh"
#include	"bibdb_keyval.hh"
#include	"bibdb_ent.hh"
#include	"bibdb_readent.hh"
#include	"bibdb_entfileproc.hh"
#include	"bibdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */
import bibdb_mag ;

/* local defines */

#define	BDB		bibdb		/* type */
#define	BDB_CUR		bibdb_cur	/* type */
#define	BDB_ENT		bibdb_ent	/* type */
#define	BDB_KEY		bibdb_key	/* type */
#define	BDB_FI		bibdb_entfile	/* type */
#define	BDB_MAGIC	BIBDB_MAGIC
#define	BDB_DEFFILES	10		/* default files */
#define	BDB_DEFENTRIES	40		/* default entries */
#define	BDB_QUERYKEY	BIBDB_QUERYKEY

#define	BDB_KA		szof(char *(*)[2])
#define	BDB_BO(v)		\
	((BDB_KA - ((v) % BDB_KA)) % BDB_KA)

#define	KEYVAL		keyval
#define	NKVE		2		/* number of K-V array entries */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debug print-outs */
#endif
#ifndef	CF_QUERY
#define	CF_QUERY	1		/* bibdb_query */
#endif


/* imported namespaces */

using namespace bibdbx ;
using std::destroy_at ;                 /* subroutime */
using bibdbx::bibdb_readent ;		/* subroutine */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int bibdb_ctor(bibdb *op,Args ... args) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->qkp	= np ;
	    op->flp	= np ;		/* pointer file-list */
	    op->klp	= np ;		/* pointer key-list */
	    if (vecobj *flp = new(nt) vecobj ; flp) ylikely {
		op->flp = flp ;
	        if (hdb *klp = new(nt) hdb ; klp) ylikely {
		    op->klp = klp ;
		    rs = SR_OK ;
	        } /* end if (new-hdb) */
		if (rs < 0) {
		    delete flp ;
		    op->flp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibdb_ctor) */

local int bibdb_dtor(bibdb *op) noex {
	int		rs = SR_OK ;
	if (op->klp) ylikely {
	    delete op->klp ;
	    op->klp = nullptr ;
	}
	if (op->flp) ylikely {
	    delete op->flp ;
	    op->flp = nullptr ;
	}
	return rs ;
} /* end subroutine (bibdb_dtor) */

[[maybe_unused]]
local int bibdb_scan(bibdb *,hdb_dat,BDB_KEY *) noex ;
[[maybe_unused]]
local int bibdb_entfileidx(bibdb *,int) noex ;


/* local variables */

enum states {
	state_search,
	state_have,
	state_overlast
} ; /* end enum (stats) */

#if	CF_DEBUG
constexpr cpcchar	states[] = {
	"search",
	"have",
	nullptr
} ; /* end array */
#endif /* CF_DEBUG */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */

const bibdboms		bibdbom ;


/* exported subroutines */

int bibdb_start(bibdb *op,cchar *qkey,int opts) noex {
    	cnullptr	np{} ;
	int		rs ;
	DEBUGPRINTF("ent qkey=%s\n",qkey) ;
	if ((qkey == nullptr) || (qkey[0] == '\0')) {
	    qkey = BDB_QUERYKEY ;
	}
	if ((rs = bibdb_ctor(op)) >= 0) ylikely {
	    vecobj	*flp = op->flp ;
	    hdb		*klp = op->klp ;
	    cint	cl = lenstr(qkey) ;
	    /* store away stuff */
	    op->opts = opts ;
	    if (cc *cp ; (rs = mem.strw(qkey,cl,&cp)) >= 0) ylikely {
	        cint	esz = szof(bibdb_entfile) ;
	        cint	nf = BDB_DEFFILES ;
	        cint	vo = (vecobjm.ordered | vecobjm.stationary) ;
	        op->qkp = cp ;
	        op->qkl = cl ;
	        if ((rs = flp->start(esz,nf,vo)) >= 0) ylikely {
		    cint	ne = BDB_DEFENTRIES ;
		    if ((rs = klp->start(ne,1,np,np)) >= 0) {
		        op->magval = BDB_MAGIC ;
		    } /* end if (vecobj_start) */
		    if (rs < 0) {
		        flp->finish() ;
		    } /* end if (error) */
	        } /* end if (vecobj_start) */
	        if (rs < 0) {
		    voidp vp = voidp(op->qkp) ;
	            mem.free(vp) ;
	            op->qkp = nullptr ;
		    op->qkl = 0 ;
	        } /* end if (error) */
	    } /* end if (memory-acquire) */
	    if (rs < 0) {
		bibdb_dtor(op) ;
	    } /* end if (error) */
	} /* end if (bibdb_ctor) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bibdb_start) */

int bibdb_finish(bibdb *op) noex {
    	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	if ((rs = bibdb_magic(op)) >= 0) ylikely {
	    vecobj	*flp = op->flp ;
	    if (op->qkp) {
	        voidp vp = voidp(op->qkp) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->qkp = nullptr ;
	    } /* end if (memory-release) */
	    /* free up all key entries */
	    if (hdb_cur cur ; (rs1 = hdb_curbegin(op->klp,&cur)) >= 0) {
	        hdb_dat	key{} ;
	        hdb_dat	val{} ;
	        while ((rs1 = hdb_curenum(op->klp,&cur,&key,&val)) >= 0) {
		    if (BDB_KEY *bkp = resumelife<bibdb_key>(val.buf) ; bkp) {
		        {
	                    rs1 = bibdbkey_finish(bkp) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = mem.free(bkp) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
		    } /* end if (non-null) */
	        } /* end while */
		if ((rs >= 0) && (rs1 = rsn)) rs = rs1 ;
	        rs1 = hdb_curend(op->klp,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    /* free up the key container */
	    if (op->klp) {
	        rs1 = hdb_finish(op->klp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    /* free up all file entries */
	    void *vp ;
	    for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
	        if (BDB_FI *bfep = resumelife<BDB_FI>(vp) ; bfep) {
	            rs1 = bibdbentfile_finish(bfep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    /* free up the file entry container */
	    {
	        rs1 = flp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = bibdb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
        } /* end if (bibdb_magic) */
	return rs ;
} /* end subroutine (bibdb_finish) */

int bibdb_add(bibdb *op,cchar *fname) noex {
	int		rs ;
	if ((rs = bibdb_magic(op,fname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	        /* check if the file actually exists */
	        if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) ylikely {
	            if (! S_ISDIR(sb.st_mode)) {
	                bibdb_entfile	bfe ;
	                if ((rs = bibdbentfile_start(&bfe,fname)) >= 0) {
	                    if ((rs = vecobj_add(op->flp,&bfe)) >= 0) {
	                        op->unindexed += 1 ;
	                    }
	                    if (rs < 0) {
	                        bibdbentfile_finish(&bfe) ;
		            } /* end if (error) */
	                } /* end if (bibdbfile) */
	            } else {
	                rs = SR_ISDIR ;
	            }
	        } /* end if (stat) */
	    } /* end if (valid) */
        } /* end if (bibdb_magic) */
	return rs ;
} /* end subroutine (bibdb_add) */

int bibdb_count(bibdb *op) noex {
	int		rs ;
	if ((rs = bibdb_magic(op)) >= 0) ylikely {
	   rs = hdb_count(op->klp) ;
        } /* end if (bibdb_magic) */
	return rs ;
} /* end subroutine (bibdb_count) */

#if	CF_DEBUG && 0
	local void dumpkeylist(hdb *) noex ;
#endif /* CF_DEBUG */

#if	CF_QUERY /* bibdb_query */
int bibdb_query(bibdb *op,cc *quep,BDB_ENT *bep,char *bebuf,int belen) noex {
    	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	DEBUGPRINTF("ent quep=%s\n",quep) ;
	if ((rs = bibdb_magic(op,quep,bep,bebuf)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (quep[0] && (belen > 0)) ylikely {
		if (vecobj *flp = resumelife<vecobj>(op->flp) ; flp) ylikely {
		    if ((rs = flp->count) > 0) {
			BDB_KEY bke{} ;
	    		n = rs ;
			/* search the keys and find the highest file-index */
			hdb_dat key{} ;
			key.buf = quep ;
			key.len = lenstr(quep) ;
			/* perform the search */
			if (op->opts & bibdbom.uniq) {
	    		    while (op->unindexed > 0) {
	        		rs1 = bibdb_entfileidx(op,n) ;
	        		if (rs1 < 0) break ;
	    		    } /* end while */
			} /* end if (uniqueness of query required) */
			rs = bibdb_scan(op,key,&bke) ;
			if ((rs >= 0) && (op->opts & bibdbom.uniq)) {
	    		    if (rs > 1) rs = SR_NOTUNIQ ;
			} /* end if (uniqueness) */
			while ((rs == rsn) && (op->unindexed > 0)) {
	    		    {
	    		        rs1 = bibdb_entfileidx(op,n) ;
	    		        if (rs1 < 0) break ;
	    		    }
	    		    {
	    		        rs = bibdb_scan(op,key,&bke) ;
	    		        if (rs >= 0) break ;
	    		    }
			} /* end while */
#if	CF_DEBUG && 0
			dumpkeylist(op->klp) ;
#endif /* CF_DEBUG */
			if (rs >= 0) {
	    		    rs = bibdb_readent(op,bep,bebuf,belen,&bke) ;
	    		    n = rs ;
			} /* end if */
		    } else if (rs == 0) {
	    	        rs = SR_NOTFOUND ;
		    }
		} /* end if (non-null) */
	    } /* end if (valid) */
        } /* end if (bdb_magic) */
	DEBUGPRINTF("ret rs=%d n=%u\n",rs,n) ;
	return (rs >= 0) ? n : rs ;
} /* end subroutine (bibdb_query) */
#endif /* CF_QUERY */


/* private subroutines */

[[maybe_unused]]
local int bibdb_scan(bibdb *op,hdb_dat key,BDB_KEY *ubkp) noex {
    	hdb		*klp = resumelife<hdb>(op->klp) ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if (hdb_cur cur ; (rs = klp->curbegin(&cur)) >= 0) ylikely {
	    hdb_dat	val ;
	    int		fi = -1 ;
	    while ((rs1 = klp->fetch(key,&cur,&val)) >= 0) {
		if (BDB_KEY *bkp = resumelife<BDB_KEY>(val.buf) ; bkp) {
	            c += 1 ;
	            if (bkp->fi > fi) {
	                *ubkp = *bkp ; /* copy */
	                fi = bkp->fi ;
	            }
		}
	    } /* end while (looping through keys) */
	    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	    rs1 = klp->curend(&cur) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs >= 0) && (fi < 0)) {
		rs = SR_NOTFOUND ;
	    }
	} /* end if */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibdb_scan) */

/* index one bibdb database file (the most recent unindexed file) */
[[maybe_unused]]
local int bibdb_entfileidx(bibdb *op,int n) noex {
	BDB_FI		*bfep = nullptr ;
	vecobj		*flp = op->flp ;
	int		rs = SR_NOTFOUND ;
	int		fi = 0 ; /* return-value */
	DEBUGPRINTF("ent n=%u\n",n) ;
	if (n > 0) {
	    /* find the youngest file that has not been indexed */
	    void *vp ;
	    cauto fget = [flp] (int fidx,void **vpp) -> int {
		int rsl = SR_OK ;
		if (fidx >= 0) {
	            if ((rsl = flp->get(fidx,vpp)) >= 0) {
			rsl = 1 ;
		    }
		}
		return rsl ;
	    } ; /* end lambda (fget) */
	    for (fi = (n - 1) ; (rs = fget(fi,&vp)) > 0 ; fi -= 1) {
		if (bfep = resumelife<BDB_FI>(vp) ; bfep) {
	            if (! bfep->f_indexed) break ;
	        }
	    } /* end for */
	    DEBUGPRINTF("file to index fi=%u\n",fi) ;
	    /* something to index? */
	    if ((rs >= 0) && (fi >= 0) && bfep) {
	        rs = bibdb_entfileproc(op,fi,bfep) ;
	    } /* end if */
	} /* end if (non-zero positive) */
	DEBUGPRINTF("ret rs=%d fi=%u\n",rs,fi) ;
	return (rs >= 0) ? fi : rs ;
} /* end subroutine (bibdb_entfileidx) */

#if	CF_DEBUG && 0
[[maybe_unused]]
local void dumpkeylist(hdb *klp) noex {
    hdb_cur     cur ;
    hdb_dat     val ;
    DEBUGPRINTF("search loop rs=%d\n",rs) ;
    hdb_curbegin(klp,&cur) ;
    while (hdb_curenum(klp,&cur,&key,&val) >= 0) {
        BDB_KEY *bkp = resumelife<BDB_KEY>(val.buf) ;
        DEBUGPRINTF("qbuf=%r fi=%u\n",key.buf,key.len,bkp->fi) ;
    } /* end while */
    hdb_curend(op->klp,&cur) ;
} /* end subroutine (dumpkeylist) */
#endif /* CF_DEBUG */


