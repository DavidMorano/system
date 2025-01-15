/* ucentsv SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UCENTSV object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_entsv{x}

	Description:
	I provide the normal (usual) subroutines for managing the
	UCENTSV (UNIX® |SERVENT| database entries) object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<intceil.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"ucentsv.h"
#include	"ucentxx.hh"


/* local defines */

#define	SVE	ucentsv
#define	CSVE	const ucentsv
#define	SI	storeitem


/* imported namespaces */

using ucent::si_loadnames ;		/* local group support subroutine */
using ucent::si_loadname ;		/* local group support subroutine */
using ucent::si_copystr ;		/* local group support subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentsv_parsenum(SVE *,SI *,cchar *,int) noex ;
static int ucentsv_parsestrs(SVE *,SI *,cchar *,int) noex ;

static int ucentsv_formatnum(SVE *,sbuf *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentsv::parse(char *ebuf,int elen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && ebuf && sp) {
	    SERVENT *sep = this ;
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(sep) ;
	    if (storeitem si ; (rs = si.start(ebuf,elen)) >= 0) {
	        cchar	*cp{} ;
		if (int idx ; (idx = sichr(sp,sl,'#')) >= 0) {
		    sl = idx ;
		}
	        for (int cl, fi = 0 ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = ccharpp(&s_name) ;
	                break ;
	            case 1:
	                rs = ucentsv_parsenum(this,&si,cp,cl) ;
	                break ;
	            case 2:
	                rs = ucentsv_parsestrs(this,&si,cp,cl) ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	        	rs = si.strw(cp,cl,vpp) ;
		    }
	            sl -= ((cp+cl)-sp) ;
	            sp = (cp+cl) ;
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsv::parse) */

int ucentsv::load(char *rbuf,int rlen,const ucentsv *csvp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf && csvp) {
	    SERVENT	*sep = this ;
	    *sep = *csvp ; /* shallow copy */
	    if (storeitem si ; (rs = si.start(rbuf,rlen)) >= 0) {
	        if (csvp->s_aliases) {
	            int	n ; /* used-afterwards */
	            for (n = 0 ; csvp->s_aliases[n] ; n += 1) ;
	            if (void **tab{} ; (rs = si.ptab(n,&tab)) >= 0) {
		        cchar	**aliases = ccharpp(csvp->s_aliases) ;
		        int	i ; /* used-afterwards */
	                s_aliases = charpp(tab) ;
	                for (i = 0 ; csvp->s_aliases[i] ; i += 1) {
			    cchar	*an = aliases[i] ;
	                    rs = si_copystr(&si,(s_aliases + i),an) ;
	                    if (rs < 0) break ;
	                } /* end for */
	                s_aliases[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
		    s_aliases = nullptr ;
	        } /* end if (aliases) */
		if (rs >= 0) {
		    rs = si_copystr(&si,&s_name,csvp->s_name) ;
		}
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsv::load) */

int ucentsv::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 3 ; i += 1) {
	            if (i > 0) rs = b.chr(' ') ;
	            if (rs >= 0) {
	                switch (i) {
	                case 0:
	                    rs = b.str(s_name) ;
	                    break ;
	                case 1:
	                    rs = ucentsv_formatnum(this,&b) ;
	                    break ;
	                case 2:
	    		    if (s_aliases) {
				cchar	**sv = ccharpp(s_aliases) ;
	                        rs = b.strs(' ',sv) ;
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
/* end subroutine (ucentsv::format) */

int ucentsv::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		sz = 1 ;
	    if (s_name) {
	        sz += (strlen(s_name)+1) ;
	    }
	    if (s_aliases) {
	        int	i ; /* used-afterwards */
	        for (i = 0 ; s_aliases[i] ; i += 1) {
	            sz += (strlen(s_aliases[i])+1) ;
	        } /* end for */
	        sz += ((i+1)*szof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsv::size) */

int ucentsv::getent(char *rb,int rl) noex {
    	return uc_getsvent(this,rb,rl) ;
}

int ucentsv::getnam(char *rb,int rl,cc *n,cc *p) noex {
    	return uc_getsvnam(this,rb,rl,n,p) ;
}

int ucentsv::getnum(char *rb,int rl,int n,cchar *p) noex {
    	return uc_getsvnum(this,rb,rl,n,p) ;
}


/* local subroutines */

static int ucentsv_parsenum(SVE *op,SI *sbp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (cchar *tp ; (tp = strnchr(sp,sl,'/')) != nullptr) {
	    if (int v{} ; (rs = cfdeci(sp,(tp-sp),&v)) >= 0) {
		cint	cl = ((sp+sl)-(tp+1))  ;
		cchar	*cp = (tp+1) ;
		op->s_port = v ;
		if (cchar *rp{} ; (rs = sbp->strw(cp,cl,&rp)) >= 0) {
		    op->s_proto = charp(rp) ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (ucentsv_parsenum) */

static int ucentsv_parsestrs(ucentsv *prp,SI *sip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = u.start(8,0)) >= 0) {
	    if ((rs = si_loadnames(sip,&u,sp,sl)) > 0) {
	        cint	n = rs ;
	        if (void **ptab{} ; (rs = sip->ptab(n,&ptab)) >= 0) {
		    int		i ; /* used-afterwards */
	            void	*vp{} ;
	            prp->s_aliases = charpp(ptab) ;
		    for (i = 0 ; u.get(i,&vp) >= 0 ; i += 1) {
	                prp->s_aliases[i] = charp(vp) ;
	            } /* end for */
	            prp->s_aliases[i] = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        prp->s_aliases = nullptr ;
	    }
	    rs1 = u.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
}
/* end subroutine (ucentsv_parsestrs) */

static int ucentsv_formatnum(SVE *op,sbuf *sbp) noex {
	int		rs ;
	if ((rs = sbp->dec(op->s_port)) >= 0) {
	    if ((rs = sbp->chr('/')) >= 0) {
		rs = sbp->strw(op->s_proto,-1) ;
	    }
	}
	return rs ;
}
/* end subroutine (ucentsv_formatnum) */


