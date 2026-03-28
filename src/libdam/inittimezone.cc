/* inittimezone SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the default timezone ('TZ') that |init(1m)| uses */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-05-16, David A­D­ Morano
	This code was originally written for use in the PCS facility.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	inittimezone

	Description:
	This subroutine retrieves (if it exists) the timezone
	variable value (value name is 'TZ') that the |init(1m)|
	program sets for its children.

	Synopsis:
	int inittimezone(char *rbuf,int rlen,cchar *fname) noex

	Arguments:
	rbuf		buffer to receive result
	rlen		length of supplied buffer to receive result
	fname		the 'init(1m)' configuration filename

	Returns:
	>=0		length of returned data
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecstr.h>
#include	<vstrkeycmp.h>		/* |vstrkeycmp(3uc)| */
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"inittimezone.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	DEFINITFNAME
#define	DEFINITFNAME	"/etc/default/init"
#endif

#ifndef	VARTZ
#define	VARTZ		"TZ"
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int inittimezone(char *rbuf,int rlen,cchar *fname) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (rbuf) {
	    rs = SR_OVERFLOW ;
	    rbuf[0] = '\0' ;
	    if (rs > 0) {
		cint vn = 4 ;
		cint vo = 0 ;
	        if (fname == nullptr) {
	    	    fname = DEFINITFNAME ;
	        }
	        if (vecstr defs ; (rs = defs.start(vn,vo)) >= 0) {
	            if ((rs = defs.envfile(fname)) >= 0) {
			cchar	*var = varname.tz ;
			cchar	*sp ;
	        	if ((rs = defs.finder(var,vstrkeycmp,&sp)) >= 0) {
	                    if (sp && sp[0]) {
			        if (cchar *tp ; (tp = strchr(sp,'=')) != np) {
	                           rs = sncpy(rbuf,rlen,(tp+1)) ;
			           len = rs ;
			       }
	                    } else {
	                	rs = SR_NOTFOUND ;
			    }
	        	} /* end if (found our key-name) */
	    	    } /* end if (got some variables) */
	            rs1 = vecstr_finish(&defs) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (vecstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (inittimezone) */


