/* configvars_sub SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Configuration-Variables - Subsidiary-Functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	configvars_sub

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
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"configvars.h"
#include	"configvars_util.hh"

import configvars_util ;

/* local defines */


/* local namespaces */

using namespace		configvars_obj ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

enum configkeys {
	configkey_define,
	configkey_export,
	configkey_set,
	configkey_unset,
	configkey_overlast
} ; /* end enum */

constexpr cpcchar	confignames[] = {
	"define",
	"export",
	"set",
	"unset",
	nullptr
} ; /* end array */

enum vartypes {
	vartype_set,
	vartype_var,
	vartype_export,
	vartype_define,
	vartype_unset,
	vartype_overlast
} ; /* end enum (vartypes) */


/* exported variables */


/* exported subroutines */

namespace configvars_obj {
    int configvars_addvar(CV *cvp,int fi,int w,
		 cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		idx = 0 ; /* return-value */
	if (cvp && kp) ylikely {
	    CV_VAR	v{} ;
	    if ((rs = var_start(&v,fi,kp,kl,vp,vl)) >= 0) ylikely {
	        vecobj	*slp = (w) ? cvp->setp : cvp->varp ;
	        if ((rs = vecobj_add(slp,&v)) >= 0) ylikely {
		    idx = rs ;
		} /* end if (ok) */
	        if (rs < 0) {
	            var_finish(&v) ;
	        } /* end if (error) */
	    } /* end if (var-) */
	} /* end if (non-null) */
	return (rs >= 0) ? idx : rs ;
    } /* end subroutine (configvars_addvar) */
} /* end namespace (configvars_obj) */

namespace configvars_obj {
    int configvars_finvars(CV *cvp) noex {
	int	rs = SR_OK ;
	int	rs1 ;
	for (int j = 0 ; j < 5 ; j += 1) {
	    vecobj	*slp{} ;
	    switch (j) {
	    case vartype_set:		slp = cvp->setp ; break ;
	    case vartype_var:		slp = cvp->varp ; break ;
	    case vartype_export:	slp = cvp->expp ; break ;
	    case vartype_define:	slp = cvp->defp ; break ;
	    case vartype_unset:		slp = cvp->unvp ; break ;
	    } /* end switch */
	    if (slp) {
		void	*vp{} ;
	        for (int i = 0 ; vecobj_get(slp,i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	    	        CV_VAR	*cep = (CV_VAR *) vp ;
	                rs1 = var_finish(cep) ;
		        if (rs >= 0) rs = rs1 ;
	 	    }
	        } /* end for */
	        {
	            rs1 = vecobj_finish(slp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end if (slp) */
	} /* end for */
	return rs ;
    } /* end subroutine (configvars_finvars) */
} /* end namespace (configvars_obj) */

namespace configvars_obj {
    int configvars_finfiles(CV *cvp) noex {
	vecobj		*slp = cvp->fesp ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(slp,i,&vp) >= 0 ; i += 1) {
	        if (vp) {
	    	    CV_FILE	*cfp = (CV_FILE *) vp ;
	            rs1 = file_finish(cfp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	}
	{
	    rs1 = vecobj_finish(slp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
    } /* end subroutine (configvars_finfiles) */
} /* end namespace (configvars_obj) */


