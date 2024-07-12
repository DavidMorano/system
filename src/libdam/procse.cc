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
#include	<vecstr.h>
#include	<varsub.h>
#include	<expcook.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"procse.h"


/* local defines */

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(4 * MAXPATHLEN)
#endif

#undef	BUFLEN
#define	BUFLEN		(10 * MAXPATHLEN)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	process(PROCSE *,EXPCOOK *,cchar *,cchar **) noex ;


/* local variables */


/* external variables */


/* exported subroutines */

int procse_start(PROCSE *pep,cchar **envv,varsub *vsp,PROCSE_ARGS *esap) noex {
	if (pep == nullptr) return SR_FAULT ;
	if (esap == nullptr) return SR_FAULT ;

	memset(pep,0,sizeof(PROCSE)) ;
	pep->envv = envv ;
	pep->vsp = vsp ;
	pep->ap = esap ;

	return SR_OK ;
}
/* end subroutine (procse_start) */

int procse_finish(PROCSE *pep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (pep == nullptr) return SR_FAULT ;

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

	memset(pep,0,sizeof(PROCSE)) ;

	return rs ;
}
/* end subroutine (procse_finish) */

int procse_process(PROCSE *pep,EXPCOOK *ecp) noex {
	PROCSE_ARGS	*ap ;
	int		rs = SR_OK ;

	if (pep == nullptr) return SR_FAULT ;

	ap = pep->ap ;

/* pop them */

	if ((rs >= 0) && (ap->passfile != nullptr))
	    rs = process(pep,ecp,ap->passfile,&pep->a.passfile) ;

	if ((rs >= 0) && (ap->sharedobj != nullptr))
	    rs = process(pep,ecp,ap->sharedobj,&pep->a.sharedobj) ;

	if ((rs >= 0) && (ap->program != nullptr))
	    rs = process(pep,ecp,ap->program,&pep->a.program) ;

	if ((rs >= 0) && (ap->srvargs != nullptr))
	    rs = process(pep,ecp,ap->srvargs,&pep->a.srvargs) ;

	if ((rs >= 0) && (ap->username != nullptr))
	    rs = process(pep,ecp,ap->username,&pep->a.username) ;

	if ((rs >= 0) && (ap->groupname != nullptr))
	    rs = process(pep,ecp,ap->groupname,&pep->a.groupname) ;

	if ((rs >= 0) && (ap->options != nullptr))
	    process(pep,ecp,ap->options,&pep->a.options) ;

	if ((rs >= 0) && (ap->access != nullptr))
	    rs = process(pep,ecp,ap->access,&pep->a.access) ;

	if ((rs >= 0) && (ap->failcont != nullptr))
	    rs = process(pep,ecp,ap->failcont,&pep->a.failcont) ;

	return rs ;
}
/* end subroutine (procse_process) */


/* local subroutines */

static int process(PROCSE *pep,EXPCOOK *ecp,cchar *inbuf,cchar **opp) noex {
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


