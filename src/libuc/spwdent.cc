/* spwdent SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* SPWD structure management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	spwdent

	Description:
	These subroutines perform some SHADOW-structure management
	functions.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<cfdec.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"spwd.h"
#include	"spwdent.h"

import libutil ;

/* local defines */

#define	SPE	SPWDENT
#define	CSPE	CSPWDENT
#define	SI	storeitem


/* external subroutines */

extern "C" {
    extern int	sfshrink(cchar *,int,cchar **) noex ;
    extern int	sichr(cchar *,int,int) noex ;
    extern char	*strnchr(cchar *,int,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int spwdent_parseone(SPWD *,SI *,int,cchar *,int) noex ;
static int spwdent_parsedefs(SPWD *,SI *,int) noex ;
static int si_copystr(SI *,char **,cchar *) noex ;


/* local variables */


/* exported subroutines */

int spwdent_parse(SPWD *spp,char *spbuf,int splen,cchar *sp,int sl) noex {
	int		rs  = SR_FAULT ;
	int		rs1 ;
	if (spp && spbuf && sp) {
	    storeitem	ib, *ibp = &ib ;
	    if (sl < 0) sl = lenstr(sp) ;
	    memclear(spp) ;
	    if ((rs = storeitem_start(ibp,spbuf,splen)) >= 0) {
	        int		fi = 0 ;
	        int		si ;
	        while ((si = sichr(sp,sl,':')) >= 0) {
	            rs = spwdent_parseone(spp,ibp,fi++,sp,si) ;
	            sl -= (si+1) ;
	            sp += (si+1) ;
	            if (rs < 0) break ;
	        } /* end while */
	        if ((rs >= 0) && sl && sp[0]) {
	            rs = spwdent_parseone(spp,ibp,fi++,sp,sl) ;
	        }
	        if (rs >= 0) {
	            rs = spwdent_parsedefs(spp,ibp,fi) ;
	            fi = rs ;
	        }
	        if ((rs >= 0) && (fi < 6)) rs = SR_BADFMT ;
	        rs1 = storeitem_finish(ibp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (spwdent_parse) */

int spwdent_load(SPWD *spp,char *spbuf,int splen,CSPWD *sspp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (spp && spbuf && sspp) {
	    memcopy(spp,sspp) ;
	    storeitem	ib ;
	    if ((rs = storeitem_start(&ib,spbuf,splen)) >= 0) {
	        si_copystr(&ib,&spp->sp_namp,sspp->sp_namp) ;
	        si_copystr(&ib,&spp->sp_pwdp,sspp->sp_pwdp) ;
	        rs1 = storeitem_finish(&ib) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (spwdent_load) */

int spwdent_format(CSPWD *spp,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (spp && rbuf) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 9 ; i += 1) {
	            long	v  = 0 ;
	            if (i > 0) rs = b.chr(':') ;
	            if (rs >= 0) {
	                switch (i) {
	                case 0:
	                    rs = b.strw(spp->sp_namp,-1) ;
	                    break ;
	                case 1:
	                    rs = b.strw(spp->sp_pwdp,-1) ;
	                    break ;
	                case 2:
	                case 3:
	                case 4:
	                case 5:
	                case 6:
	                case 7:
	                    switch (i) {
	                    case 2:
	                        v = spp->sp_lstchg ;
	                        break ;
	                    case 3:
	                        v = spp->sp_min ;
	                        break ;
	                    case 4:
	                        v = spp->sp_max ;
	                        break ;
	                    case 5:
	                        v = spp->sp_warn ;
	                        break ;
	                    case 6:
	                        v = spp->sp_inact ;
	                        break ;
	                    case 7:
	                        v = spp->sp_expire ;
	                        break ;
	                    } /* end switch */
	                    if (v != -1) {
	                        rs = b.dec(v) ;
	                    }
	                    break ;
	                case 8:
	                    if (ulong uv = spp->sp_flag ; uv != 0) {
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
/* end subroutine (spwdent_format) */

int spwdent_size(CSPWD *pp) noex {
	int		sz = 1 ;
	if (pp->sp_namp) {
	    sz += (lenstr(pp->sp_namp) + 1) ;
	}
	if (pp->sp_pwdp) {
	    sz += (lenstr(pp->sp_pwdp) + 1) ;
	}
	return sz ;
}
/* end subroutine (spwdent_size) */


/* local subroutines */

static int spwdent_parseone(SPWD *spp,SI *ibp,int fi,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	long		v  = 0 ;
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
	        if (ulong uv ; (rs = cfdecul(vp,vl,&uv)) >= 0) {
	            spp->sp_flag = uv ;
		} else if (isNotValid(rs)) {
		    rs = SR_OK ;
		}
	    }
	    break ;
	} /* end switch */
	if ((rs >= 0) && (vpp != nullptr)) {
	    cchar	*cp ;
	    if (int cl ; (cl = sfshrink(vp,vl,&cp)) >= 0) {
	        rs = storeitem_strw(ibp,cp,cl,vpp) ;
	    }
	}
	return rs ;
}
/* end subroutine (spwdent_parseone) */

static int spwdent_parsedefs(SPWD *spp,SI *ibp,int sfi) noex {
	int		rs = SR_OK ;
	if (sfi == 1) {
	    cchar	**vpp = (cchar **) &spp->sp_pwdp ;
	    cchar	*sp = spp->sp_namp ;
	    cchar	*valp ;
	    valp = (sp + lenstr(sp)) ;
	    sfi += 1 ;
	    rs = storeitem_strw(ibp,valp,0,vpp) ;
	}
	return (rs >= 0) ? sfi : rs ;
}
/* end subroutine (spwdent_parsedefs) */

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


