/* ucentsp SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UCENTSP object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_entsp{x}

	Description:
	These subroutines perform some SHADOW-structure management
	functions.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
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

#include	"ucentsp.h"
#include	"ucentxx.hh"


/* local defines */

#define	SI	storeitem


/* imported namespaces */

using ucent::si_copystr ;		/* local group support subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentsp_parseone(ucentsp *,SI *,int,cchar *,int) noex ;
static int ucentsp_parsedefs(ucentsp *,SI *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentsp::parse(char *spbuf,int splen,cchar *sp,int sl) noex {
	int		rs  = SR_FAULT ;
	int		rs1 ;
	if (spbuf && sp) {
	    SPWD *sep = this ;
	    memclear(sep) ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (storeitem si ; (rs = si.start(spbuf,splen)) >= 0) {
	        int		fi = 0 ;
	        for (int idx ; (idx = sichr(sp,sl,':')) >= 0 ; ) {
	            rs = ucentsp_parseone(this,&si,fi++,sp,idx) ;
	            sl -= (idx +1) ;
	            sp += (idx +1) ;
	            if (rs < 0) break ;
	        } /* end for */
	        if ((rs >= 0) && sl && sp[0]) {
	            rs = ucentsp_parseone(this,&si,fi++,sp,sl) ;
	        }
	        if (rs >= 0) {
	            rs = ucentsp_parsedefs(this,&si,fi) ;
	            fi = rs ;
	        }
	        if ((rs >= 0) && (fi < 6)) rs = SR_BADFMT ;
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsp::parse) */

int ucentsp::load(char *spbuf,int splen,const ucentsp *sspp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (spbuf && sspp) {
	    SPWD *sep = this ;
	    *sep = *sspp ;
	    if (storeitem si ; (rs = si.start(spbuf,splen)) >= 0) {
		{
	            si_copystr(&si,&sp_namp,sspp->sp_namp) ;
	            si_copystr(&si,&sp_pwdp,sspp->sp_pwdp) ;
		}
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsp::load) */

int ucentsp::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 9 ; i += 1) {
	            if (i > 0) rs = b.chr(':') ;
	            if (rs >= 0) {
	                long	v  = -1 ;
	                switch (i) {
	                case 0:
	                    rs = b.str(sp_namp) ;
	                    break ;
	                case 1:
	                    rs = b.str(sp_pwdp) ;
	                    break ;
	                case 2:
	                case 3:
	                case 4:
	                case 5:
	                case 6:
	                case 7:
	                    switch (i) {
	                    case 2:
	                        v = sp_lstchg ;
	                        break ;
	                    case 3:
	                        v = sp_min ;
	                        break ;
	                    case 4:
	                        v = sp_max ;
	                        break ;
	                    case 5:
	                        v = sp_warn ;
	                        break ;
	                    case 6:
	                        v = sp_inact ;
	                        break ;
	                    case 7:
	                        v = sp_expire ;
	                        break ;
	                    } /* end switch */
	                    if (v != -1) {
	                        rs = b.dec(v) ;
	                    }
	                    break ;
	                case 8:
	                    if (ulong uv = sp_flag ; uv != 0) {
	                        rs = b.dec(uv) ;
	                    }
	                    break ;
	                } /* end switch */
	            } /* end if (ok) */
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsp::format) */

int ucentsp::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		sz = 1 ;
	    if (sp_namp) {
	        sz += (strlen(sp_namp)+1) ;
	    }
	    if (sp_pwdp) {
	        sz += (strlen(sp_pwdp)+1) ;
	    }
	    rs = sz ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsp::size) */

int ucentsp::getent(char *spbuf,int splen) noex {
	return uc_getspent(this,spbuf,splen) ;
}

int ucentsp::getnam(char *spbuf,int splen,cchar *name) noex {
	return uc_getspnam(this,spbuf,splen,name) ;
}


/* local subroutines */

static int ucentsp_parseone(ucentsp *spp,SI *sip,int fi,cc *vp,int vl) noex {
	int		rs = SR_OK ;
	long		v = -1 ;
	cchar		**vpp = nullptr ;
	switch (fi) {
	case 0:
	    vpp = ccharpp(&spp->sp_namp) ;
	    break ;
	case 1:
	    vpp = ccharpp(&spp->sp_pwdp) ;
	    break ;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	    if ((vl > 0) && ((rs = cfdecl(vp,vl,&v)) >= 0)) {
	        switch (fi) {
	        case 2:
	            spp->sp_lstchg = v ;
	            break ;
	        case 3:
	            spp->sp_min = v ;
	            break ;
	        case 4:
	            spp->sp_max = v ;
	            break ;
	        case 5:
	            spp->sp_warn = v ;
	            break ;
	        case 6:
	            spp->sp_inact = v ;
	            break ;
	        case 7:
	            spp->sp_expire = v ;
	            break ;
	        } /* end switch */
	    } /* end if (cfdecl) */
	    break ;
	case 8:
	    if (vl > 0) {
	        if (ulong uv{} ; (rs = cfdecul(vp,vl,&uv)) >= 0) {
	            spp->sp_flag = uv ;
		}
	    }
	    break ;
	} /* end switch */
	if ((rs >= 0) && vpp) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(vp,vl,&cp)) >= 0) {
	        rs = sip->strw(cp,cl,vpp) ;
	    }
	}
	return rs ;
}
/* end subroutine (ucentsp_parseone) */

static int ucentsp_parsedefs(ucentsp *spp,SI *sip,int sfi) noex {
	int		rs = SR_OK ;
	if (sfi == 1) {
	    cchar	**vpp = ccharpp(&spp->sp_pwdp) ;
	    cchar	*sp = spp->sp_namp ;
	    cchar	*vp ;
	    vp = (sp + strlen(sp)) ;
	    sfi += 1 ;
	    rs = sip->strw(vp,0,vpp) ;
	}
	return (rs >= 0) ? sfi : rs ;
}
/* end subroutine (ucentsp_parsedefs) */


