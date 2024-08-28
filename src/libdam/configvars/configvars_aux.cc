/* configvars_aux SUPPORT */
/* lang=C++20 */

/* Configuration-Variables Auxillary */
/* version %I% last-modified %G% */

#define	CF_ALLOCFILENAME	1

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an object that reads configuration files and organizes
	the content into the object for structured access.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<usupport.h>
#include	<mallocstuff.h>
#include	<bfile.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<field.h>
#include	<matostr.h>
#include	<localmisc.h>

#include	"configvars.h"


/* local defines */


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine (template) */
using std::max ;			/* subroutine (template) */
using std::nothrow ;			/* constant */

using namespace		configvars_obj ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

namespace configvars_obj {

    void freeit(char **vp) noex {
	if (*vp) {
	    uc_free(*vp) ;
	    *vp = nullptr ;
	}
    } /* end subroutine (freeit) */

    void badline(vecobj *eep,char *fname,int line) noex {
	if (eep && fname) {
	    configvars_err	e ;
	    e.line = line ;
	    strwcpy(e.filename,fname,MAXPATHLEN) ;
	    if (eep) {
		[[maybe_unused]] cint esz = sizeof(struct configvars_errline) ;
	        (void) vecobj_add(eep,&e) ;
	    }
	} /* end if (non-null) */
    } /* end subroutine (badline) */

} /* end namespace (configvars_obj) */


