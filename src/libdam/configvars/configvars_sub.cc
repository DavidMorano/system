/* configvars_sub SUPPORT */
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

#undef	BUFLEN
#define	BUFLEN		(LINEBUFLEN * 2)

#define	KEYBUFLEN	20

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


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
		 char *kp,int kl,char *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		idx = 0 ;
	if (cvp && kp) {
	    CV_VAR	v{} ;
	    cint	vsz = sizeof(CV_VAR) ;
	    if ((rs = var_start(&v,fi,kp,kl,vp,vl)) >= 0) {
	        vecobj		*slp = (w) ? cvp->setp : cvp->varp ;
	        if ((rs = vecobj_add(slp,&v,vsz)) >= 0) {
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
	    	        CV_VAR	*cep = (CF_VAR *) vp ;
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
	    	    CV_VAR	*cfp = (CF_VAR *) vp ;
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


