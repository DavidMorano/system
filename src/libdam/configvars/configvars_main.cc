/* configvars_main SUPPORT */
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

template<typename ... Args>
static inline int configvars_ctor(configvars *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->checktime = time(nullptr) ;
	    if ((op->fesp = new(nothrow) vecobj) != np) {
		if ((op->varp = new(nothrow) vecobj) != np) {
	            if ((op->defp = new(nothrow) vecobj) != np) {
	                if ((op->setp = new(nothrow) vecobj) != np) {
	                    if ((op->expp = new(nothrow) vecobj) != np) {
	            		if ((op->unvp = new(nothrow) vecobj) != np) {
				    rs = SR_OK ;
	                        } /* end if (new-unv) */
			        if (rs < 0) {
				    delete op->expp ;
				    op->expp = np ;
			        }
	                    } /* end if (new-exp) */
			    if (rs < 0) {
				delete op->setp ;
				op->setp = np ;
			    }
	                } /* end if (new-set) */
		        if (rs < 0) {
			    delete op->defp ;
			    op->defp = np ;
		        }
	            } /* end if (new-def) */
		    if (rs < 0) {
			delete op->varp ;
			op->varp = np ;
		    }
	        } /* end if (new-var) */
		if (rs < 0) {
		    delete op->fesp ;
		    op->fesp = np ;
		}
	    } /* end if (new-fes) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (configvars_ctor) */

static int configvars_dtor(configvars *op) noex {
	cnullptr	np{} ;
	int		rs = SR_OK ;
	if (op->unvp) {
	    delete op->unvp ;
	    op->unvp = np ;
	}
	if (op->expp) {
	    delete op->expp ;
	    op->expp = np ;
	}
	if (op->setp) {
	    delete op->setp ;
	    op->setp = np ;
	}
	if (op->defp) {
	    delete op->defp ;
	    op->defp = np ;
	}
	if (op->varp) {
	    delete op->varp ;
	    op->varp = np ;
	}
	if (op->fesp) {
	    delete op->fesp ;
	    op->fesp = np ;
	}
	return rs ;
}
/* end subroutine (configvars_dtor) */


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

int configvars_open(configvars *cvp,cchar *cfn,vecobj *eep) noex {
	int		rs ;
	if ((rs = configvars_ctor(cvp,cfn)) >= 0) {
	    vecobj	*vip ;
	    cint	vn = 10 ;
	    int		vsz ;
	    int		vo = 0 ;
	    vip = cvp->fesp ;
	    vsz = sizeof(CV_FILE) ;
	    if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
	  	vsz = sizeof(CV_VAR) ;
	        vip = cvp->varp ;
	        if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
	      	    vip = cvp->defp ;
	            if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
	                vip = cvp->setp ;
		        if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
	            	    vip = cvp->expp ;
			    vo = VECOBJ_OSORTED ;
			    if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
	                        vip = cvp->unvp ;
				if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
				    cvp->magic = CONFIGVARS_MAGIC ;
				    if (cfn && cfn[0]) {
					auto ca = configvars_addfile ;
	    				if ((rs = ca(cvp,cfn,eep)) >= 0) {
					    rs = SR_OK ;
					}
				    }
				    if (rs < 0) {
				        vecobj_finish(cvp->unvp) ;
				    }
				} /* end if (unv) */
				if (rs < 0) {
			            vecobj_finish(cvp->expp) ;
				}
			    } /* end if (exp) */
			    if (rs < 0) {
				vecobj_finish(cvp->setp) ;
			    }
			} /* end if (set) */
			if (rs < 0) {
			    vecobj_finish(cvp->defp) ;
			}
		    } /* end if (def) */
		    if (rs < 0) {
			vecobj_finish(cvp->varp) ;
		    }
		} /* end if (var) */
		if (rs < 0) {
		    vecobj_finish(cvp->fesp) ;
		}
	    } /* end if (fes) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (configvars_open) */

int configvars_close(CV *cvp) noex {
	int		rs  ;
	int		rs1 ;
	if ((rs = configvars_magic(cvp)) >= 0) {
	    {
	        rs1 = configvars_finvars(cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = configvars_finfiles(cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = configvars_dtor(cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    cvp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (configvars_close) */

int configvars_addfile(CV *cvp,cchar *cfname,vecobj *eep) noex {
	int		rs ;
	if ((rs = configvars_magic(cvp,cfname)) >= 0) {
		rs = SR_INVALID ;
		if (cfname[0]) {
	            if ((rs = vecobj_count(cvp->fesp)) >= 0) {
			cint	isz = sizeof(int) ;
			cint	vr = rs ;
	    	        rs = SR_TOOBIG ;
			if (vr < isz) {
			    CV_FILE	fe ;
			    if ((rs = file_start(&fe,cfname)) >= 0) {
				vecobj	*vip = cvp->fesp ;
				if ((rs = vecobj_add(vip,&fe)) >= 0) {
				    cint	fi = rs ;
				    rs = configvars_parse(cvp,fi,eep) ;
				    if (rs < 0) {
					vecobj_del(cvp->fesp,fi) ;
				    }
				} /* end if (fe) */
				if (rs < 0) {
				    file_finish(&fe) ;
				}
			    } /* end if (file-) */
			} /* end if (size ok) */
		    } /* end if (count) */
	        } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (configvars_addfile) */


