/* configvars_var SUPPORT */
/* lang=C++20 */

/* Configuration-Variables */
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
#define	CONFxIGVARS_WSETS	0
#define	CONFxIGVARS_WVARS	1

#define	CV		configvars
#define	CV_VAR		configvars_var
#define	CV_FILE		configvars_file
#define	CV_CUR		configvars_cur
#define	CV_ERR		configvars_err


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


/* local variables */


/* exported variables */


/* exported subroutines */

namespace configvars_obj {
    int var_start(CV_VAR *cep,int fi,char *kp,int kl,char *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (cep && kp) {
	    int		len{} ;
	    char	*vb{} ;
	    memclear(cep) ;
	    if (kl < 0) kl = strlen(kp) ;
	    if (vp) {
	        if (vl < 0) vl = strlen(vp) ;
	    } else {
		vl = 0 ;
	    }
	    len = (kl + vl + 2) ;
	    if ((rs = uc_malloc(len,&vb)) >= 0) {
	        char	*cp = (strwcpy(vb,kp,kl) + 1) ;
	        cep->fi = fi ;
	        cep->key = vb ;
	        cep->klen = kl ;
	        cep->value = nullptr ;
	        cep->vlen = 0 ;
	        if (vp) {
	            strwcpy(cp,vp,vl) ;
	            cep->value = cp ;
	            cep->vlen = vl ;
	        } else {
	            *cp = '\0' ;
	        }
	        cep->fmask = 0 ;
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (var_start) */
} /* end namespace (configvars_obj) */

namespace configvars_obj {
    int var_finish(CV_VAR *cep) noex {
	int		rs = SR_FAULT ;
	if (cep) {
	    rs = SR_OK ;
	    freeit(&cep->key) ;
	}
	return rs ;
    } /* end subroutine (var_finish) */
} /* end namespace (configvars_obj) */


