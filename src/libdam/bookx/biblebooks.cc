/* biblebooks SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEBOOKS object implementation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	biblebooks

	Description:
	This module implements an interface (a trivial one) that
	allows access to the BIBLEBOOKS datbase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecpstr.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"biblebooks.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	BIBLEBOOKS_DBNAME	"english"
#define	BIBLEBOOKS_DBDNAME	"share/biblebooks"
#define	BIBLEBOOKS_DBTITLE	"Bible"
#define	BIBLEBOOKS_DEFENTS	67

#define	BBS		biblebooks
#define	BBS_MAG		BIBLEBOOKS_MAGIC

#ifndef	CF_DEBUG
#define	CF_DEBUG	0	/* debugging */
#endif


/* imported namespaces */

using libuc::mem ;		/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int biblebooks_ctor(BBS *op,Args ... args) noex {
    	BIBLEBOOKS	*hop = op ;
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->dbp = new(nt) vecpstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecpstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (biblebooks_ctor) */

local int biblebooks_dtor(BBS *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbp) {
		delete op->dbp ;
		op->dbp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (biblebooks_dtor) */

template<typename ... Args>
local inline int biblebooks_magic(BBS *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BBS_MAG) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (biblebooks_magic) */


/* local variables */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */

const biblebooks_obj	biblebooks_modinfo = {
	"biblebooks",
	szof(biblebooks),
	szof(int)
} ; /* end initialization */


/* exported variables */


/* exported subroutines */

int biblebooks_open(BBS *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if ((dbn == nullptr) || (dbn[0] == '\0')) {
	    dbn = BIBLEBOOKS_DBNAME ;
	}
	if ((rs = biblebooks_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		vecpstr	*dbp = op->dbp ;
	        cint	vn	= BIBLEBOOKS_DEFENTS ;
		cint	vsz	= 900 ;
		cint	vo	= 0 ;
	        if ((rs = dbp->start(vn,vsz,vo)) >= 0) {
	            cchar	*dname = BIBLEBOOKS_DBDNAME ;
		    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	                if ((rs = mkpath(tbuf,pr,dname,dbn)) >= 0) {
		            if ((rs = dbp->loadfile(false,tbuf)) >= 0) {
		                op->magval = BIBLEBOOKS_MAGIC ;
		                rs = vecpstr_count(op->dbp) ;
				c = rs ;
		            }
	                } /* end if (mkpath) */
			rs1 = mem.free(tbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	            if (rs < 0) {
	                dbp->finish() ;
		        op->magval = 0 ;
	            } /* end if (error) */
	        } /* end if (vecpstr_start) */
	    } /* end if (valid) */
	    if (rs < 0) {
		biblebooks_dtor(op) ;
	    }
        } /* end if (biblebooks_ctor) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (biblebooks_open) */

int biblebooks_close(BBS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = biblebooks_magic(op)) >= 0) {
	    if (op->dbp) {
		vecpstr *dbp = op->dbp ;
	        rs1 = dbp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = biblebooks_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (biblebooks_close) */

int biblebooks_count(BBS *op) noex {
	int		rs ;
        if ((rs = biblebooks_magic(op)) >= 0) ylikely {
	    vecpstr *dbp = op->dbp ;
	    rs = dbp->count ;
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (biblebooks_count) */

int biblebooks_max(BBS *op) noex {
	int		rs ;
        if ((rs = biblebooks_magic(op)) >= 0) ylikely {
	    vecpstr *dbp = op->dbp ;
	    if ((rs = dbp->count) > 0) {
	        rs -= 1 ;
	    }
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (biblebooks_max) */

int biblebooks_audit(BBS *op) noex {
	int		rs ;
        if ((rs = biblebooks_magic(op)) >= 0) ylikely {
	    vecpstr *dbp = op->dbp ;
	    rs = dbp->audit ;
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (biblebooks_audit) */

int biblebooks_look(BBS *op,char *rbuf,int rlen,int bi) noex {
	int		rs ;
        if ((rs = biblebooks_magic(op,rbuf)) >= 0) ylikely {
	    vecpstr *dbp = op->dbp ;
	    if (cchar *cp ; (rs = dbp->get(bi,&cp)) >= 0) {
	        rs = sncpy1(rbuf,rlen,cp) ;
	    }
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (biblebooks_look) */

int biblebooks_get(BBS *op,int bi,char *rbuf,int rlen) noex {
	return biblebooks_look(op,rbuf,rlen,bi) ;
} /* end subroutine (biblebooks_get) */

int biblebooks_size(BBS *op) noex {
    	int		rs ;
        if ((rs = biblebooks_magic(op)) >= 0) ylikely {
	    vecpstr *dbp = op->dbp ;
	    rs = dbp->strsize ;
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (biblebooks_size) */


