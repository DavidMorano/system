/* prgetprogpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the path to a program that is used within the PCS system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	prgetprogpath

	Description:
	This subroutine is used to find a PCS related program and
	to verify that it is executable.

	Important:
	This subroutine is different from |pcsgetprog(3pcs)| in
	that this will return a full path of the found program
	whenever it is different than what was supplied.  In contrast,
	the |pcsgetprog(3pcs)| subroutine only returns the full
	path of the found program when it is not absolute and it
	is found in the PCS distribution.

	Synopsis:
	int prgetprogpath(cchar *pr,char *rbuf,cchar *np,int nl) noex

	Arguments:
	pr		program-root path
	rbuf		returned file path if not the same as input
	np		name
	nl		name-length

	Returns:
	>0		found the program path and this is the length
	==0		program was found w/o a path prefix
	<0		program was not found

	programpath	returned file path if it was not in the PWD

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getbufsize.h>
#include	<bufsizevar.hh>
#include	<vecstr.h>
#include	<ids.h>
#include	<mkpathx.h>
#include	<mkpathxw.h>
#include	<pathadd.h>
#include	<storebuf.h>
#include	<strn.h>
#include	<strx.h>
#include	<rmx.h>
#include	<permx.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"prgetprogpath.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#define	SI		subinfo


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	cchar		*pr ;
	ids		id ;
	vecstr		dirs ;
	uint		f_dirs:1 ;
	uint		f_changed:1 ;
	uint		f_done:1 ;
} ; /* end struct (subinfo) */


/* forward references */

local int	subinfo_start(SI *,cchar *) noex ;
local int	subinfo_xfile(SI *,cchar *) noex ;
local int	subinfo_record(SI *,cchar *,int) noex ;
local int	subinfo_finish(SI *) noex ;

local int	subinfo_tryfull(SI *,char *,cchar *,int) noex ;
local int	subinfo_tryroot(SI *,char *,cchar *,int) noex ;
local int	subinfo_tryother(SI *,char *,cchar *,int) noex ;
local int	subinfo_tryothercheck(SI *,cchar *,int,
			char *,cchar *,int) noex ;

local int	mkdfname(char *,cchar *,int,cchar *,int) noex ;

local bool	isOverNoEntAcc(int) noex ;


/* local variables */

typedef int (*subinfo_f)(SI *,char *,cchar *,int) noex ;

constexpr subinfo_f	tries[] = {
	subinfo_tryfull,
	subinfo_tryroot,
	subinfo_tryother,
	nullptr
} ; /* end array (tries) */

constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ; /* end array (prbins) */

constexpr int		rsentacc[] = {
	SR_NOENT,
	SR_ACCESS,
	SR_OVERFLOW,
	SR_NAMETOOLONG,
	SR_RANGE,
	0
} ; /* end array (rsentacc) */


/* exported variables */


/* exported subroutines */

