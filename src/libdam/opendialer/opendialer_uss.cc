/* opendialer_uss SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open-dialer (USS-family) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUGN	0		/* special debugging */

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
		uss¥<filepath>[:<svc>][,to=<to>][­<arg(s)>]

	Example:
		uss¥/tmp/local/tcpmuxd/srv
		ussmus¥/tmp/local/tcpmuxd/srv:daytime

	Synopsis:
	int opendialer_uss(pr,prn,svc,of,om,argv,envv,to)
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

#include	"opendialer_uss.h"
#include	"defs.h"

import libutil ;

/* local defines */

#define	ARGPARSE	struct argparse

#define	NDF		"opendialer_uss.nd"


/* external subroutines */


/* external variables */


/* local structures */

struct argparse {
	cchar	*filepath ;
	cchar	*svc ;
	cchar	*a ;		/* memory allocation */
	int		to ;
} ;


/* local variables */

enum ops {
	op_to,
	op_overlast
} ;

constexpr cpcchar	ops[] = {
	"to",
	nullptr
} ;


/* forward references */

static int argparse_start(struct argparse *,cchar *) noex ;
static int argparse_finish(struct argparse *) noex ;


/* exported variables */


/* exported subroutines */

int opendialer_uss(pr,prn,fname,of,om,argv,envv,to)
cchar	*pr ;
cchar	*prn ;
cchar	*fname ;
int		of ;
mode_t		om ;
cchar	**argv ;
cchar	**envv ;
int		to ;
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		argc = 0 ;
	int		opts = 0 ;
	int		fd = -1 ;
	cchar	*argz = nullptr ;

#if	CF_DEBUGS
	{
	    int	i ;
	    debugprintf("opendialer_uss: prn=%s\n",prn) ;
	    debugprintf("opendialer_uss: fname=%s\n",fname) ;
	    if (argv != nullptr) {
	        for (i = 0 ; argv[i] != nullptr ; i += 1) {
	            debugprintf("opendialer_uss: a[%u]=%s\n",i,argv[i]) ;
	        }
	    }
	}
#endif /* CF_DEBUGS */

	if (fname[0] == '\0') return SR_INVALID ;

	if (argv != nullptr) {
	    for (argc = 0 ; argv[argc] != nullptr ; argc += 1) ;
	    argz = argv[0] ;
	}

	if ((rs >= 0) && (argz == nullptr)) rs = SR_NOENT ;
	if ((rs >= 0) && (argz[0] == '\0')) rs = SR_NOENT ;

/* parse out everything */

