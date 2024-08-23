/* removename SUPPORT */
/* lang=C++20 */

/* remove a named file-system object (and its descendants) */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was adapated from other programs that do
	similar types of functions.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	removename

	Description:
	This function removes a named UNIX file-system object along
	with all of its descendents (if any).

	Synopsis:
	int removename(cchar *name,randomvar *rvp,int opts,int bcount) noex

	Arguments:
	name		name of FS object to remove
	rvp		pointer to RADNDOMVAR object
	opts		options
	bcount		burn-count (number of times to burn files)

	Returns:
	>=0		count of objects (files or directories) removed
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<wdt.h>
#include	<vecpstr.h>
#include	<randomvar.h>
#include	<localmisc.h>

#include	"removename.h"


/* local defines */

#define	REMOVEINFO	struct removeinfo
#define	REMOVEINFO_FL	struct removeinfo_flags


/* imported namespaces */

extern "C" {
    typedef int (*wdt_f)() noex ;
}


/* local typedefs */


/* external subroutines */

extern int	burn(randomvar *,int,cchar *) noex ;


/* external variables */


/* external variables */


/* local structures */

struct removeinfo_flags {
	uint		burn:1 ;
	uint		follow:1 ;
} ;

struct removeinfo {
	randomvar	*rvp ;
	REMOVEINFO_FL	f ;
	vecpstr		dirs ;
	uint		c_removed ;
	int		bcount ;
} ;


/* forward references */

static int	removeit(cchar *,USTAT *,REMOVEINFO *) noex ;
static int	rmdirs(REMOVEINFO *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int removename(randomvar *rvp,int bcount,int opts,cchar *name) noex {
	void_f		vuf = void_f(removeit) ;
	USTAT		sb, sb2 ;
	REMOVEINFO	ri{} ;
	randomvar	x ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wopts = 0 ;
	bool		f_rv = false ;

	if (name == NULL)
	    return SR_FAULT ;

	rs = u_lstat(name,&sb) ;
	if (rs < 0)
	    goto ret0 ;

	ri.f.burn = (opts & REMOVENAME_MBURN) ? 1 : 0 ;
	ri.f.follow = (opts & REMOVENAME_MFOLLOW) ? 1 : 0 ;
	ri.rvp = rvp ;
	ri.bcount = bcount ;
	ri.c_removed = 0 ;

	if (ri.f.burn && (rvp == NULL)) {

	    rs = randomvar_start(&x,false,opts) ;
	    if (rs < 0)
	        goto ret0 ;

	    f_rv = true ;
	    ri.rvp = &x ;

	} /* end if (we need our own random variable) */

	if ((rs = vecpstr_start(&ri.dirs,10,0,0)) >= 0) {
	    wdt_f		wuf = wdt_f(vuf) ;

/* continue with our processing */

	    if (S_ISLNK(sb.st_mode)) {

	        if (ri.f.follow &&
	            (u_stat(name,&sb2) >= 0) && S_ISDIR(sb2.st_mode)) {

	            wopts |= ((ri.f.follow) ? WDT_MFOLLOW : 0) ;
	            if ((rs = wdt(name,wopts,wuf,&ri)) >= 0) {
	                rs = vecpstr_add(&ri.dirs,name,-1) ;
		    }
	        } else {
	            rs = removeit(name,&sb,&ri) ;
	        } /* end if */

	    } else if (S_ISDIR(sb.st_mode)) {

	        wopts |= ((ri.f.follow) ? WDT_MFOLLOW : 0) ;
	        rs = wdt(name,wopts,wuf,&ri) ;

	        if (rs >= 0)
	            rs = vecpstr_add(&ri.dirs,name,-1) ;

	    } else {

	        rs = removeit(name,&sb,&ri) ;

	    } /* end if */

/* remove the directories */

	    if (rs >= 0) {
	        rs = rmdirs(&ri) ;
	    }
	    rs1 = vecpstr_finish(&ri.dirs) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecpstr) */

	if (f_rv) {
	    rs1 = randomvar_finish(&x) ;
	    if (rs >= 0) rs = rs1 ;
	}

ret0:

	return (rs >= 0) ? ri.c_removed : rs ;
}
/* end subroutine (removename) */


/* local subroutines */

static int removeit(cchar *name,USTAT *sbp,removeinfo *rip) noex {
	int		rs = SR_OK ;
	if (S_ISLNK(sbp->st_mode)) {
	    USTAT	sb2 ;
	    bool	f = true ;
	    rs = (rip->f.follow) ? 0 : 1 ;
	    f = f && rip->f.follow ;
	    f = f && (u_stat(name,&sb2) >= 0) ;
	    f = f && S_ISDIR(sb2.st_mode) ;
	    if (f) {
	        rs = vecpstr_add(&rip->dirs,name,-1) ;
	    } else {
	        if ((rs = u_unlink(name)) >= 0) {
	            rip->c_removed += 1 ;
		}
	    } /* end if */
	} else if (S_ISDIR(sbp->st_mode)) {
	    rs = vecpstr_add(&rip->dirs,name,-1) ;
	} else {
	    if (rip->f.burn && S_ISREG(sbp->st_mode)) {
	        rs = burn(rip->rvp,rip->bcount,name) ;
	    }
	    if (rs >= 0) {
	        if ((rs = u_unlink(name)) >= 0) {
	            rip->c_removed += 1 ;
		}
	    } /* end if */
	} /* end if */
	return rs ;
}
/* end subroutine (removeit) */

static int rmdirs(REMOVEINFO *rip) noex {
	vecpstr		*dlp = &rip->dirs ;
	int		rs ;
	int		n = 0 ;
	if ((rs = vecpstr_count(dlp)) > 0) {
	    n = rs ;
	    if ((rs = vecpstr_sort(dlp,NULL)) >= 0) {
		cchar	*cp ;
	        for (int i = (n - 1) ; i >= 0 ; i -= 1) {
	            if (vecpstr_get(dlp,i,&cp) >= 0) {
	                if (cp) {
	                    if ((rs = u_rmdir(cp)) >= 0) {
	                        rip->c_removed += 1 ;
			    }
	                }
	            }
		    if (rs < 0) break ;
	        } /* end for */
	    } /* end if */
	} /* end if */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (rmdirs) */