int prgetprogpath(cchar *pr,char *rbuf,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (pr && rbuf && sp) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (sp[0]) {
		bool fchanged = false ;
	        if (sl < 0) sl = lenstr(sp) ;
	        while ((sl > 0) && (sp[sl - 1] == '/')) {
	            fchanged = true ;
	            sl -= 1 ;
	        } /* end while */
	        if (subinfo si ; (rs = subinfo_start(&si,pr)) >= 0) {
	            for (int i = 0 ; tries[i] ; i += 1) {
			subinfo_f fun = tries[i] ;
		        rs = fun(&si,rbuf,sp,sl) ;
		        if ((rs != 0) || si.f_done) break ;
	            } /* end for */
	            if (rs >= 0) {
		        rl = rs ;
		        if (rl == 0) {
		            rs = SR_NOENT ;
 		        } else if (rl > 0) {
		            if (! (fchanged || si.f_changed)) rl = 0 ;
		        }
	            } /* end if (ok) */
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (prgetprogpath) */


/* local subroutines */

local int subinfo_start(SI *sip,cchar *pr) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    memclear(sip) ;
	    sip->pr = pr ;
	    rs = ids_load(&sip->id) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    if (sip->f_dirs) {
	        sip->f_dirs = false ;
	        rs1 = vecstr_finish(&sip->dirs) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ids_release(&sip->id) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

local int subinfo_tryfull(SI *sip,char *rbuf,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		rl = 0 ; /* return-value */
	if (strnchr(sp,sl,'/') != nullptr) {
	    if ((rs = mkpath1w(rbuf,sp,sl)) >= 0) {
		rl = rs ;
		rs = subinfo_xfile(sip,rbuf) ;
	        if (isNotPresent(rs)) {
		    rs = SR_OK ;
		    rl = 0 ;
		    sip->f_done = true ;
		}
	    }
	} /* end if (full-path) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (subinbfo_tryfull) */

local int subinfo_tryroot(SI *sip,char *rbuf,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		rl = 0 ; /* return-value */
	cchar		*pr = sip->pr ;
	for (int i = 0 ; prbins[i] ; i += 1) {
	    rl = 0 ;
	    if ((rs = mkpath2(rbuf,pr,prbins[i])) >= 0) {
	        cint	plen = rs ;
		if ((rs = pathaddw(rbuf,plen,sp,sl)) >= 0) {
		    rl = rs ;
	            if ((rs = subinfo_xfile(sip,rbuf)) >= 0) {
			break ;
		    } else if (isOverNoEntAcc(rs)) {
	                rs = subinfo_record(sip,rbuf,rl) ;
			rl = 0 ;
		    }
		}
	    } else if (isOverNoEntAcc(rs)) {
	        rs = SR_OK ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && (rl > 0)) {
	    sip->f_changed = true ;
	}
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (subinfo_tryroot) */

local int subinfo_tryother(SI *sip,char *rbuf,cchar *sp,int sl) noex {
	static cchar	*path = getenv(varname.path) ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rl = 0 ; /* return-value */
	if (path) {
	    cchar	*pp = path ;
	    for (cchar *tp ; ((tp = strbrk(pp,":;")) != np) && (rl == 0) ; ) {
		if (cint tl = intconv(tp - pp) ; tl > 0) {
	            rs = subinfo_tryothercheck(sip,pp,tl,rbuf,sp,sl) ;
	            rl = rs ;
	            pp = (tp + 1) ;
		}
		if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (rl == 0)) {
	        if (pp[0] != '\0') {
	            rs = subinfo_tryothercheck(sip,pp,-1,rbuf,sp,sl) ;
	            rl = rs ;
	        }
	    } /* end if (continue) */
	    if ((rs >= 0) && (rl > 0)) {
	        sip->f_changed = true ;
	    }
	} /* end if (getenv-path) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (subinfo_other) */

local int subinfo_tryothercheck(SI *sip,cchar *dp,int dl,
		char *rbuf,cchar *sp,int sl) noex {
	int		rs = SR_NOENT ;
	int		rl = 0 ; /* return-value */
	if (sip->f_dirs) {
	    rs = vecstr_findn(&sip->dirs,dp,dl) ;
	}
	if (rs == SR_NOENT) {
	    if ((rs = mkdfname(rbuf,dp,dl,sp,sl)) >= 0) {
	        rl = rs ;
	        rs = subinfo_xfile(sip,rbuf) ;
	        if (isNotAccess(rs)) {
	            rs = subinfo_record(sip,rbuf,rl) ;
		    rl = 0 ;
		}
	    } /* end if */
	} /* end if (no-entry) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (subinfo_tryothercheck) */

local int subinfo_xfile(SI *sip,cchar *name) noex {
	int		rs ;
	if (ustat sb ; (rs = u_stat(name,&sb)) >= 0) {
	    rs = SR_NOENT ;
	    if (S_ISREG(sb.st_mode)) {
	        rs = permid(&sip->id,&sb,X_OK) ;
	    }
	}
	return rs ;
}
/* end subroutine (subinfo_xfile) */

local int subinfo_record(SI *sip,cchar *dp,int dl) noex {
	int		rs = SR_OK ;
	if (! sip->f_dirs) {
	    rs = vecstr_start(&sip->dirs,10,0) ;
	    sip->f_dirs = (rs >= 0) ;
	}
	if (rs >= 0) {
	    rs = vecstr_add(&sip->dirs,dp,dl) ;
	}
	return rs ;
}
/* end subroutine (subinfo_record) */

local int mkdfname(char *rbuf,cchar *dp,int dl,cchar *sp,int sl) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = getbufsize(bufsize_mp)) >= 0) {
	    cint	rlen = rs ;
	    if (rs >= 0) {
	        rs = storebuf_strw(rbuf,rlen,i,dp,dl) ;
	        i += rs ;
	    }
	    if ((rs >= 0) && (i > 0) && (rbuf[i - 1] != '/')) {
	        rs = storebuf_chr(rbuf,rlen,i,'/') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_strw(rbuf,rlen,i,sp,sl) ;
	        i += rs ;
	    }
	} /* end if (getbufsize) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkdfname) */

local bool isOverNoEntAcc(int rs) noex {
	return isOneOf(rsentacc,rs) ;
}
/* end subroutine (isOverNoEntAcc) */


