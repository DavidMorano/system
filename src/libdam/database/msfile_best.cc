/* msfile_best SUPPORT */
/* lang=C++20 */

/* find the "best" machine entry */
/* version %I% last-modified %G% */

#define	CF_GETEXECNAME	1		/* get the 'exec(2)' name */
#define	CF_FLOAT	0		/* use floating point */
#define	CF_CPUSPEED	0		/* no use here */

/* revision history:

	= 2004-01-10, David A�D� Morano
	:-) It cracks me up how I take one program and make another
	from it! :-) This program is now a built-in command (MSU)
	to the KSH program to update the machine status for the
	current node in the cluster. [added for program checking
	-- ((]

	= 2004-01-12, David A�D� Morano
	This program is now the MSINFO KSH built-in command.

*/

/* Copyright � 2004 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine provides an additional method to the MSFILE
	object. It finds the best of the entries given a couple of
	optional restrictions.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<climits>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<baops.h>
#include	<vecobj.h>
#include	<mallocstuff.h>
#include	<localmisc.h>

#include	"msfile.h"
#include	"msflag.h"


/* local defines */

#ifndef	MSFLAG_DISABLED
#define	MSFLAG_DISABLED		0x0001
#endif


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int msfile_best(MSFILE *msp,time_t daytime,int flags,MSFILE_ENT *ep) noex {
	MSFILE_CUR	cur ;
	MSFILE_ENT	be ;
	double		capacity, used ;
	double		bestavail, avail, empty ;
	cint		nl = MSFILE_NODENAMELEN ;
	int		rs ;
	int		rs1 ;
	int		minspeed = INT_MAX ;
	int		c = 0 ;

	bestavail = -1000.0 ;

/* first find the minimum speed of all nodes */

	if ((rs = msfile_curbegin(msp,&cur)) >= 0) {

	while (rs >= 0) {
	    rs1 = msfile_enum(msp,&cur,&be) ;
	    if (rs1 == SR_NOTFOUND) break ;
	    rs = rs1 ;

	    if ((rs >= 0) && (be.speed != 0)) {
		c += 1 ;
	        if (be.speed < minspeed) minspeed = be.speed ;
	    }

	} /* end while */

	rs1 = msfile_curend(msp,&cur) ;
	if (rs >= 0) rs = rs1 ;
	} /* end if *cursor) */

/* then find the one with the most available computation */

	if ((rs >= 0) && (c > 0)) {

	    c = 0 ;
	    if ((rs = msfile_curbegin(msp,&cur)) >= 0) {

	    while (rs >= 0) {
	        rs1 = msfile_enum(msp,&cur,&be) ;
		if (rs1 == SR_NOTFOUND) break ;
		rs = rs1 ;

		if (rs >= 0) {

#ifdef	COMMENT
		if (be.flags & MSFLAG_DISABLED)
			continue ;
#endif

	        capacity = (double) (be.speed * be.ncpu) ;
	        used = ((double) be.la[0]) / FSCALE ;
		empty = ((double) be.ncpu) - used ;
	        used *= ((double) minspeed) ;

	        avail = capacity - used ;

		if (((! (flags & 1)) || (! (be.flags & MSFLAG_DISABLED))) &&
			((! (flags & 2)) || (empty > 0.0))) {

	            c += 1 ;

	        if (avail > bestavail) {
	            bestavail = avail ;
	            memcpy(ep,&be,sizeof(MSFILE_ENT)) ;
	        } /* end if (better) */

		} /* end if (found one at all) */

		} /* end if */

	    } /* end while */

	    rs1 = msfile_curend(msp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */

	} /* end if */

	if (c == 0) rs = SR_NOENT ;

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (msfile_best) */


