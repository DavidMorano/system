/* procse SUPPORT */
/* lang=C++20 */

/* build up a server entry piece-meal as it were */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little object is used to create a server entry and to
	populate aspects of it with different operations on the
	object.  This object is used in "server" types of programs.
	This object is usually created from elements taken from the
	parsing of a server file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<varsub.h>
#include	<expcook.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"procse.h"


/* local defines */

#undef	BUFLEN
#define	BUFLEN		(10 * MAXPATHLEN)


/* external subroutines */


/* external variables */


/* local structures */

struct vars {
	int		ebuflen ;
} ;


/* forward references */

static int	process(PROCSE *,expcook *,cchar *,cchar **) noex ;

static int	mkvars() noex ;


/* local variables */


/* external variables */


/* exported subroutines */

int procse_start(procse *pep,cchar **envv,varsub *vsp,procse_args *esap) noex {
	int		rs = SR_FAULT ;
	if (pep && esap) {
	    rs = memclear(pep) ;
	    pep->envv = envv ;
	    pep->vsp = vsp ;
	    pep->ap = esap ;
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
	    memclear(pep) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (procse_finish) */

int procse_process(procse *pep,expcook *ecp) noex {
	int		rs = SR_FAULT ;
	if (pep) {
	    procse_args	*ap = pep->ap ;
	    rs = SR_OK ;
	    /* pop them */
	    if ((rs >= 0) && (ap->passfile != nullptr)) {
	        rs = process(pep,ecp,ap->passfile,&pep->a.passfile) ;
	    }
	    if ((rs >= 0) && (ap->sharedobj != nullptr)) {
	        rs = process(pep,ecp,ap->sharedobj,&pep->a.sharedobj) ;
	    }
	    if ((rs >= 0) && (ap->program != nullptr)) {
	        rs = process(pep,ecp,ap->program,&pep->a.program) ;
	    }
	    if ((rs >= 0) && (ap->srvargs != nullptr)) {
	        rs = process(pep,ecp,ap->srvargs,&pep->a.srvargs) ;
	    }
	    if ((rs >= 0) && (ap->username != nullptr)) {
	        rs = process(pep,ecp,ap->username,&pep->a.username) ;
	    }
	    if ((rs >= 0) && (ap->groupname != nullptr)) {
	        rs = process(pep,ecp,ap->groupname,&pep->a.groupname) ;
	    }
	    if ((rs >= 0) && (ap->options != nullptr)) {
	        process(pep,ecp,ap->options,&pep->a.options) ;
	    }
	    if ((rs >= 0) && (ap->access != nullptr)) {
	        rs = process(pep,ecp,ap->access,&pep->a.access) ;
	    }
	    if ((rs >= 0) && (ap->failcont != nullptr)) {
	        rs = process(pep,ecp,ap->failcont,&pep->a.failcont) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (procse_process) */


/* local subroutines */

static int process(procse *pep,expcook *ecp,cchar *inbuf,cchar **opp) noex {
	int		rs = SR_OK ;
	char		fl = 0 ;
	cchar		*ccp ;
	cchar		*fp ;

	if (opp == nullptr) return SR_FAULT ;

	*opp = nullptr ;
	if (rs >= 0) {
	    cint	vlen = BUFLEN ;
	    int		vl = 0 ;
	    char	vbuf[BUFLEN + 1] ;
	    if (pep->vsp != nullptr) {
	        rs = varsub_exp(pep->vsp,vbuf,vlen,inbuf,-1) ;
	        vl = rs ;
	    } else {
	        rs = sncpy1(vbuf,vlen,inbuf) ;
	        vl = rs ;
	    }
	    if (rs >= 0) {
		cint	elen = BUFLEN ;
		int	el = 0 ;
	        char	ebuf[BUFLEN + 1] ;
	        if (ecp != nullptr) {
	            rs = expcook_exp(ecp,0,ebuf,elen,vbuf,vl) ;
	            el = rs ;
	        } else {
	            rs = snwcpy(ebuf,elen,vbuf,vl) ;
	            el = rs ;
	        }
	        if (rs >= 0) {
	            fl = sfshrink(ebuf,el,&fp) ;
	            if ((rs = uc_mallocstrw(fp,fl,&ccp)) >= 0) {
	                *opp = ccp ;
	            }
	        }
	    } /* end if (ok) */
	} /* end if (ok) */

	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (process) */


