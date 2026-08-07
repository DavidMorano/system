/* bibdb_readent SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Bibliographical DataBase */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

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
#include	<ucmem.h>		/* LIBUC */
#include	<hdb.h>			/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */
#include	<bfile.h>		/* LIBB */

#include	"bibdb_ent.hh"
#include	"bibdb_key.hh"
#include	"bibdb_entfile.hh"
#include	"bibdb_bibentry.hh"
#include	"bibdb_readent.hh"
#include	"bibdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	BDB
#define	BDB		bibdb
#endif
#ifndef	BDB_ENT
#define	BDB_ENT		bibdb_ent
#endif
#ifndef	BDB_KEY
#define	BDB_KEY		bibdb_key
#endif


/* imported namespaces */

using namespace		bibdbx ;

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct procflags {
	uint		fbol:1 ;
	uint		feol:1 ;
	uint		finkey:1 ;
	uint		fdone:1 ;
    } ; /* end struct (procflags) */
    struct readenter {
	BDB		*op ;
	BDB_ENT		*bep ;
	BDB_KEY		*bkp ;
	vecobj		*flp ;
	charp		bebuf ;
	int		belen ;
	int		fi ;
	procflags	pf ;
	readenter(BDB *o,BDB_ENT *ep,char *b,int l,BDB_KEY *kp) noex {
	    op		= o ;
	    bep		= ep ;
	    bebuf	= b ;
	    belen	= l ;
	    bkp		= kp ;
	    pf = {} ;
	    {
	        flp = op->flp ;
	        fi = bkp->fi ; 
		memclear(bep) ;
		bebuf[0] = '\0' ;
	    } /* end block */
	} ; /* end ctor */
	operator int () noex ;
	int procfile	(cchar *) noex ;
	int procline	(bibentry *,charp,int) noex ;
	int reader	(bibentry *,charp,int,bfile *) noex ;
    } ; /* end struct (readenter) */
} /* end namespace */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

namespace bibdbx {
    int bibdb_readent(BDB *op,BDB_ENT *bep,char *bebuf,int belen,
		BDB_KEY *bkp) noex {
	int		rs = SR_BUGCHECK ;
	DEBUGPRINTF("ent\n") ;
	if (op && bkp && bep && bebuf) ylikely {
	    readenter ro(op,bep,bebuf,belen,bkp) ;
	    rs = ro ;
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
    } /* end subroutine (bibdb_readent) */
} /* end namespace (bibdbx) */


/* local subroutines */

readenter::operator int () noex {
    	int		rs ;
	DEBUGPRINTF("bk fi=%u\n",bkp->fi) ;
	DEBUGPRINTF("bk ckey=%s\n",bkp->ckey) ;
	DEBUGPRINTF("bk foff=%u\n",bkp->foff) ;
	DEBUGPRINTF("bk clen=%d\n",bkp->clen) ;
	if (void *vp ; (rs = flp->get(fi,&vp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (bibdb_entfile *bfep = resumelife<bibdb_entfile>(vp) ; bfep) {
	        rs = procfile(bfep->fname) ;
	    }
	} /* end if (vecobj_get) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (readenter::operator) */

int readenter::procfile(cchar *fname) noex {
    	cnothrow	nt{} ;
    	int		rs = SR_NOMEM ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (bibentry *iep = new(nt) bibentry ; iep) ylikely {
	    if ((rs = bibentry_start(iep,fi)) >= 0) ylikely {
	        if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) ylikely {
		    cint llen = rs ;
	            if (bfile bf ; (rs = bf.open(fname,"r")) >= 0) ylikely {
		        coff	boff = off_t(bkp->foff) ;
	                if ((rs = bf.seek(boff,SEEK_SET)) >= 0) {
			    {
		               rs = reader(iep,lbuf,llen,&bf) ;
			       rv = rs ;
			    }
		        } /* end if (bseek) */
	                rs1 = bf.close ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (file-open) */
	            rs1 = mem.free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	        rs1 = bibentry_finish(iep) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (bib-entry) */
	    delete iep ;
	} /* end if (new-bibentry) */
	return (rs >= 0) ? rv : rs ;
} /* end method (readenter::procfile) */

int readenter::reader(bibentry *iep,charp lbuf,int llen,bfile *bfp) noex {
    	int		rs = SR_OK ;
	int		ne = 0 ; /* return-value */
	int		rlen = 0 ;
	pf.fbol = true ;
	while ((rlen < bkp->clen) && ((rs = bfp->readln(lbuf,llen)) > 0)) {
	    cint len = rs ;
	    {
        	pf.feol = (lbuf[len - 1] == '\n') ;
		rs = procline(iep,lbuf,len) ;
	    }
	    rlen += len ;
	    pf.fbol = pf.feol ;
	    if (pf.fdone || (rs < 0)) break ;
	} /* end while */
	if ((rs >= 0) && pf.finkey) {
	    rs = bibentry_end(iep) ;
	    pf.finkey = false ;
	}
	/* OK, load this internal entry */
	if (rs >= 0) {
	    rs = entry_load(bep,bebuf,belen,iep) ;
	    ne += 1 ;
	} /* end if (ok) */
	return (rs >= 0) ? ne : rs ;
} /* end method (readenter::reader) */

int readenter::procline(bibentry *iep,charp lbuf,int len) noex {
    	int		rs = SR_OK ;
	int		nk = 0 ; /* return-value */
	{
            ccharp	lp = lbuf ;
            int		ll = (pf.feol) ? (len - 1) : len ;
            if (pf.fbol && (lbuf[0] == '%')) {
                if (pf.finkey) {
                    rs = bibentry_end(iep) ;
		    pf.finkey = false ;
                }
		if ((rs >= 0) && (ll > 1)) {
		    cchar *kp ;
		    if (cint kl = sfnext((lp + 1),(ll - 1),&kp) ; kl > 0) {
                        pf.finkey = true ;
                        ll -= intconv((kp + kl) - lp) ;
                        lp = (kp + kl) ;
                        rs = bibentry_begin(iep,kp,kl,lp,ll) ;
			nk += 1 ;
                    } /* end if (got key) */
		} /* end if (non-zero positive) */
            } else if (pf.finkey) {
	        if (ll > 0) {
                    rs = bibentry_cont(iep,lp,ll) ;
	        } else {
	            rs = bibentry_end(iep) ;
	            pf.finkey = false ;
		    pf.fdone = true ;
	        }
	    } else if (ll == 0) {
		pf.fdone = true ;
	    } /* end if */
	} /* end block */
	return (rs >= 0) ? nk : rs ;
} /* end method (readenter::procline) */


