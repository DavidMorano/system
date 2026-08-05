/* configvars_file SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Configuration-Variables */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	configvars_file

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
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"configvarsobj.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */

using std::min ;			/* subroutine (template) */
using std::max ;			/* subroutine (template) */

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

    int file_start(CV_FILE *cfp,cchar *fn) noex {
	int		rs = SR_FAULT ;
	if (cfp && fn) ylikely {
	    memclear(cfp) ;
	    if (cchar *cp ; (rs = lm_strw(fn,-1,&cp)) >= 0) ylikely {
	        vecobj		*vip = &cfp->defines ;
		cint		vsz = szof(CV_VAR) ;
		cint		vn = 0 ;
		cint		vo = 0 ;
	        cfp->filename = cp ;
	        if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) ylikely {
	            vip = &cfp->exports ;
	            if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) ylikely {
	                vip = &cfp->unsets ;
		        if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) ylikely {
			    rs = lenstr(fn) ;
		        } /* end if (ok) */
		        if (rs < 0) {
			    vecobj_finish(&cfp->exports) ;
		        } /* end if (error) */
		    } /* end if (exports) */
		    if (rs < 0) {
		        vecobj_finish(&cfp->defines) ;
		    } /* end if (error) */
	        } /* end if (defines) */
	        if (rs < 0) {
		    void *vp = voidp(cfp->filename) ;
		    lm_free(vp) ;
		    cfp->filename = nullptr ;
	        } /* end if (error) */
	    } /* end if (filename) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (file_start) */

    int file_finish(CV_FILE *cfp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < 3 ; i += 1) {
	    rs1 = file_finone(cfp,i) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end for */
	if (cfp->filename) {
	    void *vp = voidp(cfp->filename) ;
	    rs1 = lm_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    cfp->filename = nullptr ;
	} /* end if (memory-release) */
	return rs ;
    } /* end subroutine (file_finish) */

    int file_finone(CV_FILE *cfp,int type) noex {
	vecobj		*vip{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	switch (type) {
	case 0: vip = &cfp->defines ; break ;
	case 1: vip = &cfp->exports ; break ;
	case 2: vip = &cfp->unsets ; break ;
	default: rs = SR_BUGCHECK ; break ;
	} /* end switch */
	if ((rs >= 0) && vip) {
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(vip,i,&vp) >= 0 ; i += 1) {
	    	CV_VAR	*vep = (CV_VAR *) vp ;
	        if (vp) {
	            rs1 = var_finish(vep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    {
	        rs1 = vecobj_finish(vip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (ok) */
	return rs ;
    } /* end subroutine (file_finone) */

    int file_addvar(CV_FILE *cfp,int type,int fi,
		cc *key,int klen,cc *value,int vlen) noex {
	CV_VAR		ve{} ;
	int		rs ;
	if ((rs = var_start(&ve,fi,key,klen,value,vlen)) >= 0) {
	    vecobj	*vlp{} ;
	    switch (type) {
	    case keytype_define: vlp = &cfp->defines ; break ;
	    case keytype_export: vlp = &cfp->exports ; break ;
	    case keytype_unset: vlp = &cfp->unsets ; break ;
	    default: rs = SR_BUGCHECK ; break ;
	    } /* end switch */
	    if ((rs >= 0) && vlp) {
	        rs = vecobj_add(vlp,&ve) ;
	    } /* end if (ok) */
	    if (rs < 0) {
	        var_finish(&ve) ;
	    } /* end if (error) */
	} /* end if (var_start) */
	return rs ;
    } /* end subroutine (file_addvar) */

} /* end namespace (configvars_obj) */


