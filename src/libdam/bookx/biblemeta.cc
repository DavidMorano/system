/* biblemeta SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"biblemeta.h"


/* local defines */

#define	BM			biblemeta

#define	BIBLEMETA_DBNAME	"english"
#define	BIBLEMETA_DBDNAME	"lib/bibleset/metawords"
#define	BIBLEMETA_DBTITLE	"Bible"
#define	BIBLEMETA_DEFENTS	67


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int biblemeta_ctor(biblemeta *op,Args ... args) noex {
    	BIBLEMETA	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->dbp = new(nothrow) vecstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (biblemeta_ctor) */

static int biblemeta_dtor(biblemeta *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbp) {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (biblemeta_dtor) */

template<typename ... Args>
static inline int biblemeta_magic(biblemeta *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == BIBLEMETA_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (biblemeta_magic) */

static int	biblemeta_opener(BM *,cc *,cc *) noex ;


/* local variables */


/* exported variables */

biblemeta_obj	biblemeta_modinfo = {
	"biblemeta",
	sizeof(biblemeta)
} ;


/* exported subroutines */

int biblemeta_open(BM *op,cchar *pr,cchar *dbn) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rc = 0 ;
	if ((dbn == np) || (dbn[0] == '\0')) dbn = BIBLEMETA_DBNAME ;
	if ((rs = biblemeta_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		rs = biblemeta_opener(op,pr,dbn) ;
		rc = rs ;
	    } /* end if (valid) */
	    if (rs < 0) {
		biblemeta_dtor(op) ;
	    }
	} /* end if (biblemeta_ctor) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (biblemeta_open) */

int biblemeta_close(BM *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = biblemeta_magic(op)) >= 0) {
	    {
	        rs1 = vecstr_finish(op->dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = biblemeta_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (biblemeta_close) */

int biblemeta_count(BM *op) noex {
	int		rs ;
	if ((rs = biblemeta_magic(op)) >= 0) {
	    rs = vecstr_count(op->dbp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (biblemeta_count) */

int biblemeta_nummax(BM *op) noex {
	int		rs ;
	if ((rs = biblemeta_magic(op)) >= 0) {
	    if ((rs = vecstr_count(op->dbp)) > 0) {
	        rs -= 1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (biblemeta_nummax) */

int biblemeta_audit(BM *op) noex {
	int		rs ;
	if ((rs = biblemeta_magic(op)) >= 0) {
	    rs = vecstr_audit(op->dbp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (biblemeta_audit) */

int biblemeta_get(BM *op,int i,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = biblemeta_magic(op,rbuf)) >= 0) {
	    if (cchar *cp{} ; (rs = vecstr_get(op->dbp,i,&cp)) >= 0) {
	        rs = sncpy1(rbuf,rlen,cp) ;
	}
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (biblemeta_get) */


/* local subroutines */

static int biblemeta_opener(BM *op,cc *pr,cc *dbn) noex {
	cint		vn = BIBLEMETA_DEFENTS ;
	cint		vo = 0 ;
	int		rs ;
	int		c = 0 ;
	if ((rs = vecstr_start(op->dbp,vn,vo)) >= 0) {
	    if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	        if ((rs = mkpath3(tbuf,pr,BIBLEMETA_DBDNAME,dbn)) >= 0) {
	            if ((rs = vecstr_loadfile(op->dbp,false,tbuf)) >= 0) {
	                if ((rs = vecstr_count(op->dbp)) >= 0) {
			    c = rs ;
	                    op->magic = BIBLEMETA_MAGIC ;
	                }
	            } /* end if (vecstr_loadfile) */
	        } /* end if (mkpath) */
		rs = rsfree(rs,tbuf) ;
	    } /* end if (m-a-f) */
	    if (rs < 0) {
		vecstr_finish(op->dbp) ;
	    }
	} /* end if (vecstr_start) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (biblemeta_opener) */