/*
	uss¥<fname>[:<svc>][,to=<to>][­<arg(s)>]]
*/

	if (rs >= 0) {
	    ARGPARSE	ai ;
	    if ((rs = argparse_start(&ai,argz)) >= 0) {
	        if ((to < 0) && (ai.to >= 0)) to = ai.to ;
	        if (rs >= 0) {
		    rs = dialuss(fname,to,opts) ;
		    fd = rs ;
	        }
	        rs1 = argparse_finish(&ai) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (argparse) */
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (ok) */

#if	CF_DEBUGS
	debugprintf("opendialer_uss: ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opendialer_uss) */


int opendialer_ussnls(pr,prn,fname,of,om,argv,envv,to)
cchar	*pr ;
cchar	*prn ;
cchar	*fname ;
int		of ;
mode_t		om ;
cchar	**argv ;
cchar	**envv ;
int		to ;
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		argc = 0 ;
	int		opts = 0 ;
	int		fd = -1 ;
	cchar	*argz = nullptr ;

#if	CF_DEBUGS
	{
	    int	i ;
	    debugprintf("opendialer_ussnls: prn=%s\n",prn) ;
	    debugprintf("opendialer_ussnls: fname=%s\n",fname) ;
	    if (argv != nullptr) {
	        for (i = 0 ; argv[i] != nullptr ; i += 1) {
	            debugprintf("opendialer_ussnls: a[%u]=%s\n",i,argv[i]) ;
	        }
	    }
	}
#endif /* CF_DEBUGS */

	if (fname[0] == '\0') return SR_INVALID ;

	if (argv != nullptr) {
	    for (argc = 0 ; argv[argc] != nullptr ; argc += 1) ;
	    argz = argv[0] ;
	}

	if ((rs >= 0) && (argz == nullptr)) rs = SR_NOENT ;
	if ((rs >= 0) && (argz[0] == '\0')) rs = SR_NOENT ;

/* parse out everything */

/*
	ussnls¥<fname>[:<svc>][,to=<to>][­<arg(s)>]]
*/

	if (rs >= 0) {
	    ARGPARSE	ai ;
	    if ((rs = argparse_start(&ai,argz)) >= 0) {
	        if ((to < 0) && (ai.to >= 0)) to = ai.to ;
	        if (rs >= 0) {
		    rs = dialussnls(fname,ai.svc,to,opts) ;
		    fd = rs ;
		}
	        rs1 = argparse_finish(&ai) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (argparse) */
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (ok) */

#if	CF_DEBUGS
	debugprintf("opendialer_ussnls: ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opendialer_ussnls) */


int opendialer_ussmux(pr,prn,fname,of,om,argv,envv,to)
cchar	*pr ;
cchar	*prn ;
cchar	*fname ;
int		of ;
mode_t		om ;
cchar	**argv ;
cchar	**envv ;
int		to ;
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		argc = 0 ;
	int		opts = 0 ;
	int		fd = -1 ;
	cchar	*argz = nullptr ;

#if	CF_DEBUGS
	{
	    int	i ;
	    debugprintf("opendialer_ussmux: prn=%s\n",prn) ;
	    debugprintf("opendialer_ussmux: fname=%s\n",fname) ;
	    if (argv != nullptr) {
	        for (i = 0 ; argv[i] != nullptr ; i += 1) {
	            debugprintf("opendialer_ussmux: a[%u]=%s\n",i,argv[i]) ;
	        }
	    }
	}
#endif /* CF_DEBUGS */

	if (fname[0] == '\0') return SR_INVALID ;

	if (argv != nullptr) {
	    for (argc = 0 ; argv[argc] != nullptr ; argc += 1) ;
	    argz = argv[0] ;
	}

	if ((rs >= 0) && (argz == nullptr)) rs = SR_NOENT ;
	if ((rs >= 0) && (argz[0] == '\0')) rs = SR_NOENT ;

/* parse out everything */

/*
	ussmux¥<fname>[:<svc>][,to=<to>][­<arg(s)>]]
*/

	if (rs >= 0) {
	    ARGPARSE	ai ;
	    if ((rs = argparse_start(&ai,argz)) >= 0) {
	        if ((to < 0) && (ai.to >= 0)) to = ai.to ;
	        if (rs >= 0) {
		    cchar	**av = argv ;
		    if (argc > 1) av = (argv+1) ;
		    rs = dialussmux(fname,ai.svc,av,to,opts) ;
		    fd = rs ;
	        }
	        rs1 = argparse_finish(&ai) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (argparse) */
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (ok) */

#if	CF_DEBUGS
	debugprintf("opendialer_uss: ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opendialer_ussmux) */


/* local subroutines */


/*
	ticotsord¥<af>:<host>:<port>[,to=<to>][,af=<af>][­<arg(s)>]
*/

static int argparse_start(struct argparse *app,cchar *argz)
{
	int		rs = SR_OK ;
	cchar	*tp ;

	memclear(app) ;
	app->to = -1 ;

	if (argz[0] == '\0') goto ret0 ;

	app->svc = argz ;
	if ((tp = strbrk(argz,",")) != nullptr) {
	    cchar	*sp = argz ;
	    cchar	*nsp ;
	    cchar	*svcp = argz ;
	    cchar	*optp, *kp, *vp ;
	    int		optl, kl, vl ;
	    int		svcl = (tp-argz) ;
	    int		oi ;
	    int		ch = MKCHAR(sp[0]) ;
	    while (ch) {
	        optp = sp ;
	        optl = -1 ;
	        if ((tp = strchr(sp,',')) != nullptr) {
	            optl = (tp-sp) ;
		    nsp = (tp+1) ;
	        } else {
	            optl = lenstr(sp) ;
		    nsp = (sp+optl) ;
	        }
#if	CF_DEBUGS
	        debugprintf("opendialer_ticotsord/argparse_start: o=>%r<\n",
		    optp,optl) ;
#endif
		kp = optp ;
		kl = optl ;
		vp = nullptr ;
		vl = 0 ;
		if ((tp = strnchr(optp,optl,'=')) != nullptr) {
		    kl = (tp-optp) ;
		    vp = (tp+1) ;
		    vl = ((optp+optl)-(tp+1)) ;
		}
#if	CF_DEBUGS
	    debugprintf("opendialer_ticotsord/argparse_start: k=%r\n",kp,kl) ;
		if (vp != nullptr) 
	    debugprintf("opendialer_ticotsord/argparse_start: v=%r\n",vp,vl) ;
#endif
	        if ((oi = matstr(ops,kp,kl)) >= 0) {
		    int		v ;
	            switch (oi) {
	            case op_to:
		        if (vl > 0) {
	                    rs = cfdecti(vp,vl,&v) ;
	                    app->to = v ;
		        }
	                break ;
	            } /* end switch */
		} /* end if (had valid option) */
	        sp = nsp ;
#if	CF_DEBUGS
	        debugprintf("opendialer_ticotsord/argparse_start: "
		    "while-bot rs=%d\n",rs) ;
#endif
	    	ch = MKCHAR(sp[0]) ;
		if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (svcp != nullptr)) {
	        int	size = 0 ;
	        char	*bp ;
	        if (svcp != nullptr) {
	            if (svcl < 0) svcl = lenstr(svcp) ;
	            size += (svcl + 1) ;
	        }
	        if ((rs = uc_malloc(size,&bp)) >= 0) {
	            app->a = bp ;
	            if (svcp != nullptr) {
	                app->svc = bp ;
	                bp = strwcpy(bp,svcp,svcl) + 1 ;
	            }
	        }
	    } /* end if */
	} /* end if */

ret0:
	return rs ;
}
/* end subroutine (argparse_start) */


static int argparse_finish(struct argparse *app)
{
	int	rs = SR_OK ;
	int	rs1 ;

	if (app->a != nullptr) {
	    rs1 = uc_free(app->a) ;
	    if (rs >= 0) rs = rs1 ;
	    app->a = nullptr ;
	}

	return rs ;
}
/* end subroutine (argparse_finish) */


