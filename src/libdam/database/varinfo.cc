/* varinfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* VARINFO management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	varinfo_dat

	Description:
	This module implements an interface (a trivial one) that
	provides access to the VAR object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mkfname.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU*/

#include	"varinfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	INDSUF		"vi"


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int varinfo(varinfo_dat *vip,cchar *dbnp,int dbnl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (vip && dbnp) ylikely {
	    rs = SR_INVALID ;
	    memclear(vip) ;
	    if (dbnp[0]) ylikely {
	        cchar	*nam ;
	        if (nulstr ns ; (rs = ns.start(dbnp,dbnl,&nam)) >= 0) {
	            cchar	*strend = ENDIANSTR ;
	            if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	                if ((rs = mkfnamesuf2(tbuf,nam,INDSUF,strend)) >= 0) {
	                    if (ustat sb ; (rs = u_stat(tbuf,&sb)) >= 0) {
		                vip->fsize = sb.st_size ;
		                vip->mtime = sb.st_mtime ;
		            }
	                }
	                rs1 = mem.free(tbuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	            rs1 = ns.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (varinfo) */

int varunlink(cchar *dbnp,int dbnl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbnp) {
	    rs = SR_INVALID ;
	    if (dbnp[0]) {
	        cchar	*nam ;
	        if (nulstr ns ; (rs = ns.start(dbnp,dbnl,&nam)) >= 0) {
	            cchar	*strend = ENDIANSTR ;
	            if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	                if ((rs = mkfnamesuf2(tbuf,nam,INDSUF,strend)) >= 0) {
		            rs = uc_unlink(tbuf) ;
	                }
	                rs1 = mem.free(tbuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	            rs1 = ns.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (varunlink) */


