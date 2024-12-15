/* pwentry SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* some miscellaneous PWENTRY subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A-D- Morano
	This program was originally written.

	= 2018-12-21, David A.D. Morano
	Added |pwentrybufsize(3dam)|.

*/

/* Copyright © 1998,2018 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pwentrybufsize

	Description:
	We provide some miscelleneous utility subroutines for the PWENTRY
	object.

	Synopsis:
	int pwentrybufsize(void) noex

	Arguments:
	-

	Returns:
	>=0		size of necessary buffer to hold the data for 
			a PWENTRY object
	<0		error (yes, due to error from |getbufsize(3uc)|).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<strn.h>
#include	<snx.h>
#include	<snwcpy.h>
#include	<storeitem.h>
#include	<gecos.h>
#include	<cfdec.h>
#include	<mallocstuff.h>
#include	<localmisc.h>

#include	"pwentry.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	loaditem(cchar **,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pwentry_start(pwentry *ep) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    rs = memclear(ep) ;
	    ep->lstchg = -1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_start) */

int pwentry_fieldpw(pwentry *ep,int fn,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (ep && sp) {
	    int		v ;
	    cchar	*mp = nullptr ;
	    rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    switch (fn) {
	    case 0:
	        ep->username = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 1:
	        ep->password = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 2:
	        mp = sp ;
	        ep->uid = -1 ;
	        if ((sl > 0) && (cfdeci(sp,sl,&v) >= 0)) ep->uid = v ;
	        break ;
	    case 3:
	        mp = sp ;
	        ep->gid = -1 ;
	        if ((sl > 0) && (cfdeci(sp,sl,&v) >= 0)) ep->gid = v ;
	        break ;
	    case 4:
	        ep->gecos = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 5:
	        ep->dir = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 6:
	        ep->shell = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 7:
	        mp = sp ;
	        ep->lstchg = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->lstchg) ;
	        break ;
	    case 8:
	        mp = sp ;
	        ep->daymin = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->daymin) ;
	        break ;
	    case 9:
	        mp = sp ;
	        ep->daymax = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->daymax) ;
	        break ;
	    case 10:
	        mp = sp ;
	        ep->warn = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->warn) ;
	        break ;
	    case 11:
	        mp = sp ;
	        ep->inact = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->inact) ;
	        break ;
	    case 12:
	        mp = sp ;
	        ep->expire = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->expire) ;
	        break ;
	    case 13:
	        mp = sp ;
	        if (sl > 0) {
		    if (cfdec(sp,sl,&v)) {
	                ep->flag = v ;
		    }
		}
	        break ;
	    default:
	        mp = sp ;
	        break ;
	    } /* end switch */
	    rs = ((mp == nullptr) ? SR_NOMEM : SR_OK) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_fieldpw) */

