/* hdbstr_obj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Key-Value Hash DataBase for Strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-12, David A­D­ Morano
	This module was adapted from some previous code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	hdbstr

	Description:
	This module provides a hash access container object for strings.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<hdb.h>
#include	<strwcpy.h>
#include	<strdcpyx.h>
#include	<localmisc.h>

#include	"hdbstr.h"


/* local defines */

#define	HS		hdbstr
#define	HS_CUR		hdbstr_cur


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int hdbstr::add(cchar *kstr,int klen,cchar *vstr,int vlen) noex {
	return hdbstr_add(this,kstr,klen,vstr,vlen) ;
}

int hdbstr::curbegin(hdbstr_cur *curp) noex {
	return hdbstr_curbegin(this,curp) ;
}

int hdbstr::curget(hdbstr_cur *curp,cc **kpp,cc **vpp,int *vlp) noex {
	return hdbstr_curget(this,curp,kpp,vpp,vlp) ;
}

int hdbstr::curenum(hdbstr_cur *curp,cc **kpp,cc **vpp,int *vlenp) noex {
	return hdbstr_curenum(this,curp,kpp,vpp,vlenp) ;
}

int hdbstr::curdel(hdbstr_cur *curp,int fadv) noex {
	return hdbstr_curdel(this,curp,fadv) ;
}

int hdbstr::curnext(hdbstr_cur *curp) noex {
	return hdbstr_curnext(this,curp) ;
}

int hdbstr::curdone(hdbstr_cur *curp) noex {
	return hdbstr_curdone(this,curp) ;
}

int hdbstr::curend(hdbstr_cur *curp) noex {
	return hdbstr_curend(this,curp) ;
}

int hdbstr::fetch(cchar *kstr,int klen,hdbstr_cur *curp,cchar **rpp) noex {
	return hdbstr_fetch(this,kstr,klen,curp,rpp) ;
}

int hdbstr::delkey(cchar *kstr,int klen) noex {
	return hdbstr_delkey(this,kstr,klen) ;
}

int hdbstr::loadkeys(cchar *fn) noex {
	return hdbstr_loadkeys(this,fn) ;
}

int hdbstr::loadpairs(cchar *fn) noex {
	return hdbstr_loadpairs(this,fn) ;
}

void hdbstr::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("hdbstr",rs,"fini-finish") ;
	}
}

int hdbstr_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case hdbstrmem_start:
	        if ((rs = hdbstr_start(op,a)) >= 0) {
		    op->fopen = true ;
		}
	        break ;
	    case hdbstrmem_delall:
	        rs = hdbstr_delall(op) ;
	        break ;
	    case hdbstrmem_count:
	        rs = hdbstr_count(op) ;
	        break ;
	    case hdbstrmem_finish:
	        rs = hdbstr_finish(op) ;
		op->fopen = false ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (hdbstr_co::operator) */


