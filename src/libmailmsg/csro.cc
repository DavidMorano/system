/* csro SUPPORT (ComSat Receive Offset) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* email ComSat Receive Offset (CSRO) processing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	csro

	Description:
	This object processes and manipulates email CSRO addresses.

	Implementation note:
	Since email addresses can share the same host part, and
	further since we want to be able to group email addresses
	by their host parts, we store the host part of all email
	addresses in a separate structure than the local part.  All
	email addresses will be hashed with the index being the
	host part of the address.  This allows super quick retrival
	of those email addresses belonging to a particular host
	(maybe this was not an imperative but that is what we did).
	The host parts are stored in a simple VECSTR container
	object and all email addresess are registered in a VECOBJ
	container object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<hdb.h>
#include	<vecstr.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"csro.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* getlenstr(eu)| */

/* local defines */

#define	VALUE		csro_val


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

template<typename ... Args>
static int csro_ctor(csro *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    op->magic = 0 ;
	    op->elp = nullptr ;
	    rs = SR_NOMEM ;
	    if ((op->nlp = new(nothrow) vecstr) != np) {
	        if ((op->elp = new(nothrow) vecobj) != np) {
		    rs = SR_OK ;
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->nlp ;
		    op->nlp = nullptr ;
		}
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (csro_ctor) */

static int csro_dtor(csro *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	    if (op->nlp) {
		delete op->nlp ;
		op->nlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (csro_dtor) */

template<typename ... Args>
static inline int csro_magic(csro *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CSRO_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (csro_magic) */

static int	value_start(VALUE *,cchar *,cchar *,off_t) noex ;
static int	value_finish(VALUE *) noex ;

extern "C" {
    static int	vcmpname(cvoid **,cvoid **) noex ;
    static int	vcmpentry(cvoid **,cvoid **) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int csro_start(csro *op,int n) noex {
	int		rs ;
	if ((rs = csro_ctor(op)) >= 0) {
	    int		vo = VECSTR_OCONSERVE ;
	    int		sz = szof(csro_val) ;
	    if (n <= 1) n = CSRO_DEFENTS ;
	    if ((rs = vecstr_start(op->nlp,n,vo)) >= 0) {
		vo = 0 ;
	    	if ((rs = vecobj_start(op->elp,sz,n,vo)) >= 0) {
	            op->magic = CSRO_MAGIC ;
	        } /* end if (vecobj_start) */
	        if (rs < 0) {
	           vecstr_finish(op->nlp) ;
		}
	    } /* end if (vecstr_start) */
	    if (rs < 0) {
		csro_dtor(op) ;
	    }
	} /* end if (csro_ctor) */
	return rs ;
}
/* end subroutine (csro_start) */

int csro_finish(csro *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = csro_magic(op)) >= 0) {
	    vecobj	*elp = op->elp ;
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(elp,i,&vp) >= 0 ; i += 1) {
	        if (vp) {
	    	    csro_val	*valp = (csro_val *) vp ;
		    rs1 = value_finish(valp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    {
	        rs1 = vecobj_finish(elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecstr_finish(op->nlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = csro_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_finish) */

int csro_add(csro *op,cchar *mailname,cchar *fname,off_t moff) noex {
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	if ((rs = csro_magic(op,mailname,fname)) >= 0) {
	    csro_val	ve ;
	    if ((rs = value_start(&ve,mailname,fname,moff)) >= 0) {
	        vecstr	*nlp = op->nlp ;
	        vecobj	*vlp = op->elp ;
	        cint	nlen = lenstr(mailname) ;
	        bool	f_release = false ;
	        if ((rs = vecstr_findn(nlp,mailname,nlen)) >= 0) {
	            if ((rs = vecobj_search(vlp,&ve,vcmpentry,np)) >= 0) {
		         f_release = true ;
		    }
	        } else if (rs == SR_NOENT) {
	            if ((rs = vecstr_add(nlp,mailname,nlen)) >= 0) {
	                int		ni = rs ;
		        cchar	*sp{} ;
	                if ((rs = vecstr_get(nlp,ni,&sp)) >= 0) {
			    rs = vecobj_add(vlp,&ve) ;
		        }
		    }
	        } /* end if (getting existing name) */
	        if (f_release) {
		    rs1 = value_finish(&ve) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	    } /* end if (value) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_add) */

/* do we already have an entry like this? */
int csro_already(csro *op,cchar *mailname,cchar *fname,off_t moff) noex {
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = csro_magic(op,mailname,fname)) >= 0) {
	    csro_val	ve ;
	    if ((rs = value_start(&ve,mailname,fname,moff)) >= 0) {
	        cint	rsn = SR_NOTFOUND ;
	        if ((rs = vecobj_search(op->elp,&ve,vcmpentry,np)) >= 0) {
	            f = true ;
	        } else if (rs == rsn) {
	            rs = SR_OK ;
	        }
	        rs1 = value_finish(&ve) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (value) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (csro_already) */

/* return the number of hosts seen so far */
int csro_countnames(csro *op) noex {
	int		rs ;
	if ((rs = csro_magic(op)) >= 0) {
	    rs = vecstr_count(op->nlp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_countnames) */

/* return the count of the number of items in this list */
int csro_count(csro *op) noex {
	int		rs ;
	if ((rs = csro_magic(op)) >= 0) {
	    rs = vecobj_count(op->elp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_count) */

/* sort the strings in the vector list */
int csro_sort(csro *op) noex {
	int		rs ;
	if ((rs = csro_magic(op)) >= 0) {
	    rs = vecobj_sort(op->elp,vcmpname) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_sort) */

int csro_ncurbegin(csro *op,csro_ncur *hcp) noex {
	int		rs ;
	if ((rs = csro_magic(op,hcp)) >= 0) {
	    *hcp = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_ncurbegin) */

int csro_ncurend(csro *op,csro_ncur *hcp) noex {
	int		rs ;
	if ((rs = csro_magic(op,hcp)) >= 0) {
	    *hcp = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_ncurend) */

int csro_vcurbegin(csro *op,csro_vcur *vcp) noex {
	int		rs ;
	if ((rs = csro_magic(op,vcp)) >= 0) {
	    *vcp = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_vcurbegin) */

int csro_vcurend(csro *op,csro_vcur *vcp) noex {
	int		rs ;
	if ((rs = csro_magic(op,vcp)) >= 0) {
	    *vcp = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* ens subroutine (csro_vcurend) */

int csro_getname(csro *op,csro_ncur *hcp,cchar **hnpp) noex {
	int		rs ;
	if ((rs = csro_magic(op,hcp)) >= 0) {
	    cchar	*dump ;
	    int		i = (*hcp >= 0) ? (*hcp + 1) : 0 ;
	    if (hnpp == nullptr) hnpp = &dump ;
	    while ((rs = vecstr_get(op->nlp,i,hnpp)) >= 0) {
	        if (*hnpp != nullptr) break ;
	        i += 1 ;
	    } /* end while */
	    *hcp = (rs >= 0) ? i : -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_getname) */

/* fetch the next entry value that matches the given host name */
int csro_getvalue(csro *op,cc *mailname,csro_vcur *vcp,csro_val **vepp) noex {
	int		rs ;
	if ((rs = csro_magic(op,mailname,vcp,vepp)) >= 0) {
	    int		i = (*vcp < 0) ? 0 : (*vcp + 1) ;
	    csro_val	*valp = nullptr ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        bool	f = (vp != nullptr) ;
		valp = (csro_val *) vp ;
	        f = f && (strcmp(valp->mailname,mailname) == 0) ;
	        if (f) break ;
	        i += 1 ;
	    } /* end while */
	    if ((rs >= 0) && valp) {
		*vepp = valp ;
	        *vcp = i ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csro_getvalue) */


/* private subroutines */

static int value_start(VALUE *ep,cchar *mailname,cchar *fname,off_t moff) noex {
	int		rs ;
	int		sz = 1 ;
	memclear(ep) ;
	ep->moff = moff ;
	sz += (lenstr(mailname)+1) ;
	if (fname != nullptr) {
	   sz += (lenstr(fname)+1) ;
	}
	if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) {
	    ep->mailname = bp ;
	    bp = (strwcpy(bp,fname,-1)+1) ;
	    ep->fname = bp ;
	    if (fname != nullptr) {
	        bp = (strwcpy(bp,fname,-1)+1) ;
	    } else {
		*bp = '\0' ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (value_start) */

static int value_finish(VALUE *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    {
		void *vp = voidp(ep->mailname) ;
		rs1 = libmem.free(vp) ;
		if (rs >= 0) rs = rs1 ;
	        ep->mailname = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (value_finish) */

static int vcmpname(cvoid **v1pp,cvoid **v2pp) noex {
	csro_val	**e1pp = (csro_val **) v1pp ;
	csro_val	**e2pp = (csro_val **) v2pp ;
	int		rc = 0 ;
	{
	    csro_val	*e1p = *e1pp ;
	    csro_val	*e2p = *e2pp ;
	    if (e1p || e2p) {
	        if (e1p) {
	            if (e2p) {
		        cchar	*s1 = e1p->mailname ;
		        cchar	*s2 = e2p->mailname ;
	                if ((rc = strcmp(s1,s2)) == 0) {
	                    rc = intconv(e1p->moff - e2p->moff) ;
	                }
	            } else {
	                rc = -1 ;
		    }
	        } else {
	            rc = 1 ;
	        }
	    } 
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpname) */

static int vcmpentry(cvoid **v1pp,cvoid **v2pp) noex {
	csro_val	**e1pp = (csro_val **) v1pp ;
	csro_val	**e2pp = (csro_val **) v2pp ;
	int		rc = 0 ;
	{
	    csro_val	*e1p = *e1pp ;
	    csro_val	*e2p = *e2pp ;
	    if (e1p || e2p) {
	        if (e1p) {
	            if (e2p) {
		        cchar	*s1 = e1p->mailname ;
		        cchar	*s2 = e2p->mailname ;
	                if ((rc = strcmp(s1,s2)) == 0) {
	                    if ((rc = intconv(e1p->moff - e2p->moff)) == 0) {
	                        if (e1p->fname == nullptr) {
	                            rc = 1 ;
	                        } else if (e1p->fname == nullptr) {
	                            rc = -1 ;
	                        } else {
	                            rc = strcmp(e1p->fname,e2p->fname) ;
		                }
		            }
	                } /* end if */
	            } else {
	                rc = -1 ;
		    }
	        } else {
	            rc = 1 ;
	        }
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpentry) */


