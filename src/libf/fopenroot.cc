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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* STC-C */
#include	<cstdlib>		/* STC-C */
#include	<cstdio>		/* STC-C */
#include	<cstring>		/* STD-C |strcpy(3c)| */
#include	<new>			/* C++CTD placement-new */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<ubufdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"libf.h"

import libfsup ;
import libfmisc ;			/* |mknpath(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local structures */


/* local variables */

cint		maxpathlen = MAXPATHLEN ;


/* exported variables */


/* exported subroutines */

FILE *fopenroot(cchar *pr,cchar *fn,char *outname,cchar *mstr) noex {
	FILE		*fp = nullptr ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (mstr == nullptr) {
	    mstr = "r" ;
	}
	if (pr && fn) ylikely {
	    rs = SR_INVALID ;
	    outname[0] = '\0' ;
	    if (fn[0] && mstr[0]) ylikely {
		if ((rs = maxpathlen) >= 0) ylikely {
		    cint plen = rs ;
		    rs = SR_NOMEM ;
		    if (char *pbuf = new(nt) char [plen + 1] ; pbuf) ylikely {
	    	        if ((rs = mknpath(pbuf,plen,pr,fn)) >= 0) {
			    fp = fopen(pbuf,mstr) ;
		            if (outname) {
			        rs = mknpath(outname,plen,pbuf) ;
		            }
			    if (rs < 0) {
				fclose(fp) ;
				fp = nullptr ;
			    } /* end if (error) */
		        } /* end if (mknpath) */
			delete [] pbuf ;
		    } /* end if (m-a-f) */
		} /* end if (maxpathlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	if ((rs < 0) && fp) {
	    fclose(fp) ;
	    fp = nullptr ;
	} /* end if (error) */
	return fp ;
} /* end subroutine (fopenroot) */


