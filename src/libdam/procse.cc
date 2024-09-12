/* procse SUPPORT */
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<vecstr.h>
#include	<varsub.h>
#include	<expcook.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"procse.h"


/* local defines */

#define	BUF_MULT	10		/* multiplier for buffer length */


/* external subroutines */


/* external variables */


/* local structures */

struct vars {
	int		maxpathlen ;	/* set but not currently used */
	int		ebuflen ;
} ;


/* forward references */

static int	mkvars() noex ;


/* local variables */

static vars	var ;


/* external variables */


/* exported subroutines */

int procse_start(procse *pep,cchar **envv,varsub *vsp,procse_args *esap) noex {
	int		rs = SR_FAULT ;
	if (pep && esap) {
	    static int	rsv = mkvars() ;
	    memclear(pep) ; /* potentionally dangerous */
	    if ((rs = rsv) >= 0) {
	        pep->envv = envv ;
	        pep->vsp = vsp ;
	        pep->ap = esap ;
	    } /* end if (mkvars) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (procse_start) */

int procse_finish(procse *pep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pep) {
	    rs = SR_OK ;
	    if (pep->a.passfile != nullptr) {
	        rs1 = uc_free(pep->a.passfile) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pep->a.sharedobj != nullptr) {
	        rs1 = uc_free(pep->a.sharedobj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pep->a.program != nullptr) {
	        rs1 = uc_free(pep->a.program) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pep->a.srvargs != nullptr) {
	        rs1 = uc_free(pep->a.srvargs) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pep->a.username != nullptr) {
	        rs1 = uc_free(pep->a.username) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pep->a.groupname != nullptr) {
	        rs1 = uc_free(pep->a.groupname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pep->a.options != nullptr) {
	        rs1 = uc_free(pep->a.options) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pep->a.access != nullptr) {
	        rs1 = uc_free(pep->a.access) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pep->a.failcont != nullptr) {
	        rs1 = uc_free(pep->a.failcont) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    pep->a = {} ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (procse_finish) */

namespace {
    struct subproc {
	procse	*pep ;
	expcook	*ecp ;
	char	*a = nullptr ;
	char	*vbuf = nullptr ;
	char	*ebuf = nullptr ;
	int	vlen ;
	int	elen ;
	subproc(procse *pp,expcook *ep) noex : pep(pp), ecp(ep) { 
	    vlen = var.ebuflen ;
	    elen = var.ebuflen ;
	} ;
	int start() noex ;
	int finish() noex ;
	int proc(cchar *,cchar **) noex ;
	int stageone(procse *,cc *) noex ;
	int stagetwo(expcook *,int,cchar **) noex ;
    } ; /* end struct (subproc) */
}

int subproc::start() noex {
	cint		sz = (vlen+1) + (elen+1) ;
	int		rs ;
	if ((rs = uc_malloc(sz,&a)) >= 0) {
	    vbuf = a ;
	    ebuf = (a + (vlen+1)) ;
	} /* end if (malloc-{x}buf) */
	return rs ;
}
/* end method (subproc::start) */

int subproc::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (a) {
	    rs1 = uc_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	    vbuf = nullptr ;
	    ebuf = nullptr ;
	    vlen = 0 ;
	    elen = 0 ;
	}
	return rs ;
}
/* end method (subproc::finish) */

int subproc::stageone(procse *pep,cc *inbuf) noex {
	int		rs = SR_OK ;
	int		vl = 0 ;
	if (pep->vsp != nullptr) {
	    rs = varsub_exp(pep->vsp,vbuf,vlen,inbuf,-1) ;
	    vl = rs ;
	} else {
	    rs = sncpy1(vbuf,vlen,inbuf) ;
	    vl = rs ;
	}
	return (rs >= 0) ? vl : rs ;
}
/* end method (subproc::stageone) */

int subproc::stagetwo(expcook *ecp,int vl,cchar **opp) noex {
	int		rs = SR_OK ;
	int		el = 0 ;
	int		fl = 0 ;
	if (ecp != nullptr) {
	    rs = expcook_exp(ecp,0,ebuf,elen,vbuf,vl) ;
	    el = rs ;
	} else {
	    rs = snwcpy(ebuf,elen,vbuf,vl) ;
	    el = rs ;
	}
	if (rs >= 0) {
	    cchar	*fp ;
	    fl = sfshrink(ebuf,el,&fp) ;
	    if (cc *cp ; (rs = uc_mallocstrw(fp,fl,&cp)) >= 0) {
		*opp = cp ;
	    }
	}
	return (rs >= 0) ? fl : rs ;
}
/* end method (subproc::stagetwo) */

int subproc::proc(cchar *inbuf,cchar **opp) noex {
	int		rs ;
	char		fl = 0 ;
	*opp = nullptr ;
	if ((rs = stageone(pep,inbuf)) >= 0) {
	    rs = stagetwo(ecp,rs,opp) ;
	    fl = rs ;
	} /* end if (ok) */
	return (rs >= 0) ? fl : rs ;
}
/* end method (subproc::proc) */

int procse_process(procse *pep,expcook *ecp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pep) {
	    subproc	so(pep,ecp) ;
	    if ((rs = so.start()) >= 0) {
	        procse_args	*ap = pep->ap ;
	        if ((rs >= 0) && (ap->passfile != nullptr)) {
	            rs = so.proc(ap->passfile,&pep->a.passfile) ;
	        }
	        if ((rs >= 0) && (ap->sharedobj != nullptr)) {
	            rs = so.proc(ap->sharedobj,&pep->a.sharedobj) ;
	        }
	        if ((rs >= 0) && (ap->program != nullptr)) {
	            rs = so.proc(ap->program,&pep->a.program) ;
	        }
	        if ((rs >= 0) && (ap->srvargs != nullptr)) {
	            rs = so.proc(ap->srvargs,&pep->a.srvargs) ;
	        }
	        if ((rs >= 0) && (ap->username != nullptr)) {
	            rs = so.proc(ap->username,&pep->a.username) ;
	        }
	        if ((rs >= 0) && (ap->groupname != nullptr)) {
	            rs = so.proc(ap->groupname,&pep->a.groupname) ;
	        }
	        if ((rs >= 0) && (ap->options != nullptr)) {
	            rs = so.proc(ap->options,&pep->a.options) ;
	        }
	        if ((rs >= 0) && (ap->access != nullptr)) {
	            rs = so.proc(ap->access,&pep->a.access) ;
	        }
	        if ((rs >= 0) && (ap->failcont != nullptr)) {
	            rs = so.proc(ap->failcont,&pep->a.failcont) ;
	        }
	        rs1 = so.finish() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (so) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (procse_process) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    var.maxpathlen = rs ;
	    var.ebuflen = (rs * BUF_MULT) ;
	}
	return rs ;
}
/* end subroutine (mkvars) */


