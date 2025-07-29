/* svckey SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* service key data-structure-conversion access */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	int svckey_load(svckey *skp,svcfile_ent *sep) noex {

	Description:
	This object really forms some glue to pass service entry
	parameters (like from a file processed with SVCFILE) to
	other objects that expect to be handed pointers to service-entry
	related string values.

	Synopsis:
	int svckey_load(svckey *skp,svcfile_ent *sep) noex

	Arguments:
	skp
	sep

	Returns:
	>=0		OK
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<matxstr.h>
#include	<localmisc.h>

#include	"svckey.h"

import libutil ;

/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cpcchar	svckeys[] = {
	"file",
	"passfile",
	"so",
	"program",
	"args",
	"username",
	"groupname",
	"interval",
	"access",
	"opts",
	"failcont",
	"include",
	nullptr
} ; /* end array (svckeys) */


/* exported variables */


/* exported subroutines */

int svckey_load(svckey *skp,svcfile_ent *sep) noex {
	SVCKEY		*hop = skp ;
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (skp && sep) {
	    rs = memclear(hop) ;
	    skp->svc = sep->svc ;
	    for (int i = 0 ; sep->keyvals[i][0] != nullptr ; i += 1) {
	        cchar	*kp = sep->keyvals[i][0] ;
	        cchar	*vp = sep->keyvals[i][1] ;
	        if (int ki ; (ki = matostr(svckeys,1,kp,-1)) >= 0) {
	            switch (ki) {
	            case svckey_file:
	                skp->file = vp ;
	                break ;
	            case svckey_pass:
	                skp->pass = vp ;
	                break ;
	            case svckey_so:
	                skp->so = vp ;
	                break ;
	            case svckey_p:
	                skp->p = vp ;
	                break ;
	            case svckey_a:
	                skp->a = vp ;
	                break ;
	            case svckey_u:
	                skp->u = vp ;
	                break ;
	            case svckey_g:
	                skp->g = vp ;
	                break ;
	            case svckey_interval:
	                skp->interval = vp ;
	                break ;
	            case svckey_acc:
	                skp->acc = vp ;
	                break ;
	            case svckey_opts:
	                skp->opts = vp ;
	                break ;
	            case svckey_failcont:
	                skp->failcont = vp ;
	                break ;
	            case svckey_include:
	                skp->include = vp ;
	                break ;
	            } /* end switch */
	            if (ki >= 0) c += 1 ;
	        } /* end if (matostr) */
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (svckey_load) */


