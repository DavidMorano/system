/* bibdb_cur SUPPORT */
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
#define	BDB_DEFFILES	10		/* default files */
#define	BDB_DEFENTRIES	40		/* default entries */


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


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

local int bdbdb_curibeg(BDB *op,hdb_cur *icurp) noex {
    	int		rs = SR_BUGCHECK ;
	if (hdb *klp = resumelife<hdb>(op->klp) ; klp) ylikely {
	    rs = klp->curbegin(icurp) ;
	}
	return rs ;
} /* end subroutine (bdbdb_curibeg) */

int bibdb_curbegin(bibdb *op,bibdb_cur *curp) noex {
    	cnothrow	nt{} ;
	int		rs ;
	if ((rs = bibdb_magic(op,curp)) >= 0) ylikely {
	    rs = SR_NOMEM ;
	    if (hdb_cur *icurp = new(nt) hdb_cur ; icurp) ylikely {
		curp->icurp = icurp ;
		rs = bdbdb_curibeg(op,icurp) ;
		if (rs < 0) {
		    delete icurp ;
		    curp->icurp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-hdb_cur) */
        } /* end if (bibdb_magic) */
	return rs ;
} /* end subroutine (bibdb_curbegin) */

local int bdbdb_curiend(BDB *op,hdb_cur *icurp) noex {
    	int		rs = SR_BUGCHECK ;
	if (hdb *klp = resumelife<hdb>(op->klp) ; klp) ylikely {
	    rs = klp->curend(icurp) ;
	}
	return rs ;
} /* end subroutine (bdbdb_curiend) */

int bibdb_curend(bibdb *op,bibdb_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibdb_magic(op,curp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if (hdb_cur *icurp = resumelife<hdb_cur>(curp->icurp) ; icurp) {
		rs = SR_OK ;
	        {
	            rs1 = bdbdb_curiend(op,icurp) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	        {
		    delete icurp ;
		    curp->icurp = nullptr ;
	        }
	    } /* end if (bug-check) */
        } /* end if (bibdb_magic) */
	return rs ;
} /* end subroutine (bibdb_curend) */

/* NOT-FINISHED - FIXME - put data into result buffer |rbuf| */
int bibdb_curenum(bibdb *op,BDB_CUR *curp,
		BDB_ENT *ep,char *rbuf,int rlen) noex {
    	int		rs ;
	int		rl = 0 ; /* return-value */
        if ((rs = bibdb_magic(op,curp,ep,rbuf)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    rbuf[0] = '\0' ;
	    (void) rlen ;
	    if (hdb_cur *icurp = resumelife<hdb_cur>(curp->icurp) ; icurp) {
        	if (hdb *klp = resumelife<hdb>(op->klp) ; klp) {
		    hdb_dat key{} ;
		    hdb_dat val{} ;
            	    rs = klp->curenum(icurp,&key,&val) ;
		    rl = rs ;
        	} /* end if (non-null) */
	    } /* end if (non-null) */
        } /* end if (bibdb_magic) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (bibdb_curenum) */

local int bibdb_icurdel(bibdb *op,hdb_cur *icurp,int f_adv) noex {
    	int		rs = SR_BUGCHECK ;
        if (hdb *klp = resumelife<hdb>(op->klp) ; klp) ylikely {
            rs = klp->curdel(icurp,f_adv) ;
        }
	return rs ;
} /* end subroutine (bibdb_icurdel) */

int bibdb_curdel(bibdb *op,BDB_CUR *curp,int f_adv) noex {
        int             rs ;
        if ((rs = bibdb_magic(op,curp)) >= 0) ylikely {
            rs = SR_BUGCHECK ;
	    if (hdb_cur *icurp = resumelife<hdb_cur>(curp->icurp) ; icurp) {
                rs = bibdb_icurdel(op,icurp,f_adv) ;
            }
        } /* end if (bibdb_magic) */
        return rs ;
} /* end subroutine (bibdb_curdel) */


/* private subroutines */


