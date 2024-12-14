/* querystr SUPPORT */
/* lang=C++11 */

/* Query-String manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-09-25, David A­D­ Morano
	This module was adapted from some previous code.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We take a raw "query-string" and process it so that its
	components can be accessed.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<vector>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<strn.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<strwcmp.h>
#include	<char.h>
#include	<cfhex.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"querystr.h"


/* local defines */

#define	KEYVAL	struct keyval


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::vector ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef querystr_cur	cur ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct keyval {
	cchar		*kp = nullptr ;
	cchar		*vp = nullptr ;
	int		kl = 0 ;
	int		vl = 0 ;
	keyval(cchar *akp,int akl,cchar *avp,int avl) noex {
	    kp = akp ;
	    vp = avp ;
	    kl = akl ;
	    vl = avl ;
	} ;
    } ; /* end struct (keyval) */
    class subinfo {
	querystr	*op ;
	vector<keyval>	kvs ;
	char		*tbuf = nullptr ;
	int		tlen = 0 ;
    public:
	subinfo(querystr *aop) noex : op(aop) { } ;
	~subinfo() {
	    if (tbuf != nullptr) {
		uc_free(tbuf) ;
		tbuf = nullptr ;
		tlen = 0 ;
	    }
	    op = nullptr ;
	} ;
	int tsize(int nlen) noex {
	     int	rs = 0 ;
	     if (nlen > tlen) {
	        if (tbuf != nullptr) {
		    uc_free(tbuf) ;
		    tbuf = nullptr ;
	        }
		tlen = nlen ;
		rs = uc_malloc((tlen+1),&tbuf) ;
	     }
	     return rs ;
	} ;
	int split(cchar *,int) noex ;
	int procpair(cchar *,int) noex ;
	int fixval(char *,int,cchar *,int) noex ;
	int load() noex ;
	int store(cchar *,int,cchar *,int) noex ;
    } ; /* end struct (subinfo) */
}


/* forward references */

