/* pwentry SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* some miscellaneous PWENTRY subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A-D- Morano
	This program was originally written.

	= 2018-12-21, David A-D- Morano
	Added |pwentrybufsize(3dam)|.  I added this to facilitate
	buffer allocations for using this object.

*/

/* Copyright © 1998,2018 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pwentry

	Description:
	A PWNETRY is an object that stores an extended record of a
	UNIX® user account.  Of course, this is similar to a PWENT
	object (itself a derived object from the standard PASSWD
	object).  A PWENTRY is not a derived object but rather its
	own base object.

	Name:
	pwentrybufsize

	Description:
	This (below) is a utility subroutines for the
	PWENTRY object. 

	Synopsis:
	int pwentrybufsize(void) noex

	Arguments:
	-

	Returns:
	>=0		size of necessary buffer to hold the data for 
			a PWENTRY object
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<snx.h>
#include	<snwcpy.h>
#include	<storeitem.h>
#include	<gecos.h>
#include	<cfdec.h>
#include	<mallocstuff.h>
#include	<localmisc.h>

#include	"pwentry.h"

import libutil ;

/* local defines */

#define	PE	pwentry


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		pwlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

static int	pwentry_loadname(PE *,cc *,int) noex ;
static int	loaditem(cchar **,cchar *,int) noex ;


/* local variables */

static vars	var ;


/* exported variables */


/* exported subroutines */

int pwentry_start(pwentry *op) noex {
    	PWENTRY		*hop = op ;
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = memclear(hop) ;
	    op->lstchg = -1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_start) */

