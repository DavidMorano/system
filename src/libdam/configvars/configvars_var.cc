/* configvars_var SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Configuration-Variables */
/* version %I% last-modified %G% */

#define	CF_ALLOCFILENAME	1

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	congivars_var

	Description:
	This is an object that reads configuration files and organizes
	the content into the object for structured access.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC[ */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"configvars_util.hh"
#include	"configvars.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import configvars_util ;

/* local namespaces */

using namespace		configvars_obj ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local void	freeit(char **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

namespace configvars_obj {
    int var_start(CV_VAR *cep,int fi,cc *kp,int kl,cc *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (cep && kp) ylikely {
	    int		len{} ;
	    memclear(cep) ;
	    if (kl < 0) kl = lenstr(kp) ;
	    if (vp) {
	        if (vl < 0) vl = lenstr(vp) ;
	    } else {
		vl = 0 ;
	    }
	    len = (kl + vl + 2) ;
	    if (char *vb ; (rs = lm_mall(len,&vb)) >= 0) ylikely {
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
	if (cep) ylikely {
	    rs = SR_OK ;
	    freeit(&cep->key) ;
	}
	return rs ;
    } /* end subroutine (var_finish) */
} /* end namespace (configvars_obj) */

local void freeit(char **vp) noex {
	if (*vp) ylikely {
	    lm_free(*vp) ;
	    *vp = nullptr ;
	}
} /* end subroutine (freeit) */