int pwentry_mkextras(pwentry *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->gecos != nullptr) {
	        gecos	g ;
	        if ((rs = gecos_start(&g,ep->gecos,-1)) >= 0) {
	            for (int i = 0 ; i < gecosval_overlast ; i += 1) {
	                cchar	*vp{} ;
	                if (int vl ; (vl = gecos_getval(&g,i,&vp)) >= 0) {
	                    if (vp != nullptr) {
	                        void	*p{} ;
	                        switch (i) {
	                        case gecosval_organization:
	                            rs = loaditem(&ep->organization,vp,vl) ;
	                            break ;
	                        case gecosval_realname:
	                            if ((rs = uc_malloc((vl+1),&p)) >= 0) {
	                                char	*nbuf = charp(p) ;
	                                if (strnchr(vp,vl,'_') != nullptr) {
	                                    rs = snwcpyhyphen(nbuf,-1,vp,vl) ;
	                                    vp = nbuf ;
	                                }
	                                if (rs >= 0) {
	                                    rs = loaditem(&ep->realname,vp,vl) ;
	                                }
	                                rs1 = uc_free(p) ;
					if (rs >= 0) rs = rs1 ;
	                            } /* end if (m-a-f) */
	                            break ;
	                        case gecosval_account:
	                            rs = loaditem(&ep->account,vp,vl) ;
	                            break ;
	                        case gecosval_bin:
	                            rs = loaditem(&ep->bin,vp,vl) ;
	                            break ;
	                        case gecosval_office:
	                            rs = loaditem(&ep->office,vp,vl) ;
	                            break ;
	                        case gecosval_wphone:
	                            rs = loaditem(&ep->wphone,vp,vl) ;
	                            break ;
	                        case gecosval_hphone:
	                            rs = loaditem(&ep->hphone,vp,vl) ;
	                            break ;
	                        case gecosval_printer:
	                            rs = loaditem(&ep->printer,vp,vl) ;
	                            break ;
	                        } /* end switch */
	                    } /* end if (non-nullptr) */
	                } /* end if (gecos-getval) */
	                if (rs < 0) break ;
	            } /* end for */
	            rs1 = gecos_finish(&g) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (gecos) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_mkextras) */

int pwentry_mkcopy(pwentry *ep,pwentry *uep,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ep && uep && rbuf) {
	    storeitem	ubuf ;
	    memclear(uep) ;
	    if ((rs = storeitem_start(&ubuf,rbuf,rlen)) >= 0) {
	        if (ep->username != nullptr) {
	            storeitem_strw(&ubuf,ep->username,-1,&uep->username) ;
	        }
	        if (ep->password != nullptr) {
	            storeitem_strw(&ubuf,ep->password,-1,&uep->password) ;
	        }
#ifdef	OPTIONAL
	        uep->uid = ep->uid ;
	        uep->gid = ep->gid ;
#endif
	        if (ep->gecos != nullptr) {
	            storeitem_strw(&ubuf,ep->gecos,-1,&uep->gecos) ;
	        }
/* the standard AT&T stuff */
	        if (ep->organization != nullptr) {
		    cchar	**cpp = &uep->organization ;
	            storeitem_strw(&ubuf,ep->organization,-1,cpp) ;
	        }
	        if (ep->realname != nullptr) {
	            storeitem_strw(&ubuf,ep->realname,-1,&uep->realname) ;
	        }
	        if (ep->account != nullptr) {
	            storeitem_strw(&ubuf,ep->account,-1,&uep->account) ;
	        }
	        if (ep->bin != nullptr) {
	            storeitem_strw(&ubuf,ep->bin,-1,&uep->bin) ;
	        }
/* the finger information stuff */
	        if (ep->office != nullptr) {
	            storeitem_strw(&ubuf,ep->office,-1,&uep->office) ;
	        }
	        if (ep->wphone != nullptr) {
	            storeitem_strw(&ubuf,ep->wphone,-1,&uep->wphone) ;
	        }
	        if (ep->hphone != nullptr) {
	            storeitem_strw(&ubuf,ep->hphone,-1,&uep->hphone) ;
	        }
	        if (ep->printer != nullptr) {
	            storeitem_strw(&ubuf,ep->printer,-1,&uep->printer) ;
	        }
/* the remaining standard password stuff */
	        if (ep->dir != nullptr) {
	            storeitem_strw(&ubuf,ep->dir,-1,&uep->dir) ;
		}
	        if (ep->shell != nullptr) {
	            storeitem_strw(&ubuf,ep->shell,-1,&uep->shell) ;
	        }
	        len = storeitem_finish(&ubuf) ;
	        if (rs >= 0) rs = len ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pwentry_mkcopy) */

int pwentry_finish(pwentry *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->username != nullptr) {
	        rs1 = uc_free(ep->username) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->password != nullptr) {
	        rs1 = uc_free(ep->password) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->gecos != nullptr) {
	        rs1 = uc_free(ep->gecos) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->dir != nullptr) {
	        rs1 = uc_free(ep->dir) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->shell != nullptr) {
	        rs1 = uc_free(ep->shell) ;
	        if (rs >= 0) rs = rs1 ;
	    }
/* the AT&T standard extras */
	    if (ep->organization != nullptr) {
	        rs1 = uc_free(ep->organization) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->realname != nullptr) {
	        rs1 = uc_free(ep->realname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->account != nullptr) {
	        rs1 = uc_free(ep->account) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->bin != nullptr) {
	        rs1 = uc_free(ep->bin) ;
	        if (rs >= 0) rs = rs1 ;
	    }
/* the sometimes pseudo-standard finger information fields */
	    if (ep->office != nullptr) {
	        rs1 = uc_free(ep->office) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->wphone != nullptr) {
	        rs1 = uc_free(ep->wphone) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->hphone != nullptr) {
	        rs1 = uc_free(ep->hphone) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->printer != nullptr) {
	        rs1 = uc_free(ep->printer) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    memclear(ep) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_finish) */

int pwentrybufsize() noex {
	int		rs ;
	int		pwlen = 0 ;
	if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	    pwlen += rs ;
	    if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	        pwlen += rs ; /* additional size for some extra stuff */
	    }
	}
	return (rs >= 0) ? pwlen : rs ;
}
/* end subroutine (pwentrybufsize) */


/* local subroutines */

static int loaditem(cchar **rpp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (vp) {
	    rs = uc_mallocstrw(vp,vl,rpp) ;
	}
	return rs ;
}
/* end subroutine (loaditem) */


