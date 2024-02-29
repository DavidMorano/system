/* localgetsystat SUPPORT */
/* lang=C++20 */

/* get the LOCAL system-status (SYSTAT) */
/* version %I% last-modified %G% */

#define	CF_UPROGDATA	1		/* use |uprogdata_xxx(3uc)| */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	localgetsystat

	Description:
	This subroutine retrieves the LOCAL system-status (SYSTAT).

	Synopsis:
	int localgetsystat(cchar *pr,char *rbuf,int rlen) noex

	Arguments:
	pr		program root
	rbuf		caller supplied buffer to place result in
	rlen		length of caller supplied buffer

	Returns:
	>=0		length of returned value
	<0		error (system-return)

	Notes:

	Q. Why the program-cache?
	A. Because this subroutine, and a couple others like it,
	get called everytime certain pseudo-"files" are read out.
	We want some of those files to read out very quickly, so
	caching away an extra real-file read in this routine (and
	others like it) really speeds things up.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<uprogdata.h>
#include	<mallocxx.h>
#include	<strlibval.hh>
#include	<filereadln.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"localget.h"


/* local defines */

#ifndef	VARSYSTAT
#define	VARSYSTAT	"SYSTAT"
#endif

#define	ETCDNAME	"etc"
#define	VARDNAME	"var"
#define	SYSTATFNAME	"systat"
#define	TO_TTL		(5*60)

#ifndef	CF_UPROGDATA
#define	CF_UPROGDATA	1
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_uprogdata = CF_UPROGDATA ;

constexpr cchar		ssn[] = SYSTATFNAME ;


/* exported variables */


/* exported subroutines */

int localgetsystat(cchar *pr,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        cint		di = UPROGDATA_DSYSTAT ;
	        cint		ttl = TO_TTL ;
		rs = SR_OK ;
	        rbuf[0] = '\0' ;
/* user environment */
	        if ((rs >= 0) && (len == 0)) {
	            static cchar	*systat = getenv(VARSYSTAT) ;
	            if (systat && (systat[0] != '\0')) {
	                rs = sncpy1(rbuf,rlen,systat) ;
	                len = rs ;
	            }
	        } /* end if (needed) */
/* program cache */
	        if constexpr (f_uprogdata) {
	            if ((rs >= 0) && (len == 0)) {
	                if ((rs = uprogdata_get(di,rbuf,rlen)) > 0) {
	                    len = rs ;
	                }
	            }
	        } /* end if-constexpr (f_uprogdata) */
/* software facility (LOCAL) configuration */
	        if ((rs >= 0) && (len == 0)) {
	            cchar	*vardname = VARDNAME ;
	            char	*tfname{} ;
		    if ((rs = malloc_mp(&tfname)) >= 0) {
	                if ((rs = mkpath3(tfname,pr,vardname,ssn)) >= 0) {
	                    if ((rs = filereadln(tfname,rbuf,rlen)) > 0) {
	                        len = rs ;
		                if constexpr (f_uprogdata) {
	                            rs = uprogdata_set(di,rbuf,len,ttl) ;
		                }
		            } else if (isNotPresent(rs)) {
		                rs = SR_OK ;
		            }
	                } /* end if (mkpath) */
			rs1 = uc_free(tfname) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (needed) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetsystat) */


