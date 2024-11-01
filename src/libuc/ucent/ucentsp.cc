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
	ucenumsp

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


/* local defines */

#define	SI	storeitem


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentsp_parseone(ucentsp *,SI *,int,cchar *,int) noex ;
static int ucentsp_parsedefs(ucentsp *,SI *,int) noex ;

static int si_copystr(SI *,char **,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentsp::parse(char *spbuf,int splen,cchar *sp,int sl) noex {
	int		rs  = SR_FAULT ;
	int		rs1 ;
	if (spbuf && sp) {
	    rs = SR_INVALID ;
	    memclear(this) ;		/* potentially dangerous */
	    if (splen > 0) {
	        storeitem	ib, *ibp = &ib ;
	        if (sl < 0) sl = strlen(sp) ;
	        if ((rs = storeitem_start(ibp,spbuf,splen)) >= 0) {
	            int		fi = 0 ;
	            int		si ;
	            while ((si = sichr(sp,sl,':')) >= 0) {
	                rs = ucentsp_parseone(this,ibp,fi++,sp,si) ;
	                sl -= (si+1) ;
	                sp += (si+1) ;
	                if (rs < 0) break ;
	            } /* end while */
	            if ((rs >= 0) && sl && sp[0]) {
	                rs = ucentsp_parseone(this,ibp,fi++,sp,sl) ;
	            }
	            if (rs >= 0) {
	                rs = ucentsp_parsedefs(this,ibp,fi) ;
	                fi = rs ;
	            }
	            if ((rs >= 0) && (fi < 6)) rs = SR_BADFMT ;
	            rs1 = storeitem_finish(ibp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (storeitem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsp::parse) */

int ucentsp::load(char *spbuf,int splen,const ucentsp *sspp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (spbuf && sspp) {
	    rs = SR_INVALID ;
	    if (splen > 0) {
	        storeitem	ib ;
	        (*this) = *sspp ;
	        if ((rs = storeitem_start(&ib,spbuf,splen)) >= 0) {
	            si_copystr(&ib,&sp_namp,sspp->sp_namp) ;
	            si_copystr(&ib,&sp_pwdp,sspp->sp_pwdp) ;
	            rs1 = storeitem_finish(&ib) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (storeitem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsp::load) */

int ucentsp::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen > 0) {
	        sbuf	b ;
	        if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	            for (int i = 0 ; i < 9 ; i += 1) {
	                if (i > 0) rs = sbuf_chr(&b,':') ;
	                if (rs >= 0) {
	                    long	v  = -1 ;
	                    switch (i) {
	                    case 0:
	                        rs = sbuf_strw(&b,sp_namp,-1) ;
	                        break ;
	                    case 1:
	                        rs = sbuf_strw(&b,sp_pwdp,-1) ;
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
	                            rs = sbuf_decl(&b,v) ;
	                        }
	                        break ;
	                    case 8:
	                        {
	                            ulong	uv = sp_flag ;
	                            if (uv != 0) {
	                                rs = sbuf_decul(&b,uv) ;
	                            }
	                        }
	                        break ;
	                    } /* end switch */
	                } /* end if (ok) */
	                if (rs < 0) break ;
	            } /* end for */
	            rs1 = sbuf_finish(&b) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsp::format) */

int ucentsp::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		size = 1 ;
	    if (sp_namp) {
	        size += (strlen(sp_namp)+1) ;
	    }
	    if (sp_pwdp) {
	        size += (strlen(sp_pwdp)+1) ;
	    }
	    rs = size ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentsp::size) */


/* local subroutines */

static int ucentsp_parseone(ucentsp *spp,SI *ibp,int fi,cc *vp,int vl) noex {
	int		rs = SR_OK ;
	long		v = -1 ;
	cchar		**vpp = nullptr ;
	switch (fi) {
	case 0:
	    vpp = (cchar **) &spp->sp_namp ;
	    break ;
	case 1:
	    vpp = (cchar **) &spp->sp_pwdp ;
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
	        ulong	uv{} ;
	        rs = cfdecul(vp,vl,&uv) ;
	        spp->sp_flag = uv ;
	    }
	    break ;
	} /* end switch */
	if ((rs >= 0) && vpp) {
	    int		cl ;
	    cchar	*cp{} ;
	    if ((cl = sfshrink(vp,vl,&cp)) >= 0) {
	        rs = storeitem_strw(ibp,cp,cl,vpp) ;
	    }
	}
	return rs ;
}
/* end subroutine (ucentsp_parseone) */

static int ucentsp_parsedefs(ucentsp *spp,SI *ibp,int sfi) noex {
	int		rs = SR_OK ;
	if (sfi == 1) {
	    cchar	**vpp = (cchar **) &spp->sp_pwdp ;
	    cchar	*sp = spp->sp_namp ;
	    cchar	*vp ;
	    vp = (sp + strlen(sp)) ;
	    sfi += 1 ;
	    rs = storeitem_strw(ibp,vp,0,vpp) ;
	}
	return (rs >= 0) ? sfi : rs ;
}
/* end subroutine (ucentsp_parsedefs) */

static int si_copystr(SI *ibp,char **pp,cchar *p1) noex {
	int		rs = SR_OK ;
	cchar		**cpp = (cchar **) pp ;
	*cpp = nullptr ;
	if (p1) {
	    rs = storeitem_strw(ibp,p1,-1,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copystr) */


