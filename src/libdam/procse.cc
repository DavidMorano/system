/* procse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* build up a server entry piece-meal as it were */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	procse

	Description:
	This little object is used to create a server entry and to
	populate aspects of it with different operations on the
	object.  This object is used in "server" types of programs.
	This object is usually created from elements taken from the
	parsing of a server file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<varsub.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"procse.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	BUFMULT		10		/* multiplier for buffer length */


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpathlen ;	/* set but not currently used */
	int		ebuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */


/* local variables */

static vars	var ;


/* external variables */


/* exported subroutines */

int procse_start(procse *op,con mainv envv,varsub *vsp,procse_args *esap) noex {
	PROCSE		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && esap) ylikely {
	    memclear(hop) ;
	    if (static cint rsv = var ; (rs = rsv) >= 0) {
	        op->envv	= envv ;
	        op->vsp		= vsp ;
	        op->ap		= esap ;
	    } /* end if (vars) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (procse_start) */

namespace {
    struct memfreer {
	int	rs = SR_OK ;
	int	rs1 ;
	void operator () (ccharp &p) noex {
	    if (p) ylikely {
		voidp vp = voidp(p) ;
	        rs1 = mem.free(vp) ;
		if (rs >= 0) rs = rs1 ;
		p = nullptr ;
	    } /* end if (memory-release) */
	} ; /* end method */
	operator int () const noex {
	    return rs ;
	} ; /* end method */
    } ; /* end struct */
} /* end namespace */

int procse_finish(procse *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    memfreer fo ;
	    fo(op->a.passfile) ;
	    fo(op->a.sharedobj) ;
	    fo(op->a.program) ;
	    fo(op->a.srvargs) ;
	    fo(op->a.username) ;
	    fo(op->a.groupname) ;
	    fo(op->a.options) ;
	    fo(op->a.access) ;
	    fo(op->a.failcont) ;
	    rs = fo ;
	    op->a = {} ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (procse_finish) */

namespace {
    struct subproc {
	procse		*op ;
	expcook		*ecp ;
	char		*a = nullptr ;
	char		*vbuf = nullptr ;
	char		*ebuf = nullptr ;
	int		vlen ;
	int		elen ;
	subproc(procse *pp,expcook *ep) noex : op(pp), ecp(ep) { 
	    vlen = var.ebuflen ;
	    elen = var.ebuflen ;
	} ; /* end ctor */
	int start	() noex ;
	int finish	() noex ;
	int proc	(cchar *,cchar **) noex ;
	int stageone	(cc *) noex ;
	int stagetwo	(int,cchar **) noex ;
    } ; /* end struct (subproc) */
} /* end namespace */

int subproc::start() noex {
	cint		sz = (vlen+1) + (elen+1) ;
	int		rs ;
	int		ai = 0 ;
	if ((rs = mem.mall(sz,&a)) >= 0) ylikely {
	    vbuf = (a + (vlen + ai++)) ;
	    ebuf = (a + (vlen + ai++)) ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end method (subproc::start) */

int subproc::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (a) ylikely {
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	    vbuf = nullptr ;
	    ebuf = nullptr ;
	    vlen = 0 ;
	    elen = 0 ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (subproc::finish) */

int subproc::stageone(cc *inbuf) noex {
	int		rs = SR_OK ;
	int		vl = 0 ; /* return-value */
	if (op->vsp != nullptr) {
	    rs = varsub_exp(op->vsp,vbuf,vlen,inbuf,-1) ;
	    vl = rs ;
	} else {
	    rs = sncpy1(vbuf,vlen,inbuf) ;
	    vl = rs ;
	}
	return (rs >= 0) ? vl : rs ;
} /* end method (subproc::stageone) */

int subproc::stagetwo(int vl,cchar **opp) noex {
	int		rs = SR_OK ;
	int		el = 0 ;
	int		fl = 0 ; /* return-value */
	if (ecp) {
	    rs = expcook_exp(ecp,0,ebuf,elen,vbuf,vl) ;
	    el = rs ;
	} else {
	    rs = snwcpy(ebuf,elen,vbuf,vl) ;
	    el = rs ;
	}
	if (rs >= 0) ylikely {
	    cchar	*fp ;
	    fl = sfshrink(ebuf,el,&fp) ;
	    if (cc *cp ; (rs = mem.strw(fp,fl,&cp)) >= 0) {
		*opp = cp ;
	    } /* end if (memory-acquire) */
	} /* end if (ok) */
	return (rs >= 0) ? fl : rs ;
} /* end method (subproc::stagetwo) */

int subproc::proc(cchar *inbuf,cchar **opp) noex {
	int		rs ;
	int		fl = 0 ; /* return-value */
	*opp = nullptr ;
	if ((rs = stageone(inbuf)) >= 0) ylikely {
	    rs = stagetwo(rs,opp) ;
	    fl = rs ;
	} /* end if (ok) */
	return (rs >= 0) ? fl : rs ;
} /* end method (subproc::proc) */

int procse_process(procse *op,expcook *ecp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    if (subproc	so(op,ecp) ; (rs = so.start()) >= 0) ylikely {
	        procse_args	*ap = op->ap ;
	        if ((rs >= 0) && ap->passfile) {
	            rs = so.proc(ap->passfile,&op->a.passfile) ;
	        }
	        if ((rs >= 0) && ap->sharedobj) {
	            rs = so.proc(ap->sharedobj,&op->a.sharedobj) ;
	        }
	        if ((rs >= 0) && ap->program) {
	            rs = so.proc(ap->program,&op->a.program) ;
	        }
	        if ((rs >= 0) && ap->srvargs) {
	            rs = so.proc(ap->srvargs,&op->a.srvargs) ;
	        }
	        if ((rs >= 0) && ap->username) {
	            rs = so.proc(ap->username,&op->a.username) ;
	        }
	        if ((rs >= 0) && ap->groupname) {
	            rs = so.proc(ap->groupname,&op->a.groupname) ;
	        }
	        if ((rs >= 0) && ap->options) {
	            rs = so.proc(ap->options,&op->a.options) ;
	        }
	        if ((rs >= 0) && ap->access) {
	            rs = so.proc(ap->access,&op->a.access) ;
	        }
	        if ((rs >= 0) && ap->failcont) {
	            rs = so.proc(ap->failcont,&op->a.failcont) ;
	        }
	        rs1 = so.finish() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (so) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (procse_process) */

vars::operator int () noex {
	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    var.maxpathlen = rs ;
	    var.ebuflen = (rs * BUFMULT) ;
	}
	return rs ;
} /* end subroutine (vars::operator) */


