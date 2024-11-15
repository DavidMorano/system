/* localsetnetload SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* set the LOCAL network-load (NETLOAD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	localsetnetload

	Description:
	This subroutine sets the LOCAL system network-load.

	Synopsis:
	int localsetnetload(cchar *pr,cchar *sbuf,int slen) noex

	Arguments:
	pr		program root
	sbuf		c-string value to set pointer
	slen		c-string value to set length

	Returns:
	>=0		OK
	<0		error (system-return)


	Notes:

	Q. Why the program-cache?
	A. It serves as a short-cut for future "gets."

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<ucprogdata.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"localset.h"


/* local defines */

#define	ETCDNAME	"etc"
#define	VARDNAME	"var"
#define	NETLOADFNAME	"netload"
#define	TO_TTL		(5*60)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr char		filetitle[] = "# NETLOAD (Machine Network-Load)" ;


/* exported variables */


/* exported subroutines */

int localsetnetload(cchar *pr,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (pr && sbuf) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        cint		di = UCPROGDATA_DNETLOAD ;
	        cint		ttl = TO_TTL ;
	        cchar		*vardname = VARDNAME ;
	        cchar		*netloadname = NETLOADFNAME ;
	        if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	            if ((rs = mkpath(tbuf,pr,vardname,netloadname)) >= 0) {
	                cmode	om = 0664 ;
	                if (bfile df ; (rs = bopen(&df,tbuf,"wct",om)) >= 0) {
		            cchar	*fmt = filetitle ;
		            if ((rs = bprintln(&df,fmt,-1)) >= 0) {
	                        if ((rs = bprintln(&df,sbuf,slen)) >= 0) {
		                    rs = ucprogdata_set(di,sbuf,slen,ttl) ;
			            rc = rs ;
		                }
		            }
		            rs1 = bclose(&df) ;
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
/* end subroutine (localsetnetload) */


