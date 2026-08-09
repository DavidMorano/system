/* recipient SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* email recipient processing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	recipient

	Description:
	This object processes and manipulates email recipient
	addresses.

	Implementation note:
	Since email addresses can share the same host part, and
	further since we want to be able to group email addresses
	by their host parts, we store the host part of all email
	addresses in a separate structure then the local part.  All
	email addresses will be hashed with the index being the
	host part of the address.  This allows super quick retrival
	of those email addresses belonging to a particular host
	(maybe this wasn't an imperative but that is what we did).
	The host parts are stored in a simple VECSTR container
	object and all email addresess are stored in a HDB container
	object.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<netdb.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<hdb.h>			/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<strwcpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"recipient.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	RC		recipient
#define	RC_HCUR		recipient_hcur
#define	RC_VCUR		recipient_vcur
#define	RC_VAL		recipient_val
#define	RC_DEFENTS	10
#define	RC_MAGIC	RECIPIENT_MAGIC


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int recipient_ctor(recipient *op,Args ... args) noex {
    	RECIPIENT	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->hlp = new(nt) hdb) != np) ylikely {
	        if ((op->nlp = new(nt) vecstr) != np) ylikely {
		    rs = SR_OK ;
		} /* end if (new-vecstr) */
		if (rs < 0) {
		    delete op->hlp ;
		    op->hlp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-hdb) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (recipient_ctor) */

