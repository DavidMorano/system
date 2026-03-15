/* fopenroot SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open a file name according to rules */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This was straightforwardly adapted from |bopenroot(3b)|.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

  	Name:
	fopenroot

	Description:
	This subroutine will form a file name according to some
	rules.

	The rules are roughly:
	+ attempt to open it directly if it is already rooted
	+ open it if it is already in the root area
	+ attempt to open it as it is if it already exists
	+ attempt to open or create it located in the root area
	+ attempt to open or create it as it is

	Synopsis:
	FILE *fopenroot(cc *pr,cc *fname,char *outname,cc *mstr) noex

	Arguments:
	+ pr		path of program root directory
	+ fname		fname to find and open
	+ outname	user supplied buffer to hold resulting name
	+ mstr		mode-string

	Returns:
	non-null	success (pointer to FILE obect)
	null		failure

	outname		1. zero length string if no new name was needed
			2. will contain the path of the file that was opened

*****************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>
#include	<cstdlib>
#include	<cstdio>
#include	<cstring>		/* strcpy(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<umem.hh>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG */

#include	"libf.h"

#pragma		GCC dependency		"mod/ulibvals.ccm"
#pragma		GCC dependency		"mod/umisc.ccm"

import ulibvals ;
import umisc ;				/* |mknpath(3u)| */

/* local defines */


/* imported namespaces */

using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local structures */


/* local variables */

cint		maxpathlen = ulibval.maxpathlen ;


/* exported variables */


/* exported subroutines */

FILE *fopenroot(cchar *pr,cchar *fn,char *outname,cchar *mstr) noex {
	FILE	*fp = nullptr ;
	int	rs = SR_FAULT ;
	int	rs1 ;
	if (mstr == nullptr) {
	    mstr = "r" ;
	}
	if (pr && fn) {
	    rs = SR_INVALID ;
	    if (fn[0] && mstr[0]) {
		if ((rs = maxpathlen) >= 0) {
		    cint plen = rs ;
		    if (char *pbuf ; (rs = umem.mall((plen + 1),&pbuf)) >= 0) {
	    	        if ((rs = mknpath(pbuf,plen,pr,fn)) >= 0) {
			    fp = fopen(pbuf,mstr) ;
		        }
		        rs1 = umem.free(pbuf) ;
		        if (rs >= 0) rs = rs1 ;
		        if (outname) {
			    strcpy(outname,pbuf) ;
		        }
		    } /* end if (m-a-f) */
		} /* end if (maxpathlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	if ((rs < 0) && fp) {
	    fclose(fp) ;
	    fp = nullptr ;
	} /* end if (error) */
	return fp ;
}
/* end subroutine (fopenroot) */


