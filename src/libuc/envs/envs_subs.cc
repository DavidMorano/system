/* envs_subs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process the cookie substitutions for environment variables */
/* version %I% last-modified %G% */

#define	CF_EXPAND	0		/* perform expand-cookie */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	envs_subs

	Description:
	This subroutine takes raw strings that are supposed to be
	environment variables and processes any cookie substitutions
	that are found in those strings. It also cleans up those
	environment variables that are supposed to be directory
	paths of some kind.

	Synopsis:
	int envs_subs(envs *nlp,expcook *clp,vecstr *pvp,vecstr *evp) noex

	Arguments:
	nlp		new-list-pointer, new (forming) environment list
	clp		cookies list pointer
	pvp		pointer to VECSTR of path-vars
	evp		pointer to VECSTR of exportable environment

	Returns:
	>=0		count of environment variables
	<0		bad (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<buffer.h>
#include	<sbuf.h>
#include	<expcook.h>
#include	<strn.h>
#include	<pathclean.h>
#include	<localmisc.h>

#include	"envs.h"


/* local defines */

#define	EBUFMULT	3		/* env-var buf-len-multiplier */

#ifndef	DEFNPATHS
#define	DEFNPATHS	20
#endif

#ifndef	CF_EXPAND
#define	CF_EXPAND	0		/* perform expand-cookie */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct subinfo {
	envs		*nlp ;
	expcook		*clp ;
	vecstr		*pvp ;
	vecstr		*evp ;
	int procvar(cchar *,int) noex ;
	int procvarnorm(cchar *,int) noex ;
	int procvarpath(cchar *,int) noex ;
	int procpathsplit(vecstr *,cchar *,int) noex ;
	int procsub(buffer *,cchar *,int) noex ;
    } ; /* end struct (subinfo) */
    struct vars {
	int		maxpathlen ;
	int		elen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

static int	vecstr_pathjoin(vecstr *,char *,int) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;

static vars		var ;

constexpr bool		f_expand = CF_EXPAND ;


/* exported variables */


/* exported subroutines */

int envs_subs(envs *nlp,expcook *clp,vecstr *pvp,vecstr *evp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (nlp && clp && pvp && evp) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        subinfo		si ;
	        si.evp = evp ;
	        si.clp = clp ;
	        si.pvp = pvp ;
	        si.nlp = nlp ;
	        if (envs_cur cur ; (rs = envs_curbegin(nlp,&cur)) >= 0) {
	            int		kl ;
	            cchar	*kp{} ;
	            while (rs >= 0) {
	                kl = envs_curenumkey(nlp,&cur,&kp) ;
	                if (kl == SR_NOTFOUND) break ;
	                rs = kl ;
	                if (rs >= 0) {
	                    c += 1 ;
	                    rs = si.procvar(kp,kl) ;
	                }
	            } /* end while */
	            rs1 = envs_curend(nlp,&cur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	    } /* end if (vars) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (envs_subs) */


/* local subroutines */

int subinfo::procvar(cchar *kp,int kl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((rs1 = vecstr_findn(pvp,kp,kl)) >= 0) {
	    rs = procvarpath(kp,kl) ;
	} else if (rs1 == SR_NOTFOUND) {
	    rs = procvarnorm(kp,kl) ;
	} else {
	    rs = rs1 ;
	}
	return rs ;
}
/* end method (subinfo::procvar) */

int subinfo::procvarnorm(cchar *kp,int kl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (buffer b ; (rs = b.start(var.elen)) >= 0) {
	    if (envs_cur cur ; (rs = envs_curbegin(nlp,&cur)) >= 0) {
	        int	vl ;
	        cchar	*vp ;
	        while (rs >= 0) {
	            vl = envs_fetch(nlp,kp,kl,&cur,&vp) ;
	            if (vl == SR_NOTFOUND) break ;
	            rs = vl ;
	            if ((rs >= 0) && (vl > 0)) {
	                if (rs >= 0) {
	                    rs = procsub(&b,vp,vl) ;
			}
	            } /* end if */
	        } /* end while */
	        rs1 = envs_curend(nlp,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (envs-cursor) */
	    if (rs >= 0) {
	        if (cchar *bp{} ; (rs = b.get(&bp)) >= 0) {
	            rs = vecstr_envadd(evp,kp,bp,rs) ;
		}
	    } /* end if (ok) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (buffer) */
	return (rs >= 0) ? c : rs ;
}
/* end method (subinfo::procvarnorm) */

int subinfo::procvarpath(cchar *kp,int kl) noex {
    	cint		vn = DEFNPATHS ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (vecstr paths ; (rs = paths.start(vn,vo)) >= 0) {
	    cint	bslen = var.maxpathlen ;
	    if (buffer b ; (rs = b.start(bslen)) >= 0) {
		if (envs_cur cur ; (rs = envs_curbegin(nlp,&cur)) >= 0) {
		    int		vl, bl ;
		    cchar	*vp, *bp ;
	            while (rs >= 0) {
	                vl = envs_fetch(nlp,kp,kl,&cur,&vp) ;
	                if (vl == SR_NOTFOUND) break ;
	                rs = vl ;
	                if ((rs >= 0) && (vp[0] != ':')) {
	                    b.reset() ;
	                    if ((rs = procsub(&b,vp,vl)) >= 0) {
	                        if ((bl = b.get(&bp)) >= 0) {
	                            rs = procpathsplit(&paths,bp,bl) ;
				}
	                    } /* end if */
	                } /* end if */
	            } /* end while */
	            rs1 = envs_curend(nlp,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	        if (rs >= 0) {
	            cint	psz = paths.strsize ;
		    if ((rs = paths.count) >= 0) {
			cint	npaths = rs ;
	                cint	bufl = (psz + npaths + 1) ;
	                if (char *bufp{} ; (rs = uc_malloc(bufl,&bufp)) >= 0) {
			    cchar	*sp = bufp ;
			    int	sl = 0 ;
	                    if ((rs = vecstr_pathjoin(&paths,bufp,bufl)) > 0) {
			        sl = rs ;
			        while (sl && (sp[0] == ':')) {
				    sp += 1 ;
				    sl -= 1 ;
			        } /* end while */
			        len = sl ;
	                        rs = vecstr_envadd(evp,kp,sp,sl) ;
			    } /* end if (vecstr_pathjoin) */
	                    rs1 = uc_free(bufp) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (allocation) */
		    } /* end if (vecstr_count) */
	        } /* end if (ok) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (buffer) */
	    rs1 = vecstr_finish(&paths) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr-paths) */
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::procvarpath) */

int subinfo::procpathsplit(vecstr *plp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (plp) {
	    if (char *pbuf{} ; (rs = malloc_mp(&pbuf)) >= 0) {
	        int	pl, cl ;
	        cchar	*tp, *cp ;
	        while ((tp = strnbrk(vp,vl,":;")) != nullptr) {
	            cp = vp ;
	            cl = intconv(tp - vp) ;
	            c += 1 ;
	            if ((pl = pathclean(pbuf,cp,cl)) >= 0) {
	                rs = plp->adduniq(pbuf,pl) ;
		    }
	            if ((rs >= 0) && (tp[0] == ';')) {
	                rs = plp->adduniq(";",1) ;
	            }
	            vl -= intconv((tp + 1) - vp) ;
	            vp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end while */
	        if ((rs >= 0) && ((c == 0) || (vl > 0))) {
		    cint	tl = intconv(tp - vp) ;
	            c += 1 ;
	            if ((pl = pathclean(pbuf,vp,tl)) >= 0) {
	                rs = plp->adduniq(pbuf,pl) ;
	            }
	        } /* end if */
		rs1 = uc_free(pbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end method (subinfo::procpathsplit) */

int subinfo::procsub(buffer *bp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (bp) {
	    if_constexpr (f_expand) {
	        int	rs1 = SR_NOTFOUND ;
	        int	elen = max(var.elen,vl) ;
	        char	*ebuf{} ;
	        char	*p{} ;
	        for (int i = 0 ; (rs = uc_malloc(elen,&p)) >= 0 ; i += 1) {
	            ebuf = p ;
	            rs1 = expcook_exp(clp,0,ebuf,elen,vp,vl) ;
	            if (rs1 >= 0) break ;
	            elen += var.elen ;
	            uc_free(ebuf) ;
	            ebuf = nullptr ;
	        } /* end while */
	        if ((rs >= 0) && (rs1 > 0) && ebuf) {
	            rs = bp->strw(ebuf,rs1) ;
	        }
	        if (ebuf) {
	            uc_free(ebuf) ;
	        }
	    } else {
	        rs = bp->strw(vp,vl) ;
	    } /* end if_constexpr (f_expand) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (subinfo::procsub) */

static int vecstr_pathjoin(vecstr *plp,char jbuf[],int jlen) noex {
	int		rs ;
	int		bl = 0 ;
	if (sbuf b ; (rs = b.start(jbuf,jlen)) >= 0) {
	    int		c = 0 ;
	    cchar	*cp ;
	    bool	f_semi = false ;
	    for (int i = 0 ; plp->get(i,&cp) >= 0 ; i += 1) {
	        if (cp) {
	            if (cp[0] != ';') {
	                if (c++ > 0) {
			    int sc{} ;
	    		    if (f_semi) {
	                        f_semi = false ;
	                        sc = ';' ;
	                    } else {
	                        sc = ':' ;
			    }
	                    rs = b.chr(sc) ;
	                }
	                if (rs >= 0) {
	                    rs = b.str(cp) ;
		        }
	            } else {
	                f_semi = true ;
		    }
		} /* end if (non-null) */
	        if (rs < 0) break ;
	    } /* end for */
	    bl = b.finish ;
	    if (rs >= 0) rs = bl ;
	} /* end if (sbuf) */

	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (vecstr_pathjoin) */

vars::operator int () noex {
	int		rs ;
	if ((rs = maxpathlen) >= 0) {
	    maxpathlen = rs ;
	    elen = (rs * EBUFMULT) ;
	}
	return rs ;
}
/* end method (vars::operator) */

