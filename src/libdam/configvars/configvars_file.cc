/* configvars SUPPORT */
/* lang=C++20 */

/* Configuration-Variables */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

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

#include	"configvarsobj.hh"


/* local defines */

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

    int file_start(CV_FILE *cfp,cchar *filename) noex {
	int		rs = SR_FAULT ;
	if (cfp && filename) {
	    memclear(cfp) ;
	    rs = SR_NOMEM ;
	    if ((cfp->filename = mallocstr(filename)) != nullptr) {
	        vecobj		*vip{} ;
		cint		vsz = sizeof(CV_VAR) ;
		cint		vn = 0 ;
		cint		vo = 0 ;
	        vip = &cfp->defines ;
	        if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
	            vip = &cfp->exports ;
	            if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
	                vip = &cfp->unsets ;
		        if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
			    rs = strlen(filename) ;
		        }
		        if (rs < 0) {
			    vecobj_finish(&cfp->exports) ;
		        }
		    } /* end if (exports) */
		    if (rs < 0) {
		        vecobj_finish(&cfp->defines) ;
		    }
	        } /* end if (defines) */
	        if (rs < 0) {
		    uc_free(cfp->filename) ;
		    cfp->filename = nullptr ;
	        }
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
	    rs1 = uc_free(cfp->filename) ;
	    if (rs >= 0) rs = rs1 ;
	    cfp->filename = nullptr ;
	}
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
	        if (vp) {
	    	    CV_VAR	*vep = (CV_VAR *) vp ;
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
		char *key,int klen,char *value,int vlen) noex {
	CV_VAR		ve{} ;
	int		rs ;
	if ((rs = var_start(&ve,fi,key,klen,value,vlen)) >= 0) {
	    vecobj	*vlp{} ;
	    switch (type) {
	    case vartype_define: vlp = &cfp->defines ; break ;
	    case vartype_export: vlp = &cfp->exports ; break ;
	    case vartype_unset: vlp = &cfp->unsets ; break ;
	    default: rs = SR_BUGCHECK ; break ;
	    } /* end switch */
	    if ((rs >= 0) && vlp) {
	        rs = vecobj_add(vlp,&ve) ;
	    }
	    if (rs < 0) {
	        var_finish(&ve) ;
	    }
	} /* end if (var_start) */
	return rs ;
    } /* end subroutine (file_addvar) */

} /* end namespace (configvars_obj) */


