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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<snx.h>			/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<storeitem.h>		/* LIBUC */
#include	<gecos.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"pwentry.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */

/* local defines */

#define	PE	pwentry

#ifndef	F_OPTIONAL
#define	F_OPTIONAL	0		/* <- why this choice? */
#endif

/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		pwlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

local int	pwentry_loadname(PE *,cc *,int) noex ;
local int	loaditem(cchar **,cchar *,int) noex ;


/* local variables */

static vars	var ;
cbool		f_optional = F_OPTIONAL ;


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
} /* end subroutine (pwentry_start) */

int pwentry_fieldpw(pwentry *op,int fn,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) ylikely {
	    long	lv ;
	    int		v ;
	    cchar	**cpp = nullptr ;
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    switch (fn) {
	    case 0:
	        cpp = &op->username ;
	        break ;
	    case 1:
	        cpp = &op->password ;
	        break ;
	    case 2:
	        op->uid = -1 ;
	        if ((sl > 0) && (cfdeci(sp,sl,&v) >= 0)) {
		    op->uid = v ;
		}
	        break ;
	    case 3:
	        op->gid = -1 ;
	        if ((sl > 0) && (cfdeci(sp,sl,&v) >= 0)) {
		    op->gid = v ;
		}
	        break ;
	    case 4:
	        cpp = &op->gecos ;
	        break ;
	    case 5:
	        cpp = &op->dir ;
	        break ;
	    case 6:
	        cpp = &op->shell ;
	        break ;
	    case 7:
	        op->lstchg = -1 ;
	        if (sl > 0) {
		    cfdecl(sp,sl,&lv) ;
		    op->lstchg = lv ;
		}
	        break ;
	    case 8:
	        op->daymin = -1 ;
	        if (sl > 0) {
		    cfdecl(sp,sl,&lv) ;
		    op->daymin = lv ;
		}
	        break ;
	    case 9:
	        op->daymax = -1 ;
	        if (sl > 0) {
		    cfdecl(sp,sl,&lv) ;
		    op->daymax = lv ;
		}
	        break ;
	    case 10:
	        op->warn = -1 ;
	        if (sl > 0) {
		    cfdecl(sp,sl,&lv) ;
		    op->warn = lv ;
		}
	        break ;
	    case 11:
	        op->inact = -1 ;
	        if (sl > 0) {
		    cfdecl(sp,sl,&lv) ;
		    op->inact = lv ;
		}
	        break ;
	    case 12:
	        op->expire = -1 ;
	        if (sl > 0) {
		    cfdecl(sp,sl,&lv) ;
		    op->expire = lv ;
		}
	        break ;
	    case 13:
	        if (sl > 0) {
		    if (uint uv ; cfdec(sp,sl,&uv)) {
	                op->flag = uv ;
		    }
		}
	        break ;
	    default:
		rs = SR_BUGCHECK ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && cpp) {
		rs = libmem.strw(sp,sl,cpp) ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pwentry_fieldpw) */

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
	                        case gecosval_prbin:
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
} /* end subroutine (pwentry_mkextras) */

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
		if_constexpr (f_optional) {
	            uop->uid = op->uid ;
	            uop->gid = op->gid ;
		}
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
} /* end subroutine (pwentry_mkcopy) */

namespace {
    struct memfreer {
	int	rs = SR_OK ;
	int	rs1 ;
	void operator () (ccharp &p) noex {
	    if (p) ylikely {
		voidp vp = voidp(p) ;
	        rs1 = libmem.free(vp) ;
		if (rs >= 0) rs = rs1 ;
		p = nullptr ;
	    } /* end if (memory-release) */
	} ; /* end method */
	operator int () const noex {
	    return rs ;
	} ; /* end method */
    } ; /* end struct */
} /* end namespace */

int pwentry_finish(pwentry *op) noex {
    	PWENTRY		*hop = op ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    memfreer fo ;
	    rs = SR_OK ;
	    fo(op->username) ;
	    fo(op->password) ;
	    fo(op->gecos) ;
	    fo(op->dir) ;
	    fo(op->shell) ;
	    /* the AT&T standard extras */
	    fo(op->organization) ;
	    fo(op->realname) ;
	    fo(op->account) ;
	    fo(op->bin) ;
	    /* BSD fields */
	    fo(op->office) ;
	    fo(op->wphone) ;
	    fo(op->hphone) ;
	    fo(op->printer) ;
	    rs1 = fo ;
	    if (rs >= 0) rs = rs1 ;
	    memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pwentry_finish) */

int pwentrybufsize() noex {
    	static cint	rs = var ;
	return rs ;
} /* end subroutine (pwentrybufsize) */


/* local subroutines */

local int pwentry_loadname(PE *op,cc *vp,int vl) noex {
    	int		rs ;
	int		rs1 ;
	int		tl = 0 ;
	if (char *tbuf ; (rs = lm_mall((vl + 1),&tbuf)) >= 0) ylikely {
	    cint	tlen = rs ;
	    if (strnchr(vp,vl,'_') != nullptr) {
		rs = snwcpyhyphen(tbuf,tlen,vp,vl) ;
		vp = tbuf ;
	    }
	    if (rs >= 0) ylikely {
		rs = loaditem(&op->realname,vp,vl) ;
		tl = rs ;
	    }
	    rs1 = lm_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? tl : rs ;
} /* end subroutine (pwentry_loadname) */

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
} /* end method (pwentry_co::operator) */

local int loaditem(cchar **rpp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (vp) ylikely {
	    rs = lm_strw(vp,vl,rpp) ;
	}
	return rs ;
} /* end subroutine (loaditem) */

vars::operator int () noex {
	int		rs ;
	if ((rs = bufsizeget(bufsize_pw)) >= 0) ylikely {
	    pwlen += rs ;
	    if ((rs = bufsizeget(bufsize_mn)) >= 0) ylikely {
	        pwlen += rs ; /* additional size for some extra stuff */
	    }
	}
	return (rs >= 0) ? pwlen : rs ;
} /* end method (vars::operator) */


