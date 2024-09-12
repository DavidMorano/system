/* localsetsystat SUPPORT */
/* lang=C++20 */

/* set the LOCAL system-status (SYSTAT) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	localsetsystat

	Description:
	This subroutine sets the LOCAL system-status (SYSTAT).

	Synopsis:
	int localsetsystat(cchar *pr,cchar *sbuf,int slen) noex

	Arguments:
	pr		program root
	sbuf		caller supplied buffer w/ value to set
	slen		length of caller supplied buffer

	Returns:
	>=0		OK
	<0		error (system-return)


	Notes:

	Q. Why the program-cache?
	A. It serves as a short-cut for future "gets."

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<ucprogdata.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"localset.h"


/* local defines */

#ifndef	VARSYSTAT
#define	VARSYSTAT	"SYSTAT"
#endif

#define	ETCDNAME	"etc"
#define	VARDNAME	"var"
#define	SYSTATFNAME	"systat"
#define	TO_TTL		(5*60)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cchar		filetitle[] = "# SYSTAT (Machine System-Status)" ;


/* exported variables */


/* exported subroutines */

int localsetsystat(cchar *pr,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (pr && sbuf) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        cint	di = UCPROGDATA_DSYSTAT ;
	        cint	ttl = TO_TTL ;
	        cchar	*vardname = VARDNAME ;
	        cchar	*name = SYSTATFNAME ;
	        char	*tbuf{} ;
	        if ((rs = malloc_mp(&tbuf)) >= 0) {
	            if ((rs = mkpath(tbuf,pr,vardname,name)) >= 0) {
	                bfile	dfile, *dfp = &dfile ;
	                cmode	om = 0664 ;
	                if ((rs = bopen(dfp,tbuf,"wct",om)) >= 0) {
		            cchar	*fmt = filetitle ;
		            if ((rs = bprintln(dfp,fmt,-1)) >= 0) {
	                        if ((rs = bprintln(dfp,sbuf,slen)) >= 0) {
		                    rs = ucprogdata_set(di,sbuf,slen,ttl) ;
			            rc = rs ;
		                }
		            }
		            rs1 = bclose(dfp) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (file-write) */
	            } /* end if (mkpath) */
	            rs1 = uc_free(tbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (localsetsystat) */


