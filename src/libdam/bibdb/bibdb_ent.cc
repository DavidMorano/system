/* bibdb_ent SUPPORT */
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
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"bibdb_keyval.hh"
#include	"bibdb_bibentry.hh"
#include	"bibdb_ent.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	NKVE		2		/* number of K-V array entries */

#ifndef	BDB_ENT
#define	BDB_ENT		bibdb_ent
#endif
#ifndef	BDB_KA
#define	BDB_KA		szof(char *(*)[NKVE])
#endif
#ifndef	BDB_BO
#define	BDB_BO(v)	((BDB_KA - ((v) % BDB_KA)) % BDB_KA)
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

typedef cchar *(*kvtabp)[NKVE] ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

namespace bibdbx {
    int entry_load(BDB_ENT *ep,char *ebuf,int elen,bibentry *iep) noex {
	int		rs ;
	int		nk = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if ((rs = iep->count) > 0) {
	    cint	tabsz = ((rs + 1) * NKVE * szof(cchar *)) ;
	    cint	bo = BDB_BO((ulong) ebuf) ;
	    nk = rs ;
	    if (cint sz = (tabsz + iep->sz) ; sz <= (elen - bo)) {
	        kvtabp	keyvals = kvtabp(ebuf + bo) ;
	        keyval	*kvp{} ;
	        int	j = 0 ;
	        charp	bp = charp(ebuf + bo + tabsz) ;
	        for (int i = 0 ; iep->getkv(i,&kvp) >= 0 ; i += 1) {
		    if (kvp) {
	                keyvals[j][0] = bp ;
	                bp = strwcpy(bp,kvp->kp,kvp->kl) + 1 ;
	                if (kvp->vp) {
	                    keyvals[j][1] = bp ;
	                    bp = strwcpy(bp,kvp->vp,kvp->vl) + 1 ;
	                } else {
	                    keyvals[j][1] = nullptr ;
		        } /* end if */
	                j += 1 ;
	            } /* end if (non-null) */
	        } /* end for */
	        keyvals[j][0] = nullptr ;
	        keyvals[j][1] = nullptr ;
	        ep->sz		= sz ;
	        ep->nkeys	= nk ;
	        ep->keyvals	= keyvals ;
	        ep->fi		= iep->fi ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (vecobj_count) */
	DEBUGPRINTF("ret rs=%d nk=%d\n",rs,nk) ;
	return (rs >= 0) ? nk : rs ;
    } /* end subroutine (entry_load) */
} /* end namespace (bibdbx) */


