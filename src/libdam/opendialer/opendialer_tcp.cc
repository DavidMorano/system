/* opendialer_tcp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open-dialer (tcp) */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-11-04, David A­D­ Morano
	This code was started by taking the corresponding code from
	the TCP-family module.  In retrospect, that was a mistake.
	Rather I should have started this code by using the
	corresponding UUX dialer module.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an open-dialer.

	The file-name corresponding to this dialer looks like:
		tcp¥<af>:<host>:<port>[,to=<to>][­<arg(s)>]
	or
		tcp¥<host>:<port>[,to=<to>][,af=<af>][­<arg(s)>]

	Example:
		tcp¥inet6:rca:daytime

	Synopsis:
	int opendialer_tcp(pr,prn,svc,of,om,argv,envv,to)
	cchar	*pr ;
	cchar	*prn ;
	cchar	*svc ;
	int		of ;
	mode_t		om ;
	cchar	**argv ;
	cchar	**envv ;
	int		to ;

	Arguments:
	pr		program-root
	prn		facility name
	svc		service name
	of		open-flags
	om		open-mode
	argv		argument array
	envv		environment array
	to		time-out

	Returns:
	>=0		file-descriptor
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strx.h>
#include	<localmisc.h>

#include	"opendialer_tcp.h"
#include	"defs.h"

import libutil ;

/* local defines */

#define	ARGPARSE	struct argparse


/* external subroutines */


/* external variables */


/* local structures */

struct argparse {
	cchar	*hostname ;
	cchar	*portspec ;
	cchar	*a ;		/* memory allocation */
	int		af ;
	int		to ;
} ;


/* local variables */

enum ops {
	op_to,
	op_af,
	op_overlast
} ;

constexpr cpcchar	ops[] = {
	"to",
	"af",
	nullptr
} ;


/* forward references */

static int argparse_start(struct argparse *,cchar *) ;
static int argparse_finish(struct argparse *) ;


/* exported variables */


/* exported subroutines */

int opendialer_tcp(pr,prn,svc,of,om,argv,envv,to)
cchar	*pr ;
cchar	*prn ;
cchar	*svc ;
int		of ;
mode_t		om ;
cchar	**argv ;
cchar	**envv ;
int		to ;
{
	ARGPARSE	ai ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		argc = 0 ;
	int		af = AF_UNSPEC ;
	int		opts = 0 ;
	int		fd = -1 ;
	cchar	*argz = nullptr ;
	cchar	*hostname = nullptr ;
	cchar	*portspec = nullptr ;

	if (svc[0] == '\0') return SR_INVALID ;

	if (argv != nullptr) {
	    for (argc = 0 ; argv[argc] != nullptr ; argc += 1) ;
	    argz = argv[0] ;
	}

	if ((rs >= 0) && (argz == nullptr)) rs = SR_NOENT ;
	if ((rs >= 0) && (argz[0] == '\0')) rs = SR_NOENT ;
	if (rs < 0) goto ret0 ;

/* parse out everything */

/*
	tcp¥<af>:<host>:<port>[,to=<to>][­<arg(s)>]
*/

	if ((rs = argparse_start(&ai,argz)) >= 0) {

	    if (ai.to >= 0) to = ai.to ;
	    if (ai.af >= 0) af = ai.af ;
	    portspec = ai.portspec ;
	    if (ai.hostname != nullptr) {
	        rs = getaf(svc,-1) ;
	        af = rs ;
	        hostname = ai.hostname ;
	    } else {
	        hostname = svc ;
	    }

/* continue */

	    if (rs >= 0) {
		rs = dialtcp(hostname,portspec,af,to,opts) ;
		fd = rs ;
	    }

	    rs1 = argparse_finish(&ai) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (argparse) */

ret0:

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opendialer_tcp) */


/* local subroutines */


/*
	tcp¥<af>:<host>:<port>[,to=<to>][,af=<af>][­<arg(s)>]
*/

static int argparse_start(struct argparse *app,cchar *argz) noex {
	int		rs = SR_OK ;
	int		hostl = 0 ;
	int		portl = 0 ;
	int		opl = 0 ;
	cchar	*tp, *sp ;
	cchar	*hostp = nullptr ;
	cchar	*portp = nullptr ;
	cchar	*opp = nullptr ;

	memclear(app) ;
	app->to = -1 ;
	app->af = -1 ;

	if (argz[0] == '\0') goto ret0 ;

	if ((tp = strbrk(argz,",:")) != nullptr) {
	    int		oi ;
	    int		v ;
	    int		kl, vl ;
	    cchar	*nsp ;
	    cchar	*kp, *vp ;
	    sp = (tp+1) ;
	    if (tp[0] == ':') {
	        hostp = argz ;
	        hostl = (tp-argz) ;
	        portp = (tp+1) ;
	        portl = -1 ;
	        if ((tp = strchr(sp,',')) != nullptr) {
	            portl = (tp-sp) ;
	            sp = (tp+1) ;
	        }
	    } else {
	        portp = argz ;
	        portl = (tp-argz) ;
	    }
	    while (sp[0]) {
	        opp = sp ;
	        opl = -1 ;
	        if ((tp = strchr(sp,',')) != nullptr) {
	            opl = (tp-sp) ;
		    nsp = (tp+1) ;
	        } else {
	            opl = lenstr(sp) ;
		    nsp = (sp+opl) ;
	        }
		kp = opp ;
		kl = opl ;
		vp = nullptr ;
		vl = 0 ;
		if ((tp = strnchr(opp,opl,'=')) != nullptr) {
		    kl = (tp-opp) ;
		    vp = (tp+1) ;
		    vl = (opp+opl) - (tp+1) ;
		}
	        if ((oi = matstr(ops,kp,kl)) >= 0) {
	            switch (oi) {
	            case op_af:
		        if (vl > 0) {
	                    rs = getaf(vp,vl) ;
	                    app->af = rs ;
		        }
	                break ;
	            case op_to:
		        if (vl > 0) {
	                    rs = cfdecti(vp,vl,&v) ;
	                    app->to = v ;
		        }
	                break ;
	            } /* end switch */
		} /* end if (had valid option) */
	        sp = nsp ;
		if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && ((hostp != nullptr) || (portp != nullptr))) {
	        int	sz = 0 ;
	        char	*bp ;
	        if (hostp != nullptr) {
	            if (hostl < 0) hostl = lenstr(hostp) ;
	            sz += (hostl + 1) ;
	        }
	        if (portp != nullptr) {
	            if (portl < 0) portl = lenstr(portp) ;
	            sz += (portl + 1) ;
	        }
	        if ((rs = uc_malloc(sz,&bp)) >= 0) {
	            app->a = bp ;
	            if (hostp != nullptr) {
	                app->hostname = bp ;
	                bp = strwcpy(bp,hostp,hostl) + 1 ;
	            }
	            if (portp != nullptr) {
	                app->portspec = bp ;
	                bp = strwcpy(bp,portp,portl) + 1 ;
	            }
	        }
	    } /* end if */
	} else {
	    app->portspec = argz ;
	}

ret0:
	return rs ;
}
/* end subroutine (argparse_start) */

static int argparse_finish(struct argparse *app) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (app->a != nullptr) {
	    rs1 = uc_free(app->a) ;
	    if (rs >= 0) rs = rs1 ;
	    app->a = nullptr ;
	}

	app->hostname = nullptr ;
	app->portspec = nullptr ;
	return rs ;
}
/* end subroutine (argparse_finish) */


