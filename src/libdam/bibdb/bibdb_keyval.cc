/* bibdb_keyval SUPPORT */
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
#include	"bibdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using std::destroy_at ;                 /* subroutime */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

namespace bibdbx {
    int keyval_start(keyval *kvp) noex {
    	int		rs = SR_BUGCHECK ;
	if (kvp) ylikely {
	    rs = memclear(kvp) ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (keyval_start) */
    int keyval_size(keyval *kvp) noex {
	int		rs = SR_OK ;
	int		sz = 2 ;
	if (kvp->kp) {
	    sz += kvp->kl ;
	}
	if (kvp->vp) {
	    sz += kvp->vl ;
	}
	return (rs >= 0) ? sz : rs ;
    } /* end subroutine (keyval_size) */
    int keyval_finish(keyval *kvp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (kvp->kp) {
	    voidp vp = voidp(kvp->kp) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    kvp->kp = nullptr ;
	} /* end if (memory-release) */
	if (kvp->vp) {
	    voidp vp = voidp(kvp->vp) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    kvp->vp = nullptr ;
	} /* end if (memory-release) */
	return rs ;
    } /* end subroutine (keyval_finish) */
} /* end namespace (bibdbx) */


