/* msfile_best SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the "best" machine entry */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 2004-01-10, David A­D­ Morano
	:-) It cracks me up how I take one program and make another
	from it! :-) This program is now a built-in command (MSU)
	to the KSH program to update the machine status for the
	current node in the cluster. [added for program checking
	-- ((]

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	msfile_best

	Description:
	This subroutine provides an additional method to the MSFILE
	object.  It finds the best of the entries given a couple of
	optional restrictions.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"msfilee.h"
#include	"msflag.h"
#include	"msfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import msfile_mag ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	MSFLAG_DISABLED
#define	MSFLAG_DISABLED		0x0001
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct bester {
	msfile		*op ;
	msfile_ent	*ep ;
	double		valavailbest ;
	uint		minspeed ;
	int		flags ;
	int		cnt ;
	bester(msfile *o,msfile_ent *e,int fl) noex : op(o), ep(e) {
	    flags = fl ;
	    valavailbest = -1000.0 ;
	    minspeed = INT_MAX ;
	    cnt = 0 ;
	} ; /* end ctor */
	operator int () noex ;
	int findmin	() noex ;
	int findbest	() noex ;
	int procent	(msfile_ent *) noex ;
    } ; /* end struct (bester) */
} /* end namespace */


/* forward references */

local bool isEnabled(int fl) noex {
	return (! (fl & MSFLAG_DISABLED)) ;
} /* end subroutine */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int msfile_best(msfile *op,time_t dt,int flags,msfile_ent *ep) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	(void) dt ;
	if ((rs = msfile_magic(op,ep)) >= 0) {
	    if (bester bo(op,ep,flags) ; (rs = bo) >= 0) {
		c = rs ;
		if (c == 0) rs = SR_NOENT ;
	    }
	} /* end if (msfile_magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (msfile_best) */


/* local subroutines */

bester::operator int () noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = findmin()) > 0) {
	    rs = findbest() ;
	    c = rs ;
	} /* end if (findmin) */
	return (rs >= 0) ? c : rs ;
} /* end method (bester::operator) */

/* first find the minimum speed of all nodes */
int bester::findmin() noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (msfile_cur cur ; (rs = msfile_curbegin(op,&cur)) >= 0) ylikely {
	    msfile_ent	be ;
	    while ((rs1 = msfile_curenum(op,&cur,&be)) >= 0) {
	        if (be.speed != 0) {
		    c += 1 ;
	            if (be.speed < minspeed) minspeed = be.speed ;
	        }
		if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (rs1 < 0) && (rs1 != rsn)) rs = rs1 ;
	    rs1 = msfile_curend(op,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if *cursor) */
	return (rs >= 0) ? c : rs ;
} /* end method (bester::findmin) */

/* then find the one with the most available computation */
int bester::findbest() noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs ;
	int		rs1 ;
	if (msfile_cur cur ; (rs = msfile_curbegin(op,&cur)) >= 0) ylikely {
	    msfile_ent	be ;
	    while ((rs1 = msfile_curenum(op,&cur,&be)) >= 0) {
		if (isEnabled(be.flags)) {
		    rs = procent(&be) ;
		}
		if (rs < 0) break ;
	    } /* end if (msfile_magic) */
	    if ((rs >= 0) && (rs1 < 0) && (rs1 != rsn)) rs = rs1 ;
	    rs1 = msfile_curend(op,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msfile-cursor) */
	return (rs >= 0) ? cnt : rs ;
} /* end subroutine (msfile_best) */

int bester::procent(msfile_ent *bep) noex {
    	int		rs = SR_OK ;
	double valcapacity	= double(bep->speed * bep->ncpu) ;
	double valused		= (double(bep->la[0]) / FSCALE) ;
	double valempty		= (double(bep->ncpu) - valused) ;
	valused			*= double(minspeed) ;
	double valavail		= (valcapacity - valused) ;
	if (isEnabled(bep->flags) && (valempty > 0.0)) ylikely {
	    cnt += 1 ;
	    if (valavail > valavailbest) {
	        valavailbest = valavail ;
	        memcopy(ep,bep) ;
	    } /* end if (better) */
	} /* end if (got one) */
	return rs ;
} /* end method (bester::procent) */