int pwentry_fieldpw(pwentry *op,int fn,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) ylikely {
	    int		v ;
	    cchar	*mp = nullptr ;
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    switch (fn) {
	    case 0:
	        op->username = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 1:
	        op->password = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 2:
	        mp = sp ;
	        op->uid = -1 ;
	        if ((sl > 0) && (cfdeci(sp,sl,&v) >= 0)) {
		    op->uid = v ;
		}
	        break ;
	    case 3:
	        mp = sp ;
	        op->gid = -1 ;
	        if ((sl > 0) && (cfdeci(sp,sl,&v) >= 0)) {
		    op->gid = v ;
		}
	        break ;
	    case 4:
	        op->gecos = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 5:
	        op->dir = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 6:
	        op->shell = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 7:
	        mp = sp ;
	        op->lstchg = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&op->lstchg) ;
	        break ;
	    case 8:
	        mp = sp ;
	        op->daymin = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&op->daymin) ;
	        break ;
	    case 9:
	        mp = sp ;
	        op->daymax = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&op->daymax) ;
	        break ;
	    case 10:
	        mp = sp ;
	        op->warn = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&op->warn) ;
	        break ;
	    case 11:
	        mp = sp ;
	        op->inact = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&op->inact) ;
	        break ;
	    case 12:
	        mp = sp ;
	        op->expire = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&op->expire) ;
	        break ;
	    case 13:
	        mp = sp ;
	        if (sl > 0) {
		    if (cfdec(sp,sl,&v)) {
	                op->flag = v ;
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

int pwentry_mkextras(pwentry *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->gecos != nullptr) ylikely {
	        if (gecos g ; (rs = g.start(op->gecos)) >= 0) ylikely {
	            for (int i = 0 ; i < gecosval_overlast ; i += 1) {
	                cchar	*vp{} ;
	                if (int vl ; (vl = g.getval(i,&vp)) >= 0) ylikely {
	                    if (vp != nullptr) {
	                        switch (i) {
	                        case gecosval_organization:
	                            rs = loaditem(&op->organization,vp,vl) ;
	                            break ;
	                        case gecosval_realname:
				    rs = pwentry_loadname(op,vp,vl) ;
	                            break ;
	                        case gecosval_account:
	                            rs = loaditem(&op->account,vp,vl) ;
	                            break ;
	                        case gecosval_bin:
	                            rs = loaditem(&op->bin,vp,vl) ;
	                            break ;
	                        case gecosval_office:
	                            rs = loaditem(&op->office,vp,vl) ;
	                            break ;
	                        case gecosval_wphone:
	                            rs = loaditem(&op->wphone,vp,vl) ;
	                            break ;
	                        case gecosval_hphone:
	                            rs = loaditem(&op->hphone,vp,vl) ;
	                            break ;
	                        case gecosval_printer:
	                            rs = loaditem(&op->printer,vp,vl) ;
	                            break ;
	                        } /* end switch */
	                    } /* end if (non-nullptr) */
	                } /* end if (gecos-getval) */
	                if (rs < 0) break ;
	            } /* end for */
	            rs1 = g.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (gecos) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_mkextras) */

int pwentry_mkcopy(pwentry *op,pwentry *uop,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && uop && rbuf) ylikely {
	    memclear(uop) ;
	    if (storeitem ub ; (rs = ub.start(rbuf,rlen)) >= 0) ylikely {
	        if (op->username != nullptr) {
	            ub.strw(op->username,-1,&uop->username) ;
	        }
	        if (op->password != nullptr) {
	            ub.strw(op->password,-1,&uop->password) ;
	        }
#ifdef	OPTIONAL
	        uop->uid = op->uid ;
	        uop->gid = op->gid ;
#endif
	        if (op->gecos != nullptr) {
	            ub.strw(op->gecos,-1,&uop->gecos) ;
	        }
		/* the standard AT&T stuff */
	        if (op->organization != nullptr) {
		    cchar	**cpp = &uop->organization ;
	            ub.strw(op->organization,-1,cpp) ;
	        }
	        if (op->realname != nullptr) {
	            ub.strw(op->realname,-1,&uop->realname) ;
	        }
	        if (op->account != nullptr) {
	            ub.strw(op->account,-1,&uop->account) ;
	        }
	        if (op->bin != nullptr) {
	            ub.strw(op->bin,-1,&uop->bin) ;
	        }
		/* the finger information stuff */
	        if (op->office != nullptr) {
	            ub.strw(op->office,-1,&uop->office) ;
	        }
	        if (op->wphone != nullptr) {
	            ub.strw(op->wphone,-1,&uop->wphone) ;
	        }
	        if (op->hphone != nullptr) {
	            ub.strw(op->hphone,-1,&uop->hphone) ;
	        }
	        if (op->printer != nullptr) {
	            ub.strw(op->printer,-1,&uop->printer) ;
	        }
		/* the remaining standard password stuff */
	        if (op->dir != nullptr) {
	            ub.strw(op->dir,-1,&uop->dir) ;
		}
	        if (op->shell != nullptr) {
	            ub.strw(op->shell,-1,&uop->shell) ;
	        }
	        len = ub.finish ;
	        if (rs >= 0) rs = len ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pwentry_mkcopy) */

int pwentry_finish(pwentry *op) noex {
    	PWENTRY		*hop = op ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->username != nullptr) {
	        rs1 = uc_free(op->username) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->password != nullptr) {
	        rs1 = uc_free(op->password) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->gecos != nullptr) {
	        rs1 = uc_free(op->gecos) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dir != nullptr) {
	        rs1 = uc_free(op->dir) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->shell != nullptr) {
	        rs1 = uc_free(op->shell) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    /* the AT&T standard extras */
	    if (op->organization != nullptr) {
	        rs1 = uc_free(op->organization) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->realname != nullptr) {
	        rs1 = uc_free(op->realname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->account != nullptr) {
	        rs1 = uc_free(op->account) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->bin != nullptr) {
	        rs1 = uc_free(op->bin) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    /* the sometimes pseudo-standard finger information fields */
	    if (op->office != nullptr) {
	        rs1 = uc_free(op->office) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->wphone != nullptr) {
	        rs1 = uc_free(op->wphone) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->hphone != nullptr) {
	        rs1 = uc_free(op->hphone) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->printer != nullptr) {
	        rs1 = uc_free(op->printer) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_finish) */

int pwentrybufsize() noex {
    	static cint	rs = var ;
	return rs ;
}
/* end subroutine (pwentrybufsize) */


/* local subroutines */

static int pwentry_loadname(PE *op,cc *vp,int vl) noex {
    	int		rs ;
	int		rs1 ;
	int		tl = 0 ;
	if (char *tbuf ; (rs = uc_malloc((vl + 1),&tbuf)) >= 0) ylikely {
	    cint	tlen = rs ;
	    if (strnchr(vp,vl,'_') != nullptr) {
		rs = snwcpyhyphen(tbuf,tlen,vp,vl) ;
		vp = tbuf ;
	    }
	    if (rs >= 0) ylikely {
		rs = loaditem(&op->realname,vp,vl) ;
		tl = rs ;
	    }
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? tl : rs ;
}
/* end subroutine (pwentry_loadname) */

int pwentry::fieldpw(int fn,cchar *sp,int sl) noex {
	return pwentry_fieldpw(this,fn,sp,sl) ;
}

int pwentry::mkcopy(pwentry *oop,char *rbuf,int rlen) noex {
	return pwentry_mkcopy(this,oop,rbuf,rlen) ;
}

void pwentry::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("pwentry",rs,"fini-finish") ;
	}
} /* end method (pwentry::dtor) */

pwentry_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case pwentrymem_start:
	        rs = pwentry_start(op) ;
	        break ;
	    case pwentrymem_mkextras:
	        rs = pwentry_mkextras(op) ;
	        break ;
	    case pwentrymem_finish:
	        rs = pwentry_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (pwentry_co::operator) */

static int loaditem(cchar **rpp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (vp) ylikely {
	    rs = uc_mallocstrw(vp,vl,rpp) ;
	}
	return rs ;
}
/* end subroutine (loaditem) */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_pw)) >= 0) ylikely {
	    pwlen += rs ;
	    if ((rs = getbufsize(getbufsize_mn)) >= 0) ylikely {
	        pwlen += rs ; /* additional size for some extra stuff */
	    }
	}
	return (rs >= 0) ? pwlen : rs ;
} /* end method (vars::operator) */


