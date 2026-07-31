/* vmail_term SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program terminal mode */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_DEBUG	0		/* run-time debug print-outs */
#define	CF_TERMSTORE	0		/* use 'termstore()' */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code module was originally written but was modeled
	after some preexisting code. This whole program has been
	rewritten from the ground up.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	name:
	vmail_term

	Description:
	This is the front-end subroutine for the PCS program VMAIL.
	There is a good bit of setup in this subroutine before the
	program goes interactive.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<csignal>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<tzfile.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<bfile.h>
#include	<uterm.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG */

#include	"vmail_config.h"
#include	"defs.h"
#include	"inter.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int proctermlines(PROGINFO *,uterm *) noex ;
local int procmesg_begin(PROGINFO *,uterm *) noex ;
local int procmesg_end(PROGINFO *,uterm *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progterm(progifo *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (isatty(pip->tfd)) {

/* part one -- get a terminal type */

#if	CF_TERMSTORE
	termstore_save(&pip->ts,pip->tfd) ;
#endif

/* terminal input-manager initialization and GO */

	if (uterm ut ; (rs = uterm_start(&ut,pip->tfd)) >= 0) {
	    if ((rs = proctermlines(pip,&ut)) >= 0) {
		if ((rs = procmesg_begin(pip,&ut)) >= 0) {
		    cint	ucmd = utermcmd_setmode ;
		    cint	fm = fm_notecho ;
	            if ((rs = uterm_control(&ut,ucmd,fm)) >= 0) {
	        	pip->iap = &ia ;
			if (INTER ia ; (rs = inter_start(&ia,pip,&ut)) >= 0) {
			    {
	                        while ((rs = inter_cmd(&ia)) > 0) ;
			    }
	                    rs1 = inter_finish(&ia) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (inter) */
	                pip->iap = nullptr ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progterm: inter-out rs=%d\n",rs) ;
#endif

		    } /* end if (utermcmd-setmode) */
		    rs1 = procmesg_end(pip,&ut) ;
		    if (rs >= 0) rs = rs1 ;
	 	} /* end if (procmesg) */
#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progterm: setmesg-out rs=%d\n",rs) ;
#endif
	    } /* end if (proctermlines) */
	    rs1 = uterm_finish(&ut) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (uterm) */

#if	CF_TERMSTORE
	termstore_restore(&pip->ts,pip->tfd) ;
#endif

	} else {
	    rs = SR_INVALID ;
	    bprintf(pip->efp,"%s: not on a terminal\n",
	        pip->progname) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progterm: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (progterm) */


/* local subroutines */

local int proctermlines(PROGINFO *pip,uterm *utp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		v ;
	cchar	*cp ;

#ifdef	COMMENT
	if (pip->termtype == nullptr) pip->termtype = getenv(VARTERM) ;
	if (pip->termtype == nullptr) pip->termtype = DEFTERMTYPE ;
#endif /* COMMENT */

/* actual terminal lines */

	if (pip->lines_term <= 0) {
	    static cchar *varp = getenv(VARLINES) ;
	    if (varp) {
	        if (cfdeci(cp,-1,&v) >= 0) {
		    pip->lines_term = v ;
		}
	    } /* end if (getenv-varlines) */
	}

	if (pip->lines_term <= 0) {
	    rs1 = uterm_control(utp,utermcmd_getlines,0) ;
	    if (rs1 >= 0) pip->lines_term = rs1 ;
	} /* end if */

	if (pip->lines_term <= 0) {
	    pip->lines_term = TERMLINES_DEF ;
	}

/* requested terminal lines */

	if (pip->lines_req <= 0) {
	    pip->lines_req = pip->lines_term ;
	}

	if (pip->lines_req > TERMLINES_MAX) {
	    pip->lines_req = TERMLINES_MAX ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progterm: lines_req=%u\n",
	        pip->lines_req) ;
#endif

/* ensure a minimum number of viewable terminal lines */

	if (pip->lines_req < (MINSCANLINES + MINVIEWLINES + FRAMELINES)) {
	    cchar *fmt = "%s: not enough terminal lines for program\n" ;
	    rs = SR_DOM ;
	    bprintf(pip->efp,fmt,pip->progname) ;
	}

	return rs ;
}
/* end subroutine (proctermlines) */

local int procmesg_begin(PROGINFO *pip,uterm *utp) noex {
    	int		ucmd ;
	int		rs ;
	ucmd = utermcmd_getuid ;
	if ((rs = uterm_control(utp,ucmd,0)) >= 0) {
	    if (pip->euid == rs) {
		ucmd = utermcmd_setmesg ;
		if ((rs = uterm_control(utp,ucmd,false)) >= 0) {
	    	    pip->fl.mesgs = (rs > 0) ;
		    if (rs > 0) pip->changed.mesgs = true ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (procmesg_begin) */

local int procmesg_end(PROGINFO *pip,uterm *utp) noex {
	int		rs = SR_OK ;
	if (pip->changed.mesgs) {
	    cint	ucmd = utermcmd_setmesg ;
	    rs = uterm_control(utp,ucmd,pip->fl.mesgs) ;
	}
	return rs ;
}
/* end subroutine (procmesg_end) */


