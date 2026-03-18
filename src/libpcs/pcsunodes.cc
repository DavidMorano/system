/* pcsunodes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* PCS user-nodes */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */

/* revision history:

	= 2000-09-10, Dave Morano
	This program was originally written.

*/

/* Copyright © 2000 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pcsunodes

	Description:
	This object manages the list of user-nodes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<vecpstr.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<matstr.h>		/* |matcasestr(3uc)| */
#include	<localmisc.h>

#include	"pcsunodes.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	PN		pcsunodes
#define	PN_CUR		PCSUNODES_CUR
#define	PN_MAGIC	PCSUNODES_MAGIC
#define	PN_FNAME	"etc/usernodes"


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int pcsunodes_magic(pcsunodes *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PN_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (pcsunodes_magic) */

local int	pcsunodes_mktab(PN *,vecpstr *) noex ;

local int	vecpstr_loadnodes(vecpstr *,cchar **,char *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsunodes_start(PN *op,cchar *pr) noex {
    	PCSUNODES	*hop = op ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && pr) {
	    rs = SR_INVALID ;
	    memclear(hop) ;
	    if (pr[0]) {
	        cchar	*ufn = PN_FNAME ;
		if (char *ubuf ; (rs = lm_mp(&ubuf)) >= 0) {
	            if ((rs = mkpath(ubuf,pr,ufn)) >= 0) {
	                cint	vsz = 0 ;
	                cint	vn = 0 ;
	                cint	vo = 0 ;
	                cint	f = true ;
	                if (vecpstr un ; (rs = un.start(vsz,vn,vo)) >= 0) {
	                    if ((rs = un.loadfile(f,ubuf)) >= 0) {
		                if ((rs = pcsunodes_mktab(op,&un)) >= 0) {
			            op->magic = PN_MAGIC ;
		                }
		            } /* end if (vecpstr_loadfile) */
		            rs1 = un.finish ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (vecpstr) */
	                if (rs < 0) {
		            pcsunodes_finish(op) ;
	                }
	            } /* end if (mkpath) */
		    rs1 = lm_free(ubuf) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pcsunodes_start) */

int pcsunodes_finish(PN *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pcsunodes_magic(op)) >= 0) {
	    if (op->unodes != nullptr) {
	        rs1 = lm_free(op->unodes) ;
	        if (rs >= 0) rs = rs1 ;
	        op->unodes = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (pcsunodes_magic) */
	return rs ;
}
/* end subroutine (pcsunodes_finish) */

int pcsunodes_get(PN *op,int i,cchar **rpp) noex {
	int		rs ;
	if ((rs = pcsunodes_magic(op)) >= 0) {
	    if ((i >= 0) && (i < op->n)) {
	        if (rpp) {
		    *rpp = op->unodes[i] ;
		}
	        rs = lenstr(op->unodes[i]) ;
	    } else {
	        if (rpp != nullptr) *rpp = nullptr ;
	        rs = SR_INVALID ;
	    }
	} /* end if (pcsunodes_magic) */
	return rs ;
}
/* end subroutine (pcsunodes_get) */

int pcsunodes_mat(PN *op,cchar *mp,int ml) noex {
	int		rs ;
	if ((rs = pcsunodes_magic(op)) >= 0) {
	    if ((rs = matcasestr(op->unodes,mp,ml)) < 0) {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (pcsunodes_magic) */
	return rs ;
}
/* end subroutine (pcsunodes_mat) */

int pcsunodes_curbegin(PN *op,PN_CUR *curp) noex {
    	int		rs ;
	if ((rs = pcsunodes_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (pcsunodes_magic) */
	return rs ;
}
/* end subroutine (pcsunodes_curbegin) */

int pcsunodes_curend(PN *op,PN_CUR *curp) noex {
    	int		rs ;
	if ((rs = pcsunodes_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (pcsunodes_magic) */
	return rs ;
}
/* end subroutine (pcsunodes_curend) */

int pcsunodes_enum(PN *op,PN_CUR *curp,char *rbuf,int rlen) noex {
	int		rs ;
	int		rl = 0 ; /* return-value */
	if ((rs = pcsunodes_magic(op,curp,rbuf)) >= 0) {
	    int		i = (curp->i >= 0) ? (curp->i+1) : 0 ;
	    if (i < op->n) {
	        if ((rs = sncpy1(rbuf,rlen,op->unodes[i])) >= 0) {
	            rl = rs ;
		    curp->i = i ;
	        }
	    } else {
	        rbuf[0] = '\0' ;
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (pcsunodes_magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (pcsunodes_enum) */

int pcsunodes_audit(PN *op) noex {
    	int		rs ;
	if ((rs = pcsunodes_magic(op)) >= 0) {
	    rs = SR_OK ;
	} /* end if (pcsunodes_magic) */
	return rs ;
}
/* end subroutine (pcsunodes_mat) */


/* local subroutines */

local int pcsunodes_mktab(PN *op,vecpstr *ulp) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = vecpstr_count(ulp)) >= 0) {
	    cint	vsz = ((rs+1) * szof(cchar *)) ;
	    if ((rs = vecpstr_strsize(ulp)) >= 0) {
		cint	ssz = rs ;
	        int	tsz = (vsz + rs) ;
		if (char *bp ; (rs = lm_mall(tsz,&bp)) >= 0) {
	            cchar	**va = (cchar **) bp ;
		    char	*st = (bp + vsz) ;
	            if ((rs = vecpstr_strmk(ulp,st,ssz)) >= 0) {
			if ((rs = vecpstr_loadnodes(ulp,va,st)) >= 0) {
			    op->n = rs ;
			    op->unodes = va ;
			}
	            } /* end if (record-table allocated) */
		    if (rs < 0) {
			lm_free(bp) ;
		    }
	        } /* end if (m-a) */
	    } /* end if (vecpstr_strsize) */
	} /* end if (vecpstr_count) */

#if	CF_DEBUGS
	debugprintf("pcsunodes_mktab: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (pcsunodes_mktab) */

local int vecpstr_loadnodes(vecpstr *ulp,cchar **va,char *st) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecpstr_recsize(ulp)) >= 0) {
	    cint	rsz = rs ;
	    if (int *rec ; (rs = lm_mall(rsz,&rec)) >= 0) {
	        if ((rs = vecpstr_recmk(ulp,rec,rsz)) >= 0) {
		    cint	n = rs ;
		    for (int i = 0 ; (i < n) && (rec[i] >= 0) ; i += 1) {
		        if (rec[i] > 0) {
#if	CF_DEBUGS
			    debugprintf("vecpstr_loadnodes: i=%u c=%u v=%s\n",
				i,c,(st+rec[i])) ;
#endif
			    va[c++] = (st + rec[i]) ;
		        }
		    } /* end for */
	        } /* end if */
	        va[c] = nullptr ;
	        rs1 = lm_free(rec) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (vecpstr_recsize) */
#if	CF_DEBUGS
	debugprintf("vecpstr_loadnodes: ret rs=%d c=%u\n",rs,c) ;
#endif
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_loadnodes) */


