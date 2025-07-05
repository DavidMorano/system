/* ucentua SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UCENTUA object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_entua{x}

	Description:
	These subroutines manage some simple tasks for the USERATTR
	object, referenced as 'userattr'. This object is defined
	by UNIX® (really Solaris®) standards.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vecstr.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<intceil.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"ucentua.h"
#include	"ucentxx.hh"

import libutil ;

/* local defines */

#define	SI		storeitem
#define	UA		ucentua
#define	CUA		const ucentua


/* imported namespaces */

using ucent::si_copystr ;		/* local group support subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int userattrent_parseattr(UA *,storeitem *,cchar *,int) noex ;
static int userattrent_parseattrload(UA *,storeitem *,vecstr *,int) noex ;

static int si_attrload(storeitem *,kv_t *,int,cchar *) noex ;

static int sbuf_fmtattrs(sbuf *,kva_t *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentua::parse(char *uabuf,int ualen,cc *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (this && uabuf && sp) {
	    USERATTR *uep = this ;
	    if (sl < 0) sl = lenstr(sp) ;
	    rs = memclear(uep) ;
	    if ((sl > 0) && (sp[0] != '#')) {
	        if (storeitem si ; (rs = si.start(uabuf,ualen)) >= 0) {
	            int		fi = 0 ;
	            for (cc *tp ; (tp = strnchr(sp,sl,':')) != np ; ) {
			cint	tl = intconv(tp - sp) ;
	                cchar	**vpp = nullptr ;
	                switch (fi++) {
	                case 0:
	                    vpp = ccharpp(&name) ;
	                    break ;
	                case 1:
	                    vpp = ccharpp(&qualifier) ;
	                    break ;
	                case 2:
	                    vpp = ccharpp(&res1) ;
	                    break ;
	                case 3:
	                    vpp = ccharpp(&res1) ;
	                    break ;
	                case 4:
	                    rs = userattrent_parseattr(this,&si,sp,tl) ;
	                    break ;
	                } /* end switch */
	                if ((rs >= 0) && vpp) {
	                    cchar	*cp{} ;
	                    if (int cl ; (cl = sfshrink(sp,tl,&cp)) >= 0) {
	                        rs = si.strw(cp,cl,vpp) ;
	                    }
	                }
	                sl -= intconv((tp + 1) - sp) ;
	                sp = (tp + 1) ;
	                if (rs < 0) break ;
	            } /* end for */
	            if ((rs >= 0) && (fi == 4) && sl && sp[0]) {
	                rs = userattrent_parseattr(this,&si,sp,sl) ;
	            }
	            rs1 = si.finish ;
	            if (rs >= 0) rs = rs1 ;
		    wlen = rs1 ;
	        } /* end if (storeitem) */
	    } /* end if (non-zero non-comment) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (ucentua::parse) */

