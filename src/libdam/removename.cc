/* removename SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove a named file-system object (and its descendants if any) */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was adapted from other programs that do
	similar types of functions.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	removename

	Description:
	This function removes a named UNIX® file-system object along
	with all of its descendents (if any).  But besides a
	basic removæl (deletion), this subroutine can "burn"
	the file before deletion (a specified number of times
	as desired).

	Synopsis:
	int removename(cchar *name,int rno,randomvar *rvp,int bc) noex

	Arguments:
	name		name of FS object to remove
	rno		remove-name options
	rvp		pointer to RANDOMVAR object
	bc		burn-count (number of times to burn files)

	Returns:
	>=0		count of objects (files or directories) removed
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<wdt.h>
#include	<vecpstr.h>
#include	<randomvar.h>
#include	<burn.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"removename.h"


/* local defines */

#define	REMOVEINFO	struct remover
#define	REMOVEINFO_FL	struct remover_flags


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct remover_flags {
    	uint		randomvar:1 ;
	uint		burn:1 ;
	uint		follow:1 ;
} ; /* end struct */

namespace {
    struct remover {
	cchar		*name ;
	randomvar	*rvp ;
	vecpstr		dirs ;
	randomvar	x ;
	REMOVEINFO_FL	fl{} ;
	int		crem = 0 ;
	int		bcount = 0 ;
	remover(cc *n,randomvar *p,int bc) noex : name(n), rvp(p) {
	    if (bc > 0) bcount = bc ;
	} ;
	int start(int) noex ;
	int finish() noex ;
	int decider(ustat *) noex ;
	int rmdirs() noex ;
	int removeit(cchar *,ustat *) noex ;
	int removelink(cchar *,ustat *) noex ;
    } ; /* end struct (remover) */
} /* end namespace */


/* forward references */

extern "C" {
    local int	remover_co(cchar *,ustat *,void *) noex ;
} /* end extern (C) */


/* local variables */


/* exported variables */

constexpr removenameoms		removenamem ;


/* exported subroutines */

int removename(cchar *name,int rno,randomvar *rvp,int bcount) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (name) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
		if (ustat sb ; (rs = u_lstat(name,&sb)) >= 0) ylikely {
		    remover	ro(name,rvp,bcount) ;
		    if ((rs = ro.start(rno)) >= 0) ylikely {
			{
			    rs = ro.decider(&sb) ;
			    c = rs ;
			}
			rs1 = ro.finish() ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (remover) */
		} /* end if (lstat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutinte (removename) */


/* local subroutines */

int remover::start(int rno) noex {
    	int		rs = SR_OK ;
	fl.burn = bool(rno & REMOVENAME_MBURN) ;
	fl.follow = bool(rno & REMOVENAME_MFOLLOW) ;
	if (fl.burn && (rvp == nullptr)) {
	    if ((rs = x.start) >= 0) {
	        fl.randomvar = true ;
	        rvp = &x ;
	    }
	} /* end if (we need our own random variable) */
	return rs ;
} /* end method (remover::start) */

int remover::finish() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (fl.randomvar && rvp) ylikely {
	    rs1 = x.finish ;
	    if (rs >= 0) rs = rs1 ;
	    rvp = nullptr ;
	    fl.randomvar = false ;
	} /* end if (non-null) */
	return (rs >= 0) ? crem : rs ;
} /* end method (remover::finish) */

int remover::decider(ustat *sbp) noex {
    	cint		vn = 10 ;
	cint		vo = 0 ;
    	int		rs ;
	int		rs1 ;
	if ((rs = dirs.start(vn,0,vo)) >= 0) ylikely {
	    wdt_f	wuf = remover_co ;
	    int		wopts = 0 ;
	    wopts |= ((fl.follow) ? WDT_MFOLLOW : 0) ;
	    if (S_ISLNK(sbp->st_mode)) {
	        if (fl.follow) {
	            if (ustat sbf ; (rs = u_stat(name,&sbf)) >= 0) {
			if (S_ISDIR(sbf.st_mode)) {
	            	    rs = wdt(name,wopts,wuf,this) ;
			} else {
	                    rs = removeit(name,sbp) ;
			}
		    } else if (isNotPresent(rs)) {
	                rs = u_unlink(name) ;
		    } /* end if (stat) */
	        } else {
	            rs = u_unlink(name) ;
	        } /* end if */
	    } else if (S_ISDIR(sbp->st_mode)) {
	        rs = wdt(name,wopts,wuf,this) ;
	    } else if (S_ISREG(sbp->st_mode)) {
	        rs = removeit(name,sbp) ;
	    } else {
		rs = u_unlink(name) ;
	    } /* end if */
	    /* remove the directories */
	    if (rs >= 0) {
	        rs = rmdirs() ;
	    } /* end if (ok) */
	    rs1 = dirs.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecpstr) */
	return (rs >= 0) ? crem : rs ;
} /* end subroutine (remover::decider) */

int remover::removelink(cchar *n,ustat *) noex {
    	int		rs = SR_OK ;
	if (fl.follow) {
	    if (ustat sb2 ; (rs = u_stat(n,&sb2) >= 0)) {
	        if (S_ISDIR(sb2.st_mode)) {
	            rs = dirs.add(n) ;
		} else {
	            if ((rs = u_unlink(n)) >= 0) {
	                crem += 1 ;
		    }
		} /* end if */
	    } /* end if (u_stat) */
	} /* end if (follow) */
	return rs ;
} /* end method (remover::removelink) */

int remover::removeit(cchar *n,ustat *sbp) noex {
	int		rs = SR_OK ;
	if (S_ISLNK(sbp->st_mode)) {
	    rs = removelink(n,sbp) ;
	} else if (S_ISDIR(sbp->st_mode)) {
	    rs = dirs.add(n) ;
	} else {
	    if (fl.burn && S_ISREG(sbp->st_mode)) {
	        rs = burn(n,bcount,rvp) ;
	    }
	    if (rs >= 0) {
	        if ((rs = u_unlink(n)) >= 0) {
	            crem += 1 ;
		}
	    } /* end if */
	} /* end if */
	return rs ;
} /* end method (remover::removeit) */

int remover::rmdirs() noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		n = 0 ;
	if ((rs = dirs.count) > 0) {
	    n = rs ;
	    if ((rs = dirs.sort(nullptr)) >= 0) {
		bool	fe = false ;
	        for (int i = (n - 1) ; i >= 0 ; i -= 1) {
		    if (cchar *cp{} ; (rs = dirs.get(i,&cp)) >= 0) {
	                if (cp) {
	                    if ((rs = u_rmdir(cp)) >= 0) {
	                        crem += 1 ;
			    }
	                }
		    } else if (rs == rsn) {
			rs = SR_OK ;
			fe = true ;
	            }
		    if (fe) break ;
		    if (rs < 0) break ;
	        } /* end for */
	    } /* end if */
	} /* end if (vecpstr_count) */
	return (rs >= 0) ? n : rs ;
} /* end method (remover::rmdirs) */

local int remover_co(cchar *name,ustat *sbp,void *vop) noex {
    	remover		*op = (remover *) vop ;
	return op->removeit(name,sbp) ;
} /* end subroutine (remover_co) */


