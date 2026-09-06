/* configvars_prime SUPPORT */
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
	configvars

	Description:
	This is an object that reads configuration files and organizes
	the content into the object for structured access.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
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

template<typename ... Args>
local inline int configvars_ctor(configvars *op,Args ... args) noex {
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->magval = 0 ;
	    op->ticheck = getustime ;
	    if ((op->fesp = new(nt) vecobj) != np) ylikely {
		if ((op->varp = new(nt) vecobj) != np) ylikely {
	            if ((op->defp = new(nt) vecobj) != np) ylikely {
	                if ((op->setp = new(nt) vecobj) != np) ylikely {
	                    if ((op->expp = new(nt) vecobj) != np) ylikely {
	            		if ((op->unvp = new(nt) vecobj) != np) ylikely {
				    rs = SR_OK ;
	                        } /* end if (new-unv) */
			        if (rs < 0) {
				    delete op->expp ;
				    op->expp = np ;
			        } /* end if (error) */
	                    } /* end if (new-exp) */
			    if (rs < 0) {
				delete op->setp ;
				op->setp = np ;
			    } /* end if (error) */
	                } /* end if (new-set) */
		        if (rs < 0) {
			    delete op->defp ;
			    op->defp = np ;
		        } /* end if (error) */
	            } /* end if (new-def) */
		    if (rs < 0) {
			delete op->varp ;
			op->varp = np ;
		    } /* end if (error) */
	        } /* end if (new-var) */
		if (rs < 0) {
		    delete op->fesp ;
		    op->fesp = np ;
		} /* end if (error) */
	    } /* end if (new-fes) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (configvars_ctor) */

local int configvars_dtor(configvars *op) noex {
	cnullptr	np{} ;
	int		rs = SR_OK ;
	if (op->unvp) {
	    delete op->unvp ;
	    op->unvp = np ;
	} /* end if (delete-vecobj) */
	if (op->expp) {
	    delete op->expp ;
	    op->expp = np ;
	} /* end if (delete-vecobj) */
	if (op->setp) {
	    delete op->setp ;
	    op->setp = np ;
	} /* end if (delete-vecobj) */
	if (op->defp) {
	    delete op->defp ;
	    op->defp = np ;
	} /* end if (delete-vecobj) */
	if (op->varp) {
	    delete op->varp ;
	    op->varp = np ;
	} /* end if (delete-vecobj) */
	if (op->fesp) {
	    delete op->fesp ;
	    op->fesp = np ;
	} /* end if (delete-vecobj) */
	return rs ;
} /* end subroutine (configvars_dtor) */


/* local variables */

enum configkeys {
	configkey_define,
	configkey_export,
	configkey_set,
	configkey_unset,
	configkey_overlast
} ; /* end enum */

constexpr cpcchar	configkeys[] = {
	"define",
	"export",
	"set",
	"unset",
	nullptr
} ; /* end array (configkeys) */

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
	if ((rs = configvars_ctor(cvp,cfn)) >= 0) ylikely {
	    cint	vn = 10 ;
	    int		vsz = szof(CV_FILE) ;
	    int		vo = 0 ;
	    vecobj	*vip = cvp->fesp ;
	    if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) ylikely {
	  	vsz = szof(CV_VAR) ;
	        vip = cvp->varp ;
	        if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) ylikely {
	      	    vip = cvp->defp ;
	            if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) ylikely {
	                vip = cvp->setp ;
		        if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) ylikely {
	            	    vip = cvp->expp ;
			    vo = vecobjm.sorted ;
			    if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
	                        vip = cvp->unvp ;
				if ((rs = vecobj_start(vip,vsz,vn,vo)) >= 0) {
				    cvp->magval = CONFIGVARS_MAGIC ;
				    if (cfn && cfn[0]) {
					auto ca = configvars_addfile ;
	    				if ((rs = ca(cvp,cfn,eep)) >= 0) {
					    rs = SR_OK ;
					}
				    }
				    if (rs < 0) {
				        vecobj_finish(cvp->unvp) ;
				    } /* end if (error) */
				} /* end if (unv) */
				if (rs < 0) {
			            vecobj_finish(cvp->expp) ;
				} /* end if (error) */
			    } /* end if (exp) */
			    if (rs < 0) {
				vecobj_finish(cvp->setp) ;
			    } /* end if (error) */
			} /* end if (set) */
			if (rs < 0) {
			    vecobj_finish(cvp->defp) ;
			} /* end if (error) */
		    } /* end if (def) */
		    if (rs < 0) {
			vecobj_finish(cvp->varp) ;
		    } /* end if (error) */
		} /* end if (var) */
		if (rs < 0) {
		    vecobj_finish(cvp->fesp) ;
		} /* end if (error) */
	    } /* end if (fes) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (configvars_open) */

int configvars_close(CV *cvp) noex {
	int		rs  ;
	int		rs1 ;
	if ((rs = configvars_magic(cvp)) >= 0) ylikely {
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
	    cvp->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (configvars_close) */

int configvars_addfile(CV *cvp,cchar *cfname,vecobj *eep) noex {
	int		rs ;
	if ((rs = configvars_magic(cvp,cfname)) >= 0) ylikely {
		rs = SR_INVALID ;
		if (cfname[0]) ylikely {
	            if ((rs = vecobj_count(cvp->fesp)) >= 0) ylikely {
			cint	isz = szof(int) ;
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
				    } /* end if (error) */
				} /* end if (fe) */
				if (rs < 0) {
				    file_finish(&fe) ;
				} /* end if (error) */
			    } /* end if (file-) */
			} /* end if (size ok) */
		    } /* end if (count) */
	        } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (configvars_addfile) */


