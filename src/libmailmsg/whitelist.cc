/* whitelist SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* whitelist mail address management */
/* version %I% last-modified %G% */

#define	CF_PARTIAL	0		/* partial-domain match */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	whitelist

	Description:
	This object manages a mail whitelist (or blacklist).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<bfile.h>
#include	<bfliner.h>
#include	<sfx.h>			/* |sfconrtent(3uc)| */
#include	<strn.h>
#include	<sncpyx.h>
#include	<snwcpyx.h>
#include	<strx.h>
#include	<localmisc.h>

#include	"splitaddr.h"
#include	"whitelist.h"

import libutil ;

/* local defines */

#ifndef	CF_PARTIAL
#define	CF_PARTIAL	0		/* partial-domain match */
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* exported variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int whitelist_ctor(whitelist *op,Args ... args) noex {
	WHITELIST	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->wlp = new(nothrow) vecstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (whitelist_ctor) */

static int whitelist_dtor(whitelist *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->wlp) {
		delete op->wlp ;
		op->wlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (whitelist_dtor) */

template<typename ... Args>
static inline int whitelist_magic(whitelist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == WHITELIST_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (whitelist_magic) */

static int	mkaddr(char *,int,cchar *,int) noex ;

extern "C" {
    static int	vcmpaddr(cchar **,cchar **) noex ;
}


/* local variables */

constexpr bool		f_partial = CF_PARTIAL ;


/* exported variables */


/* exported subroutines */

int whitelist_open(whitelist *op,cchar *fname) noex {
	cint		vn = WHITELIST_DEFENTS ;
	cint		vo = 0 ;
	int		rs ;
	if ((rs = whitelist_ctor(op)) >= 0) {
	    if ((rs = vecstr_start(op->wlp,vn,vo)) >= 0) {
	        op->magic = WHITELIST_MAGIC ;
	        if (fname) {
	            rs = whitelist_fileadd(op,fname) ;
	        } /* end if */
	        if (rs < 0) {
	            vecstr_finish(op->wlp) ;
	            op->magic = 0 ;
	        }
	    }
	    if (rs < 0) {
		whitelist_dtor(op) ;
	    }
	} /* end if (whitelist_ctor) */
	return rs ;
}
/* end subroutine (whitelist_open) */

int whitelist_close(whitelist *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = whitelist_magic(op)) >= 0) {
	    {
	        rs1 = vecstr_finish(op->wlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = whitelist_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (whitelist_close) */

int whitelist_fileadd(whitelist *op,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = whitelist_magic(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if (char *mbuf ; (rs = malloc_mailaddr(&mbuf)) >= 0) {
		    cint	mlen = rs ;
		    if (bfile lf ; (rs = lf.open(fname,"r",0)) >= 0) {
			if (bfliner bl ; (rs = bl.start(&lf)) >= 0) {
			    cchar	*lp ;
	    		    while ((rs = bl.getln(&lp)) > 0) {
				cchar	*cp ;
				if (int cl ; (cl = sfcontent(lp,rs,&cp)) > 0) {
	            		    if ((rs = mkaddr(mbuf,mlen,cp,cl)) > 0) {
	                		n += 1 ;
	                		rs = vecstr_add(op->wlp,mbuf,rs) ;
		    		    } else if (rs == SR_OVERFLOW) {
					rs = SR_OK ;
	            		    } /* end if */
	        		} /* end if (have-content) */
	        		if (rs < 0) break ;
	    		    } /* end while (reading lines) */
			    rs1 = bl.finish ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (bfliner) */
	                rs1 = lf.close ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (bfile) */
		    rs1 = malloc_free(mbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (whitelist_fileadd) */

int whitelist_get(whitelist *op,int i,cchar **rpp) noex {
	int		rs ;
	if ((rs = whitelist_magic(op)) >= 0) {
	    cchar	*cp ;
	    rs = vecstr_get(op->wlp,i,&cp) ;
	    if (rpp) {
	        *rpp = (rs >= 0) ? cp : nullptr ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (whitelist_get) */

int whitelist_read(whitelist *op,char *rbuf,int rlen,int idx) noex {
	int		rs ;
	if ((rs = whitelist_magic(op,rbuf)) >= 0) {
	    cchar	*cp ;
	    if ((rs = vecstr_get(op->wlp,idx,&cp)) >= 0) {
	        rs = sncpy1(rbuf,rlen,cp) ;
	}
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (whitelist_fetch) */

int whitelist_count(whitelist *op) noex {
	int		rs ;
	if ((rs = whitelist_magic(op)) >= 0) {
	    rs = vecstr_count(op->wlp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (whitelist_count) */

int whitelist_prematch(whitelist *op,cchar *ta) noex {
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = whitelist_magic(op,ta)) >= 0) {
	    rs = SR_INVALID ;
	    if (ta[0]) {
	        vecstr	*lp = op->wlp ;
	        if_constexpr (f_partial) {
	             if (strbrk(ta,"@!") != nullptr) {
	                 if ((rs = vecstr_search(lp,ta,vcmpaddr,np)) >= 0) {
	                     f = true ;
	                 }
	             }
	        } else {
	            splitaddr	ac ; /* address-candidate */
	            if ((rs = splitaddr_start(&ac,ta)) >= 0) {
	                cchar	*cp ;
	                for (int i = 0 ; lp->get(i,&cp) >= 0 ; i += 1) {
	                    if (cp) {
	            		splitaddr	aw ; /* address-white */
	                        if ((rs = splitaddr_start(&aw,cp)) >= 0) {
        			    {
	                                 rs = splitaddr_prematch(&aw,&ac) ;
	                                 f = (rs > 0) ;
        			    }
	                            rs1 = splitaddr_finish(&aw) ;
	                            if (rs >= 0) rs = rs1 ;
	                        } /* end if (splitaddr) */
	                    } /* end if (non-null) */
	                    if ((rs < 0) || f) break ;
	                } /* end for */
	                rs1 = splitaddr_finish(&ac) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (splitaddr) */
	        } /* end if_constepxr (f_partial) block */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (whitelist_prematch) */

int whitelist_audit(whitelist *op) noex {
	int		rs ;
	if ((rs = whitelist_magic(op)) >= 0) {
	    rs = vecstr_audit(op->wlp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (whitelist_audit) */


/* private subroutines */

static int mkaddr(char *mbuf,int mlen,cchar *lp,int ll) noex {
	if (cchar *tp ; (tp = strnchr(lp,ll,'#')) != nullptr) {
	    ll = intconv(tp - lp) ;
	}
	return snwcpyopaque(mbuf,mlen,lp,ll) ;
}
/* end subroutine (mkaddr) */

static int vcmpaddr(cchar **e1pp,cchar **e2pp) noex {
	cchar		*e1p = *e1pp ;
	cchar		*e2p = *e2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = 1 ;
	    if (e1pp) {
		rc = -1 ;
	        if (e2p) {
	            cchar	*h1p = strchr(e1p,'@') ;
	            cchar	*h2p = strchr(e2p,'@') ;
	            if ((h1p == nullptr) || (h2p == nullptr)) {
	                if (h1p == nullptr) {
	                    rc = strcasecmp(e1p,(h2p + 1)) ;
	                } else {
	                    rc = strcasecmp((h1p + 1),e2p) ;
	                }
	            } else {
	                rc = strcasecmp(e1p,e2p) ;
	            }
	        }
	    }
	}
	return rc ;
}
/* end subroutine (vcmpaddr) */