local int recipient_dtor(recipient *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->nlp) ylikely {
		delete op->nlp ;
		op->nlp = nullptr ;
	    }
	    if (op->hlp) ylikely {
		delete op->hlp ;
		op->hlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (recipient_dtor) */

template<typename ... Args>
local inline int recipient_magic(recipient *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == RECIPIENT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (recipient_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int recipient_start(RC *op,int vn) noex {
	int		rs ;
	if (vn <= 1) vn = RC_DEFENTS ;
	if ((rs = recipient_ctor(op)) >= 0) ylikely {
	    if ((rs = hdb_start(op->hlp,vn,0,nullptr,nullptr)) >= 0) ylikely {
	        cint	vo = vecstrm.conserve ;
	        if ((rs = vecstr_start(op->nlp,vn,vo)) >= 0) ylikely {
		    rs = RC_MAGIC ;
		}
	        if (rs < 0) {
	            hdb_finish(op->hlp) ;
	        } /* end if (error) */
	    } /* end if */
	    if (rs < 0) {
		recipient_dtor(op) ;
	    } /* end if (error) */
	} /* end if (recipient_ctor) */
	return rs ;
} /* end subroutine (recipient_start) */

int recipient_finish(RC *op) noex {
    	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		rs2 ;
	if ((rs = recipient_magic(op)) >= 0) ylikely {
	    /* pop the hash table first */
	    if (hdb_cur kcur ; (rs = hdb_curbegin(op->hlp,&kcur)) >= 0) {
		hdb_dat		key{} ;
		hdb_dat		val{} ;
	        while ((rs2 = hdb_curenum(op->hlp,&kcur,&key,&val)) >= 0) {
	            RC_VAL *vap = resumelife<RC_VAL>(val.buf) ;
	            if (val.buf) {
	                if (vap->a) {
	                    rs1 = mem.free(vap->a) ;
			    if (rs >= 0) rs = rs1 ;
	                    vap->a = nullptr ;
	                } /* end if (memory-release) */
	                if (vap->localpart) {
			    voidp vp = voidp(vap->localpart) ;
	                    rs1 = mem.free(vp) ;
			    if (rs >= 0) rs = rs1 ;
	                    vap->localpart = nullptr ;
	                } /* end if (memory-release) */
		        {
	                    rs1 = mem.free(vap) ;
			    if (rs >= 0) rs = rs1 ;
		        } /* end if (memory-release) */
	            } /* end if (non-null) */
	        } /* end while */
	        rs1 = hdb_curend(op->hlp,&kcur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	    {
	        rs1 = hdb_finish(op->hlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    /* pop the vector of strings */
	    if (op->nlp) {
	        rs1 = vecstr_finish(op->nlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = recipient_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_finish) */

int recipient_add(RC *op,cc *host,cc *locvap,int type) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	if ((host == nullptr) || (type < 0)) {
	    host = RECIPIENT_NOHOST ;
	}
	if ((rs = recipient_magic(op,locvap)) >= 0) {
	    if (char *hbuf ; (rs = mem.hostname(&hbuf)) >= 0) {
	        cint hl = intconv(strwcpylc(hbuf,host,rs) - hbuf) ;
	        if (hl > 0) {
		    hdb_dat	key ;
		    hdb_dat	val{} ;
		    RC_VAL	ve{} ;
		    int		vidx = -1 ;
		    cchar	*cp{} ;
		    /* is this host in the hash table? */
		    key.buf = hbuf ;
		    key.len = hl ;
		    if ((rs = hdb_fetch(op->hlp,key,np,&val)) >= 0) {
	    	        RC_VAL	*valp = (RC_VAL *) val.buf ;
	    	        key.buf = valp->hostpart ;
	    	        key.len = -1 ;
		    } else if (rs == SR_NOENT) {
	    	        if ((rs = vecstr_add(op->nlp,host,hl)) >= 0) {
	        	    vidx = rs ;
	        	    if ((rs = vecstr_get(op->nlp,rs,&cp)) >= 0) {
	            	        key.buf = cp ;
	            	        key.len = -1 ;
			    }
	    	        } /* end if (vecstr_get) */
		    } /* end if */
		    if (rs >= 0) {
	    	        if ((rs = mem.strw(locvap,-1,&cp)) >= 0) {
	        	    cint	osz = szof(RC_VAL) ;
	        	    ve.type = type ;
	        	    ve.a = nullptr ;
	        	    ve.hostpart = charp(key.buf) ;
	        	    ve.localpart = cp ;
			    if (void *vp{} ; (rs = mem.mall(osz,&vp)) >= 0) {
	                        RC_VAL	*valp = (RC_VAL *) vp ;
	                        val.buf = valp ;
	                        val.len = osz ;
	                        rs = hdb_store(op->hlp,key,val) ;
	                        if (rs < 0) {
	                            mem.free(vp) ;
	                        } /* end if (error) */
	                    } /* end if (m-a) */
	                    if (rs < 0) {
	                        voidp vp = voidp(ve.localpart) ;
	                        mem.free(vp) ;
	                    } /* end if (error) */
	                } /* end if (m-a) */
	                if ((rs < 0) && (vidx >= 0)) {
	                    vecstr_del(op->nlp,vidx) ;
	                } /* end if (error) */
	            } /* end if (ok) */
		} /* end if (to-lower-case) */
		rs1 = mem.free(hbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_add) */

/* fetch on a address tripple */
int recipient_already(RC *op,cchar *host,cchar *locvap,int type) noex {
	int		rs ;
	int		rs1 ;
	if ((host == nullptr) || (type < 0)) {
	    host = RECIPIENT_NOHOST ;
	}
	if ((rs = recipient_magic(op,locvap)) >= 0) {
	    if (char *hbuf ; (rs = mem.hostname(&hbuf)) >= 0) {
		cint hl = intconv(strwcpylc(hbuf,host,rs) - hbuf) ;
		if (hl > 0) {
	            hdb_dat	key ;
	            hdb_dat	val{} ;
	            key.buf = hbuf ;
	            key.len = hl ;
	            if (hdb_cur cur ; (rs = hdb_curbegin(op->hlp,&cur)) >= 0) {
	                while ((rs = hdb_fetch(op->hlp,key,&cur,&val)) >= 0) {
		            RC_VAL	*valp = (RC_VAL *) val.buf ;
	                    bool	f = true ;
	                    f = f && (type == valp->type) ;
	                    f = f && (strcmp(locvap,valp->localpart) == 0) ;
	                    if (f) break ;
	                } /* end while */
	                rs1 = hdb_curend(op->hlp,&cur) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (curosr) */
		    rs1 = mem.free(hbuf) ;
	    	    if (rs >= 0) rs = rs1 ;
		} /* end if (to-lower-case) */
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_already) */

int recipient_counthosts(RC *op) noex {
	int		rs ;
	if ((rs = recipient_magic(op)) >= 0) {
	    rs = vecstr_count(op->nlp) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_counthosts) */

/* return the count of the number of items in this list */
int recipient_count(RC *op) noex {
	int		rs ;
	if ((rs = recipient_magic(op)) >= 0) {
	    rs = hdb_count(op->hlp) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_count) */

#ifdef	COMMENT

/* sort the strings in the vector list */
int recipient_sort(RC *vhp) noex {
    	int		rs ;
	int		idx = 0 ;
	if ((rs = recipient_magic(op)) >= 0) {
	    if (vhp->c > 1) {
	        csize	sidx = size_t(vhp->i) ;
	        csize	esz = szof(char *) ;
	        qsort(vhp->va,sidx,esz,ourcmp) ;
	        idx = vhp->i ;
	    }
	} /* end if (magic) */
	return (rs >= 0) idx : rs ;
} /* end subroutine (recipient_sort) */

#endif /* COMMENT */

int recipient_hcurenum(RC *op,RC_HCUR *hcp,cchar **hnpp) noex {
	int		rs ;
	if ((rs = recipient_magic(op,hcp)) >= 0) {
	    cchar	*dump{} ;
	    int		i = (*hcp >= 0) ? (*hcp + 1) : 0 ;
	    if (hnpp == nullptr) hnpp = &dump ;
	    while ((rs = vecstr_get(op->nlp,i,hnpp)) >= 0) {
	        if (*hnpp != nullptr) break ;
	        i += 1 ;
	    } /* end while */
	*hcp = (rs >= 0) ? i : -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_hcurenum) */

/* fetch the next entry value that matches the given host name */
int recipient_fetchvalue(RC *op,cc *host,RC_VCUR *vcp,RC_VAL **vepp) noex {
	int		rs ;
	if ((rs = recipient_magic(op,vcp)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    key.buf = host ;
	    key.len = -1 ;
	    if ((rs = hdb_fetch(op->hlp,key,vcp,&val)) >= 0) {
	        if (vepp) {
	            *vepp = (RC_VAL *) val.buf ;
	        }
	        rs = val.len ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_fetchvalue) */

int recipient_hcurbegin(RC *op,RC_HCUR *hcp) noex {
    	int		rs ;
	if ((rs = recipient_magic(op,hcp)) >= 0) {
	    *hcp = -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_hcurbegin) */

int recipient_hcurend(RC *op,RC_HCUR *hcp) noex {
    	int		rs ;
	if ((rs = recipient_magic(op,hcp)) >= 0) {
	    *hcp = -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_hcurend) */

int recipient_vcurbegin(RC *op,RC_VCUR *vcp) noex {
	int		rs ;
	if ((rs = recipient_magic(op,vcp)) >= 0) {
	    rs = hdb_curbegin(op->hlp,vcp) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (recipient_vcurbegin) */

int recipient_vcurend(RC *op,RC_VCUR *vcp) noex {
	int		rs ;
	if ((rs = recipient_magic(op,vcp)) >= 0) {
	    rs = hdb_curend(op->hlp,vcp) ;
	} /* end if (magic) */
	return rs ;
} /* ens subroutine (recipient_vcurend) */