template<typename ... Args>
static int querystr_ctor(querystr *op,Args ... args) noex {
    	QUERYSTR	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ; /* dangerous */
	    if ((op->spp = new(nothrow) strpack) != np) {
		rs = SR_OK ;
	    } /* end if (new-strpack) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (querystr_ctor) */

static int querystr_dtor(querystr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->spp) {
		delete op->spp ;
		op->spp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (querystr_dtor) */

static char	*strwebhex(char *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int querystr_start(querystr *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = querystr_ctor(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	        cint	llen = rs ;
	        if ((rs = strpack_start(op->spp,llen)) >= 0) {
	            subinfo		si(op) ;
	            op->open.packer = true ;
	            if ((rs = si.split(sp,sl)) >= 0) {
		        rs = si.load() ;
	            } /* end if (subinfo) */
	            if (rs < 0) {
	                op->open.packer = false ;
		        strpack_finish(op->spp) ;
	            }
	        } /* end if (strpack_start) */
	    } /* end if (getbufsize) */
	    if (rs < 0) {
		querystr_dtor(op) ;
	    }
	} /* end if (querystr_ctor) */
	return rs ;
}
/* end subroutine (querystr_start) */

int querystr_finish(querystr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->kv) {
	        rs1 = uc_free(op->kv) ;
	        if (rs >= 0) rs = rs1 ;
	        op->kv = nullptr ;
	    }
	    if (op->open.packer) {
	        op->open.packer = false ;
	        rs1 = strpack_finish(op->spp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = querystr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (querystr_finish) */

int querystr_count(querystr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->n ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (querystr_count) */

int querystr_already(querystr *op,cchar *kstr,int klen) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (op && kstr) {
	    cint	n = op->n ;
	    rs = SR_OK ;
	    if (klen < 0) klen = strlen(kstr) ;
	    for (int i = 0 ; i < n ; i += 1) {
	        f = (strwcmp(op->kv[i][0],kstr,klen) == 0) ;
	        if (f) break ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end if (querystr_already) */

int querystr_curbegin(querystr *op,cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    curp->i = -1 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (querystr_curbegin) */

int querystr_curend(querystr *op,cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    curp->i = -1 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (querystr_curend) */

/* fetch the next entry value matching the given key */
int querystr_fetch(querystr *op,cc *kstr,int klen,cur *curp,cc **rpp) noex {
	int		rs = SR_FAULT ;
	int		vl = 0 ;
	if (op && curp) {
	    rs = SR_OK ;
	    if (klen < 0) klen = strlen(kstr) ;
	    if (op->n > 0) {
	        int		i = (curp->i + 1) ;
	        if (i < op->n) {
	            cchar	*(*kv)[2] = op->kv ;
	            bool	f = false ;
	            while (i < op->n) {
		        f = (strwcmp(kv[i][0],kstr,klen) == 0) ;
		        if (f) break ;
		        i += 1 ;
	            } /* end while */
	            if (f) {
		        cchar	*vp = kv[i][1] ;
		        curp->i = i ;
		        vl = strlen(vp) ;
		        if (rpp != nullptr) *rpp = vp ;
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } else {
		    rs = SR_NOTFOUND ;
	        }
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (querystr_fetch) */

int querystr_enum(querystr *op,cur *curp,cc **kpp,cc **vpp) noex {
	int		rs = SR_FAULT ;
	int		vl = 0 ;
	if (op && curp) {
	    rs = SR_OK ;
	    if (op->n > 0) {
	        int	i = (curp->i + 1) ;
	        if (i < op->n) {
	            cchar	*(*kv)[2] = op->kv ;
		    {
		        cchar	*kp = kv[i][0] ;
		        if (kpp != nullptr) *kpp = kp ;
		    }
	            {
		        cchar	*vp = kv[i][1] ;
		        vl = strlen(vp) ;
		        if (vpp != nullptr) *vpp = vp ;
	            }
		    curp->i = i ;
	        } else {
		    rs = SR_NOTFOUND ;
	        }
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (querystr_enum) */


/* private subroutines */

int subinfo::split(cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	cchar		*tp ;
	while ((tp = strnchr(sp,sl,'&')) != nullptr) {
	    cint	cl = (tp-sp) ;
	    cchar	*cp = sp ;
	    if (cl > 0) {
	        rs = procpair(cp,cl) ;
	    }
	    sl -= ((tp+1)-sp) ;
	    sp = (tp+1) ;
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && (sl > 0)) {
	    rs = procpair(sp,sl) ;
	}
	return rs ;
}
/* end subroutine (subinfo::split) */

int subinfo::procpair(cchar *sbuf,int slen) noex {
	int		rs = SR_OK ;
	int		sl ;
	cchar		*sp ;
	if (slen < 0) slen = strlen(sbuf) ;
	if ((sl = sfshrink(sbuf,slen,&sp)) > 0) {
	    int		kl = sl ;
	    int		vl = 0 ;
	    cchar	*tp ;
	    cchar	*kp = sp ;
	    cchar	*vp = nullptr ;
	    if ((tp = strnchr(sp,sl,'=')) != nullptr) {
	        kl = (tp - sp) ;
	        while (kl && CHAR_ISWHITE(*kp)) kl -= 1 ;
	        vp = (tp + 1) ;
	        vl = ((sp + sl) - vp) ;
	    }
	    if (kl > 0) {
	        while ((vl > 0) && CHAR_ISWHITE(*vp)) {
	            vp += 1 ;
	            vl -= 1 ;
	        }
	        if ((vl > 0) && ((strnpbrk(vp,vl,"%+\t")) != nullptr)) {
	            if ((rs = tsize(vl)) >= 0) {
	                rs = fixval(tbuf,tlen,vp,vl) ;
	                vl = rs ;
	                vp = tbuf ;
		    } /* end if (tsize) */
	        } /* end if (value) */
	        if (rs >= 0) {
	            rs = store(kp,kl,vp,vl) ;
	        }
	    } /* end if (key) */
	} /* end if (sfshrink) */
	return rs ;
}
/* end subroutine (subinfo::procpair) */

int subinfo::fixval(char *rbuf,int rlen,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	int		i = 0 ;
	if (vl > 0) {
	    cchar	*tp ;
	    char	*rp = rbuf ;
	    if (vl > rlen) vl = rlen ;
	    while ((tp = strnpbrk(vp,vl,"%+\t")) != nullptr) {
	        cint	sch = mkchar(*tp) ;
	        if ((tp-vp) > 0) {
	            rp = strwcpy(rp,vp,(tp-vp)) ;
	        }
	        switch (sch) {
	        case '+':
	            if (((rp-rbuf) == 0) || (rp[-1] != ' ')) *rp++ = ' ' ;
	            break ;
	        case '\t':
	            if (((rp-rbuf) == 0) || (rp[-1] != ' ')) *rp++ = ' ' ;
	            break ;
	        case '%':
	            {
	                cint	tl = (vl-(tp-vp)) ;
	                rp = strwebhex(rp,tp,tl) ;
	                tp += MIN(2,tl) ;
	            }
	            break ;
	        } /* end switch */
	        vl -= ((tp+1)-vp) ;
	        vp = (tp+1) ;
	    } /* end while */
	    if ((rs >= 0) && (vl > 0)) {
	        while ((vl > 0) && CHAR_ISWHITE(vp[vl-1])) vl -= 1 ;
	        rp = strwcpy(rp,vp,vl) ;
	    }
	    i = (rp-rbuf) ;
	} /* end if (positive) */
	rbuf[i] = '\0' ;
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (subinfo::fixval) */

int subinfo::store(cchar *kp,int kl,cchar *vp,int vl) noex {
	strpack		*spp = op->spp ;
	int		rs ;
	cchar		*sp ;
	if ((rs = strpack_store(spp,kp,kl,&sp)) >= 0) {
	    kp = sp ;
	    kl = rs ;
	    if (vl > 0) {
	        if ((rs = strpack_store(spp,vp,vl,&sp)) >= 0) {
		    vp = sp ;
		    vl = rs ;
		}
	    } else {
		vp = (kp+kl) ;
	    }
	    if (rs >= 0) {
	        keyval	kv(kp,kl,vp,vl) ;
		try {
	            kvs.push_back(kv) ;
		} catch (...) {
		    rs = SR_NOMEM ;
		}
	    } /* end if (ok) */
	} /* end if (strpack_store) */
	return rs ;
}
/* end subroutine (subinfo::store) */

int subinfo::load() noex {
	cint		n = kvs.size() ;
	cint		esize = 2*sizeof(cchar *) ;
	int		rs ;
	int		sz ;
	void		*p ;
	sz = ((n+1)*esize) ;
	if ((rs = uc_malloc(sz,&p)) >= 0) {
	    op->kv = (cchar *(*)[2]) p ;
	    op->n = n ;
	    for (int i = 0 ; i < n ; i += 1) {
	        op->kv[i][0] = kvs[i].kp ;
	        op->kv[i][1] = kvs[i].vp ;
	    }
	    op->kv[n][0] = nullptr ;
	    op->kv[n][1] = nullptr ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (subinfo::load) */

static char *strwebhex(char *rp,cchar *tp,int tl) noex {
	if ((tl >= 3) && (*tp == '%')) {
	    cint	ch1 = mkchar(tp[1]) ;
	    cint	ch2 = mkchar(tp[2]) ;
	    if (ishexlatin(ch1) && ishexlatin(ch2)) {
	        int	v ;
	        if (cfhexi((tp+1),2,&v) >= 0) {
	            *rp++ = v ;
	        }
	    }
	}
	return rp ;
}
/* end subroutine (strwebhex) */


