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
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<new>			/* |nothrow(3c++)| */
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

#include	"configvarsobj.hh"


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


/* local variables */

enum configkeys {
	configkey_define,
	configkey_export,
	configkey_set,
	configkey_unset,
	configkey_overlast
} ;

constexpr cpcchar	configkeys[] = {
	"define",
	"export",
	"set",
	"unset",
	nullptr
} ;

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
	int		idx = 0 ;
	if (cvp && kp) {
	    CV_VAR	v{} ;
	    if ((rs = var_start(&v,fi,kp,kl,vp,vl)) >= 0) {
	        vecobj		*slp = (w) ? cvp->setp : cvp->varp ;
	        if ((rs = vecobj_add(slp,&v)) >= 0) {
		    idx = rs ;
		}
	        if (rs < 0) {
	            var_finish(&v) ;
	        }
	    } /* end if (var-) */
	} /* end if (non-null) */
	return (rs >= 0) ? idx : rs ;
    } /* end subroutine (configvars_addvar) */

    int configvars_finvars(CV *cvp) noex {
	int	rs = SR_OK ;
	int	rs1 ;
	for (int j = 0 ; j < 5 ; j += 1) {
	    vecobj	*slp{} ;
	    switch (j) {
	    case vartype_set: slp = cvp->setp ; break ;
	    case vartype_var: slp = cvp->varp ; break ;
	    case vartype_export: slp = cvp->expp ; break ;
	    case vartype_define: slp = cvp->defp ; break ;
	    case vartype_unset: slp = cvp->unvp ; break ;
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


