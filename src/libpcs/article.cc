/* article SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage an ARTICLE object */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	article

	Description:
	This little object manages some particulars about an
	bulletin-board article.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucmem.h>
#include	<sfx.h>			/* |sfshrink(3uc)| */
#include	<localmisc.h>

#include	"article.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	AR		article


/* namespaces */

using libuc::mem ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int article_ctor(article *op,Args ... args) noex {
    	ARTICLE		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->pathp = new(nt) retpath) != np) {
	        if ((op->ngp = new(nt) ng) != np) {
	            if ((op->envp = new(nt) vechand) != np) {
	                if ((op->msgp = new(nt) dater) != np) {
			    rs = SR_OK ;
		        } /* end if (new-dater) */
		        if (rs < 0) {
		            delete op->envp ;
		            op->envp = nullptr ;
		        } /* end if (error) */
		    } /* end if (new-vechand) */
		    if (rs < 0) {
		        delete op->ngp ;
		        op->ngp = nullptr ;
		    } /* end if (error) */
	        } /* end if (new-ng) */
		if (rs < 0) {
		    delete op->pathp ;
		    op->pathp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-retpath) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (article_ctor) */

local inline int article_dtor(article *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->pathp) {
		delete op->pathp ;
		op->pathp = nullptr ;
	    }
	    if (op->ngp) {
		delete op->ngp ;
		op->ngp = nullptr ;
	    }
	    if (op->envp) {
		delete op->envp ;
		op->envp = nullptr ;
	    }
	    if (op->msgp) {
		delete op->msgp ;
		op->msgp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (article_dtor) */

template<typename ... Args>
local inline int article_magic(article *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == ARTICLE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (article_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int article_start(AR *op) noex {
	int		rs ;
	if ((rs = article_ctor(op)) >= 0) {
	    op->clen = -1 ;
	    op->clines = -1 ;
	    if ((rs = ng_start(op->ngp)) >= 0) {
	        op->fl.ngs = true ;
	        if ((rs = retpath_start(op->pathp)) >= 0) {
	            op->fl.path = true ;
		    if ((rs = vechand_start(op->envp,1,0)) >= 0) {
	                op->fl.envdates = true ;
		    }
		    if (rs < 0) {
	                op->fl.path = true ;
	    	        retpath_finish(op->pathp) ;
		    }
	        }
	        if (rs < 0) {
	            op->fl.ngs = false ;
		    ng_finish(op->ngp) ;
	        }
	    } /* end if (ngs) */
	    if (rs < 0) {
		article_dtor(op) ;
	    } /* end if (error) */
	} /* end if (article_ctor) */
	return rs ;
}
/* end subroutine (article_start) */

int article_finish(AR *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = article_magic(op)) >= 0) {
	    if (op->fl.msgdate) {
	        op->fl.msgdate = false ;
	        rs1 = dater_finish(op->msgp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fl.envdates) {
	        vechand *envp = op->envp ;
	        op->fl.envdates = false ;
	        void *vp ;
	        for (int i = 0 ; envp->get(i,&vp) >= 0 ; i += 1) {
	            dater	*dp = (dater *) vp ;
		    {
		        rs1 = dater_finish(dp) ;
		        if (rs >= 0) rs = rs1 ;
		    }
		    {
		        rs1 = mem.free(dp) ;
		        if (rs >= 0) rs = rs1 ;
		    }
	        } /* end for */
	        rs1 = vechand_finish(op->envp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (envdates) */
	    if (op->fl.path) {
	        op->fl.path = false ;
	        rs1 = retpath_finish(op->pathp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fl.ngs) {
	        op->fl.ngs = false ;
	        rs1 = ng_finish(op->ngp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    for (int i = 0 ; i < articleaddr_overlast ; i += 1) {
	        if (op->af[i]) {
	            op->af[i] = false ;
	            rs1 = ema_finish(op->addr + 1) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    for (int i = 0 ; i < articlestr_overlast ; i += 1) {
	        if (op->strs[i] != nullptr) {
	            void *vp = voidp(op->strs[i]) ;
	            rs1 = mem.free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            op->strs[i] = nullptr ;
	        }
	    } /* end for */
	    {
	        rs1 = article_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_finish) */

int article_addpath(AR *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = article_magic(op,sp)) >= 0) {
	    if (! op->fl.path) {
	        op->fl.path = true ;
	        rs = retpath_start(op->pathp) ;
	    }
	    if (rs >= 0) {
	        rs = retpath_parse(op->pathp,sp,sl) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_addpath) */

int article_addng(AR *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = article_magic(op,sp)) >= 0) {
	    if (! op->fl.ngs) {
	        op->fl.ngs = true ;
	        rs = ng_start(op->ngp) ;
	    }
	    if (rs >= 0) {
	        rs = ng_addparse(op->ngp,sp,sl) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_addng) */

int article_addenvdate(AR *op,dater *d2p) noex {
	int		rs ;
	if ((rs = article_magic(op,d2p)) >= 0) {
	    cint	msz = szof(dater) ;
	    if (dater *dp ; (rs = mem.mall(msz,&dp)) >= 0) {
	        if ((rs = dater_startcopy(dp,d2p)) >= 0) {
		    rs = vechand_add(op->envp,dp) ;
		    if (rs < 0) {
		        dater_finish(dp) ;
		    }
	        }
	        if (rs < 0) {
		    mem.free(dp) ;
	        } /* end if (error) */
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_addenvdate) */

int article_addmsgdate(AR *op,dater *dp) noex {
	int		rs ;
	if ((rs = article_magic(op,dp)) >= 0) {
	    if (! op->fl.msgdate) {
	        op->fl.msgdate = true ;
	        rs = dater_start(op->msgp,nullptr,nullptr,0) ;
	    }
	    if (rs >= 0) {
	        rs = dater_setcopy(op->msgp,dp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_addmsgdate) */

int article_addaddr(AR *op,int type,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = article_magic(op,sp)) >= 0) {
	    cint	n = articleaddr_overlast ;
	    rs = SR_INVALID ;
	    if ((type >= 0) && (type < n)) {
		rs = SR_OK ;
	        if (! op->af[type]) {
	            op->af[type] = true ;
	            rs = ema_start(&op->addr[type]) ;
	        }
	        if (rs >= 0) {
	            rs = ema_parse(&op->addr[type],sp,sl) ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_addaddr) */

int article_addstr(AR *op,int type,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = article_magic(op,sp)) >= 0) {
	    cint	n = articlestr_overlast ;
	    rs = SR_INVALID ;
	    if ((type >= 0) &&  (type < n)) {
		rs = SR_OK ;
	        if (op->strs[type] != nullptr) {
	            void *vp = voidp(op->strs[type]) ;
	            rs = mem.free(vp) ;
	            op->strs[type] = nullptr ;
	        }
	        if (rs >= 0) {
	            if (cchar *cp ; (rs = mem.strw(sp,sl,&cp)) > 0) {
		        op->strs[type] = cp ;
	            }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_addstr) */

/* extract newsgroup names from the "newsgroups" header string */
int article_addparse(AR *op,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if ((rs = article_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (ema aid ; (rs = ema_start(&aid)) >= 0) {
	        if ((rs = ema_parse(&aid,sp,sl)) > 0) {
		    int	cl ;
		    cchar	*cp ;
		    ema_ent *ep ;
	            for (int i = 0 ; ema_get(&aid,i,&ep) >= 0 ; i += 1) {
	                if (ep) {
	                    if ((ep->fl.error) || (ep->al <= 0)) continue ;
	                    if ((cl = sfshrink(ep->ap,ep->al,&cp)) > 0) {
			        n += 1 ;
	                        rs = ng_add(op->ngp,cp,cl,nullptr) ;
		            }
		        } /* end if (non-null) */
	                if (rs < 0) break ;
	            } /* end for */
	        } /* end if (parse) */
	        rs1 = ema_finish(&aid) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ema) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (article_addparse) */

int article_ao(AR *op,uint aoff,uint alen) noex {
    	int		rs ;
	if ((rs = article_magic(op)) >= 0) {
	    op->aoff = aoff ;
	    op->alen = alen ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_ao) */

int article_countenvdate(AR *op) noex {
	int		rs ;
	if ((rs = article_magic(op)) >= 0) {
	    rs = vechand_count(op->envp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_countenvdate) */

int article_getenvdate(AR *op,int i,dater **epp) noex {
	int		rs = SR_FAULT ;
	if (op && epp) {
	    vechand *envp = op->envp ;
	    if (void *vp ; (rs = envp->get(i,&vp)) >= 0) {
		dater *dp = (dater *) vp ;
		*epp = dp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (article_getenvdate) */

int article_getstr(AR *op,int type,cchar **rpp) noex {
	int		rs = SR_OK ;
	if ((rs = article_magic(op)) >= 0) {
	    cint	n = articlestr_overlast ;
	    cchar	*sp = nullptr ;
	    rs = SR_INVALID ;
	    if ((type >= 0) && (type < n)) {
		rs = SR_NOTFOUND ;
	        if ((sp = op->strs[type]) != nullptr) {
		    rs = lenstr(sp) ;
		}
	    } /* end if (valid) */
	    if (rpp) {
	        *rpp = (rs >= 0) ? sp : nullptr ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_getstr) */

int article_getaddrema(AR *op,int type,ema **rpp) noex {
	int		rs = SR_OK ;
	if ((rs = article_magic(op)) >= 0) {
	    cint	n = articleaddr_overlast ;
	    rs = SR_INVALID ;
	    if ((type >= 0) && (type < n)) {
		rs = SR_OK ;
	        if (! op->af[type]) {
	            op->af[type] = true ;
	            rs = ema_start(&op->addr[type]) ;
	        }
	        if (rpp) {
	            *rpp = (rs >= 0) ? (op->addr + type) : nullptr ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (article_getaddrema) */