int ucentua::load(char *uabuf,int ualen,CUA *suap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (this && uabuf && suap) {
	    USERATTR *uep = this ;
	    *uep = *suap ; /* shallow copy */
	    if (storeitem si ; (rs = si.start(uabuf,ualen)) >= 0) {
	        if (suap->attr) {
	            cint	ksz = szof(kva_t) ;
	            cint	al = szof(void *) ;
	            cint	n = suap->attr->length ;
	            void	*p{} ;
	            if ((rs = si.block(ksz,al,&p)) >= 0) {
	                kva_t	*kvap = (kva_t *) p ;
	                int	dsize = (n * szof(kv_t)) ;
	                attr = kvap ;
	                if ((rs = si.block(dsize,al,&p)) >= 0) {
	                    kv_t	*kvp = (kv_t *) p ;
	                    attr->length = n ;
	                    attr->data = kvp ;
	                    for (int i = 0 ; i < n ; i += 1) {
	                        cchar	*dp = suap->attr->data[i].key ;
	                        char	*rp ;
	                        if ((rs = si_copystr(&si,&rp,dp)) >= 0) {
	                            kvp[i].key = rp ;
	                            dp = suap->attr->data[i].value ;
	                            if ((rs = si_copystr(&si,&rp,dp)) >= 0) {
	                                kvp[i].value = rp ;
	                            }
	                        }
	                        if (rs < 0) break ;
	                    } /* end for */
	                } /* end if (storeitem_block) */
	            } /* end if (storeitem_block) */
	        } /* end if (attr) */
	        if (rs >= 0) {
	            char *rp{} ;
		    if ((rs = si_copystr(&si,&rp,suap->name)) >= 0) {
	                name = rp ;
		    }
	        }
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
		wlen = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (userattrent_load) */

int ucentua::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 5 ; i += 1) {
	            if (i > 0) rs = b.chr(':') ;
	            if (rs >= 0) {
	                switch (i) {
	                case 0:
	                    rs = b.str(name) ;
	                    break ;
	                case 1:
	                    if (qualifier) {
	                        rs = b.str(qualifier) ;
	                    }
	                    break ;
	                case 2:
	                    if (res1) {
	                        rs = b.str(res1) ;
	                    }
	                    break ;
	                case 3:
	                    if (res2) {
	                        rs = b.strw(res2) ;
	                    }
	                    break ;
	                case 4:
	                    if (attr) {
	                        rs = sbuf_fmtattrs(&b,attr) ;
	                    }
	                    break ;
	                } /* end switch */
	            } /* end if */
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (userattrent_format) */

int ucentua::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		sz = 1 ;
	    kva_t	*kvap = attr ;
	    if (name) {
	        sz += (lenstr(name)+1) ;
	    }
	    if (attr) {
	        kv_t	*kvp = kvap->data ;
	        cint	n = kvap->length ;
	        sz += szof(kva_t) ;
	        for (int i = 0 ; i < n ; i += 1) {
	            sz += (lenstr(kvp[i].key)+1) ;
	            sz += (lenstr(kvp[i].value)+1) ;
	        } /* end for */
	        sz += ((n+1)*szof(kv_t)) ;
	    } /* end if */
	    rs = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (userattrent_size) */

int ucentua::getent(char *uabuf,int ualen) noex {
	return uc_getuaent(this,uabuf,ualen) ;
}

int ucentua::getnam(char *uabuf,int ualen,cchar *uaname) noex {
	return uc_getuanam(this,uabuf,ualen,uaname) ;
}

int ucentua::getuid(char *uabuf,int ualen,uid_t uid) noex {
	return uc_getuauid(this,uabuf,ualen,uid) ;
}


/* local subroutines */

static int userattrent_parseattr(UA *uap,SI *sip,cc *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (vecstr a ; (rs = a.start(0,0)) >= 0) {
	    cint	sch = ';' ;
	    cchar	*tp ;
	    while ((tp = strnchr(sp,sl,sch)) != nullptr) {
	        if (cint tl = intconv(tp - sp) ; tl > 0) {
		    c += 1 ;
	            rs = a.add(sp,tl) ;
	        }
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (sl > 0)) {
		c += 1 ;
	        rs = a.add(sp,sl) ;
	    }
	    if ((rs >= 0) && ((rs = a.count) > 0)) {
	        rs = userattrent_parseattrload(uap,sip,&a,rs) ;
	    }
	    rs1 = a.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return rs ;
}
/* end subroutine (userattrent_parseattr) */

static int userattrent_parseattrload(UA *uap,SI *sip,vecstr *alp,int n) noex {
	cint		ksz = szof(kva_t) ;
	cint		al = szof(void *) ;
	int		rs ;
	if (void *p ; (rs = sip->block(ksz,al,&p)) >= 0) {
	    kva_t	*kvap = (kva_t *) p ;
	    int		dsize = (n*szof(kv_t)) ;
	    uap->attr = kvap ;
	    if ((rs = sip->block(dsize,al,&p)) >= 0) {
	        kv_t	*kvp = (kv_t *) p ;
	        cchar	*ep{} ;
	        uap->attr->length = n ;
	        uap->attr->data = kvp ;
	        for (int i = 0 ; alp->get(i,&ep) >= 0 ; i += 1) {
	            if (ep) {
	                rs = si_attrload(sip,kvp,i,ep) ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (storeitem_block) */
	} /* end if (storeitem_block) */
	return rs ;
}
/* end subroutine (userattrent_parseattrload) */

static int si_attrload(SI *sip,kv_t *kvp,int i,cchar *ep) noex {
	int		rs ;
	int		el = -1 ;
	cchar		*vp ;
	if (cchar *tp ; (tp = strchr(ep,'=')) != nullptr) {
	    vp = (tp + 1) ;
	    el = intconv(tp - ep) ;
	} else {
	    vp = (ep + lenstr(ep)) ;
	}
	{
	    if (cchar *crp{} ; (rs = sip->strw(ep,el,&crp)) >= 0) {
		char	*rp = cast_const<charp>(crp) ;
	        kvp[i].key = rp ;
	        if ((rs = si_copystr(sip,&rp,vp)) >= 0) {
	            kvp[i].value = rp ;
	        }
	    }
	}
	return rs ;
}
/* end subroutine (si_attrload) */

static int sbuf_fmtattrs(sbuf *sbp,kva_t *attr) noex {
	int		rs = SR_FAULT ;
	if (sbp && attr) {
	    kv_t	*kv = attr->data ;
	    cint	n = attr->length ;
	    cint	sch = ';' ;
	    rs = SR_OK ;
	    for (int i = 0 ; i < n ; i += 1) {
	        if (kv[i].key != nullptr) {
	            if (i > 0) rs = sbp->chr(sch) ;
	            if (rs >= 0) rs = sbp->str(kv[i].key) ;
	            if ((rs >= 0) && (kv[i].value != nullptr)) {
	                rs = sbp->chr('=') ;
	                if (rs >= 0) rs = sbp->str(kv[i].value) ;
	            }
	        } /* end if */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (non-null attr) */
	return rs ;
}
/* end subroutine (sbuf_fmtattrs) */


