/* cm_vars SUPPORT (Communications-Connection Manager) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Connection Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cm

	Description:
	This is a communications connection manager object.  This
	object abstracts the details of a particular connection
	from the calling program.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */

#include	"cm_defs.h"
#include	"cm_vars.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

namespace cm_ns {
    vars		var ;
    char		terms[fieldterms_termsize] ;
} /* end namespace */


/* exported subroutines */

namespace cm_ns {
    vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mn)) >= 0) ylikely {
	    maxnamelen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
		maxpathlen = rs ;
	    	if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
		    usernamelen = rs ;
	    	    if ((rs = bufsizeget(bufsize_nn)) >= 0) ylikely {
			nodenamelen = rs ;
	    	    	if ((rs = bufsizeget(bufsize_hostname)) >= 0) ylikely {
			    hostnamelen = rs ;
	    	    	    if ((rs = bufsizeget(bufsize_ma)) >= 0) ylikely {
				maxarg = rs ;
				rs = fieldterms(terms,0," \t") ;
			    } /* end if */
			}
		    }
		}
	    }
	} /* end if */
	return rs ;
    } /* end method (vars::operator) */
} /* end namespace (cm_ns) */


