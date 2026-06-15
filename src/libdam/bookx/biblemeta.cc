/* biblemeta SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vector string operations */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written for some previous
	purpose and was adapted for the present use.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	biblemeta

	Description:
	This module implements an interface (a trivial one) that
	allows access to the BIBLEMETA datbase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD*/
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"biblemeta.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BM			biblemeta

#define	BIBLEMETA_DBNAME	"english"
#define	BIBLEMETA_DBDNAME	"lib/bibleset/metawords"
#define	BIBLEMETA_DBTITLE	"Bible"
#define	BIBLEMETA_DEFENTS	67


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int biblemeta_ctor(biblemeta *op,Args ... args) noex {
	cnullptr	np{} ;
	cnothrow	nt{} ;
    	BIBLEMETA	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->dbp = new(nt) vecstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (biblemeta_ctor) */

local int biblemeta_dtor(biblemeta *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->dbp) ylikely {
		delete op->dbp ;
		op->dbp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (biblemeta_dtor) */

template<typename ... Args>
local inline int biblemeta_magic(biblemeta *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BIBLEMETA_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (biblemeta_magic) */

local int	biblemeta_opener(BM *,cc *,cc *) noex ;


/* local variables */

constexpr char		dbdname[]	= BIBLEMETA_DBDNAME ;


/* exported variables */

biblemeta_obj	biblemeta_modinfo = {
	"biblemeta",
	szof(biblemeta),
	0
} ; /* end initialization */


/* exported subroutines */

int biblemeta_open(BM *op,cchar *pr,cchar *dbn) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rc = 0 ;
	if ((dbn == np) || (dbn[0] == '\0')) {
	    dbn = BIBLEMETA_DBNAME ;
	}
	if ((rs = biblemeta_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		rs = biblemeta_opener(op,pr,dbn) ;
		rc = rs ;
	    } /* end if (valid) */
	    if (rs < 0) {
		biblemeta_dtor(op) ;
	    } /* end if (error) */
	} /* end if (biblemeta_ctor) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (biblemeta_open) */

int biblemeta_close(BM *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = biblemeta_magic(op)) >= 0) {
	    if (vecstr *dbp = op->dbp ; dbp) {
	        rs1 = dbp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = biblemeta_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (biblemeta_close) */

int biblemeta_count(BM *op) noex {
	int		rs ;
	if ((rs = biblemeta_magic(op)) >= 0) {
	    vecstr *dbp = op->dbp ;
	    rs = dbp->count ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (biblemeta_count) */

int biblemeta_nummax(BM *op) noex {
	int		rs ;
	int		num = 0 ;
	if ((rs = biblemeta_magic(op)) >= 0) {
	    vecstr *dbp = op->dbp ;
	    if ((rs = dbp->count) > 0) {
	        num = (rs - 1) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? num : rs ;
} /* end subroutine (biblemeta_nummax) */

int biblemeta_audit(BM *op) noex {
	int		rs ;
	if ((rs = biblemeta_magic(op)) >= 0) {
	    vecstr *dbp = op->dbp ;
	    rs = dbp->audit ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (biblemeta_audit) */

int biblemeta_get(BM *op,int i,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = biblemeta_magic(op,rbuf)) >= 0) {
	    vecstr *dbp = op->dbp ;
	    if (cchar *cp{} ; (rs = dbp->get(i,&cp)) >= 0) {
	        rs = sncpy1(rbuf,rlen,cp) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (biblemeta_get) */


/* local subroutines */

local int biblemeta_opener(BM *op,cc *pr,cc *dbn) noex {
	vecstr		*dbp = op->dbp ;
	cint		vn = BIBLEMETA_DEFENTS ;
	cint		vo = 0 ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if ((rs = dbp->start(vn,vo)) >= 0) {
	    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	        if ((rs = mkpath3(tbuf,pr,dbdname,dbn)) >= 0) {
	            if ((rs = dbp->loadfile(false,tbuf)) >= 0) {
	                if ((rs = dbp->count) >= 0) {
			    c = rs ;
	                    op->magval = BIBLEMETA_MAGIC ;
	                }
	            } /* end if (vecstr_loadfile) */
	        } /* end if (mkpath) */
		rs1 = mem.free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    if (rs < 0) {
		dbp->finish() ;
	    } /* end if (error) */
	} /* end if (vecstr_start) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (biblemeta_opener) */


