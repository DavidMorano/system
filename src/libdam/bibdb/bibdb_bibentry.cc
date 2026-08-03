/* bibdb_bibentry SUPPORT */
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
#include	<memory>                /* C++STD |destroy_at(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
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

#include	"bibdb_keyval.hh"
#include	"bibdb_bibentry.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* imported namespaces */

using namespace		bibdbx ;
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int bibentry_ctor(bibentry *op,Args ... args) noex {
    	cnullptr	np{} ;
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->kvlp	= np ;
	    op->vbp	= np ;
	    op->kp	= np ;
	    if (vecobj *kvlp = new(nt) vecobj ; kvlp) ylikely {
		op->kvlp = kvlp ;
	        if (buffer *vbp = new(nt) buffer ; vbp) ylikely {
		    op->vbp = vbp ;
		    rs = SR_OK ;
		} /* end if (new-buffer) */
		if (rs < 0) {
		    delete kvlp ;
		    op->kvlp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibentry_ctor) */

local int bibentry_dtor(bibentry *op) noex {
	int		rs = SR_OK ;
	if (op->vbp) ylikely {
	    delete op->vbp ;
	    op->vbp = nullptr ;
	}
	if (op->kvlp) ylikely {
	    delete op->kvlp ;
	    op->kvlp = nullptr ;
	}
	return rs ;
} /* end subroutine (bibentry_dtor) */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace bibdbx {
    int bibentry_start(bibentry *iep,int fi) noex {
	int		rs ;
	if ((rs = bibentry_ctor(iep)) >= 0) ylikely {
	    vecobj	*kvlp = iep->kvlp ;
	    cint	esz = szof(keyval) ;
	    cint	vn = 10 ;
	    cint	vo = 0 ;
	    if ((rs = kvlp->start(esz,vn,vo)) >= 0) ylikely {
	        iep->sz = 0 ;
	        iep->fi = fi ;
	    } /* end if (vecobj_start) */
	    if (rs < 0) {
		bibentry_dtor(iep) ;
	    } /* end if (erro) */
        } /* end if (bibentry_ctor) */
	return rs ;
    } /* end subroutine (bibentry_start) */
    int bibentry_begin(bibentry *iep,cc *kp,int kl,cc *vap,int val) noex {
	buffer		*bfp = iep->vbp ;
	int		rs ;
	if (kl < 0) kl = lenstr(kp) ;
	/* key */
	iep->kp = nullptr ;
	if (cchar *cp ; (rs = mem.strw(kp,kl,&cp)) >= 0) ylikely {
	    iep->kp = cp ;
	    iep->kl = kl ;
	    if ((rs = bfp->start(80)) >= 0) ylikely {
	        if (val < 0) val = lenstr(vap) ;
	        while (val && ISWHT(*vap)) {
	            vap += 1 ;
	            val -= 1 ;
	        } /* end while */
	        rs = bfp->strw(vap,val) ;
	        if (rs < 0) {
	            bfp->finish() ;
		} /* end if (error) */
	    } /* end if (buffer_start) */
	    if (rs < 0) {
	        voidp vp = voidp(iep->kp) ;
	        mem.free(vp) ;
	        iep->kp = nullptr ;
	    } /* end if (error) */
	} /* end if (m-a) */
	return rs ;
    } /* end subroutine (bibentry_begin) */
    int bibentry_cont(bibentry *iep,cchar *vap,int val) noex {
	buffer		*bfp = iep->vbp ;
	int		rs ;
	if (val < 0) val = lenstr(vap) ;
	while (val && ISWHT(*vap)) {
	    vap += 1 ;
	    val -= 1 ;
	} /* end while */
	if (cchar *bp ; (rs = bfp->get(&bp)) >= 0) ylikely {
	    cint bl = rs ;
	    if (! ISWHT(bp[bl - 1])) {
	        rs = bfp->chr(' ') ;
	    }
	    if (rs >= 0) {
	        rs = bfp->strw(vap,val) ;
	    } /* end if (ok) */
	} /* end if */
	return rs ;
    } /* end subroutine (bibentry_cont) */
    int bibentry_count(bibentry *iep) noex {
	int		rs = SR_FAULT ;
	if (iep) {
	    rs = SR_BUGCHECK ;
	    if (vecobj *kvlp = iep->kvlp ; kvlp) {
	        rs = kvlp->count ;
	    } /* end if (bug-check) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (bibentry_count) */
    int bibentry_end(bibentry *iep) noex {
	buffer		*bfp = iep->vbp ;
	int		rs ;
	int		rs1 ;
	if (keyval kv ; (rs = keyval_start(&kv)) >= 0) ylikely {
	    kv.kl = iep->kl ;
	    kv.kp = iep->kp ;
	    iep->kp = nullptr ;		/* good!  zapped! */
	    if (cchar *vap ; (rs = bfp->get(&vap)) >= 0) ylikely {
	        cint val = rs ;
	        if (cchar *cp ; (rs = mem.strw(vap,val,&cp)) >= 0) ylikely {
	            kv.vp = cp ;
	            kv.vl = val ;
	        } /* end if (memory-acquire) */
	    } /* end if */
	    if (rs >= 0) ylikely {
	        if ((rs = vecobj_add(iep->kvlp,&kv)) >= 0) ylikely {
	            cint sz = keyval_size(&kv) ;
	            iep->sz += sz ;
	        }
	        if (rs < 0) {
	            keyval_finish(&kv) ;
	        } /* end if (error) */
	    } /* end if (inserting into DB) */
	    {
	        rs1 = bfp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (keyval_start) */
	return rs ;
    } /* end subroutine (bibentry_end) */
    int bibentry_finish(bibentry *iep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (iep->kp) {
	    voidp vp = voidp(iep->kp) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    iep->kp = nullptr ;
	} /* end if (memory-release) */
    	if (vecobj *kvlp = iep->kvlp ; kvlp) ylikely {
	    void *vp ;
	    for (int i = 0 ; kvlp->get(i,&vp) >= 0 ; i += 1) {
	        keyval *kvp = resumelife<keyval>(vp) ;
	        if (kvp) {
	            rs1 = keyval_finish(kvp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    {
	        rs1 = kvlp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = bibentry_dtor(iep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (key-value list pointer) */
	return rs ;
   } /* end subroutine (bibentry_finish) */
} /* end namespace (bibdbx) */


/* local subroutines */

int bibentry::getkv(int i,keyval **rpp) noex {
    	int		rs = SR_BUGCHECK ;
	if (kvlp) ylikely {
	    if (void *vp ; (rs = kvlp->get(i,&vp)) >= 0) ylikely {
		rs = SR_BUGCHECK ;
	        if (keyval *kvp = resumelife<keyval>(vp) ; kvp) ylikely {
		    rs = SR_OK ;
		    if (rpp) *rpp = kvp ;
		}
	    } /* end if (vecobj_get) */
	} /* end if (non-null) */
	return rs ;
} /* end method (bibentry::getkv) */

void bibentry::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("bibentry",rs,"fini-finish") ;
	}
} /* end method (bibentry::dtor) */

bibentry_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case bibentrymem_count:
	        rs = bibentry_count(op) ;
	        break ;
	    case bibentrymem_end:
	        rs = bibentry_end(op) ;
	        break ;
	    case bibentrymem_finish:
	        rs = bibentry_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (bibentry_co::operator) */


