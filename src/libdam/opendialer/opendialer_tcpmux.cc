/* opendialer_tcpmux SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open-dialer (tcpmux) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 2003-11-04, David A�D� Morano
	This code was started by taking the corresponding code from
	the TCP-family module.  In retrospect, that was a mistake.
	Rather I should have started this code by using the
	corresponding UUX dialer module.

*/

/* Copyright � 2003 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is an open-dialer.

	The file-name corresponding to this dialer looks like:

		tcpmux�[<af>:]<host>[:<port>]:<svc>[,to=<to>][�<arg(s)>]

	Example:

		tcpmux�rca:daytime

	Synopsis:
	int opendialer_tcpmux(pr,prn,svc,of,om,argv,envv,to)
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

#include	"opendialer_tcpmux.h"
#include	"defs.h"


/* local defines */

#define	ARGPARSE	struct argparse


/* external subroutines */

extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	cfdecti(cchar *,int,int *) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	findxfile(IDS *,char *,cchar *) ;
extern int	getaf(cchar *,int) ;
extern int	getpwd(char *,int) ;
extern int	dialtcp(cchar *,cchar *,int,int,int) ;
extern int	dialtcpmux(cchar *,cchar *,int,cchar *,cchar **,int,int) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* local structures */

struct argparse {
	cchar	*hostname ;
	cchar	*portspec ;
	cchar	*svcspec ;
	cchar	*a ;		/* memory allocation */
	int		af ;
	int		to ;
} ;


/* local variables */

static cchar	*ops[] = {
	"to",
	"af",
	NULL
} ;

enum ops {
	op_to,
	op_af,
	op_overlast
} ;


/* forward references */

static int argparse_start(struct argparse *,cchar *) ;
static int argparse_finish(struct argparse *) ;


/* exported subroutines */


int opendialer_tcpmux(pr,prn,svc,of,om,argv,envv,to)
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
	cint	opts = 0 ;
	int		rs = SR_OK ;
	int		argc = 0 ;
	int		af = AF_UNSPEC ;
	int		fd = -1 ;
	cchar	*argz = NULL ;
	cchar	*hostname = NULL ;
	cchar	*portspec = NULL ;

#if	CF_DEBUGS
	{
	    int	i ;
	    debugprintf("opendialer_tcpmux: svc=%s\n",svc) ;
	    if (argv != NULL) {
	        for (i = 0 ; argv[i] != NULL ; i += 1) {
	            debugprintf("opendialer_tcpmux: a[%u]=%s\n",i,argv[i]) ;
	        }
	    }
	}
#endif /* CF_DEBUGS */

	if (svc[0] == '\0') return SR_INVALID ;

	if (argv != NULL) {
	    for (argc = 0 ; argv[argc] != NULL ; argc += 1) ;
	    argz = argv[0] ;
	}

	if ((rs >= 0) && (argz == NULL)) rs = SR_NOENT ;
	if ((rs >= 0) && (argz[0] == '\0')) rs = SR_NOENT ;
	if (rs < 0) goto ret0 ;

/* parse out everything */

/*
	tcpmux�<af>:<host>:<port>[,to=<to>][�<arg(s)>]
*/

	if ((rs = argparse_start(&ai,argz)) >= 0) {

#if	CF_DEBUGS
	debugprintf("opendialer_tcpmux: ai.to=%d\n",ai.to) ;
	debugprintf("opendialer_tcpmux: ai.af=%d\n",ai.af) ;
	debugprintf("opendialer_tcpmux: ai.hostname=%s\n",ai.hostname) ;
	debugprintf("opendialer_tcpmux: ai.portspec=%s\n",ai.portspec) ;
	debugprintf("opendialer_tcpmux: ai.svcspec=%s\n",ai.svcspec) ;
#endif

	    if (ai.to >= 0) to = ai.to ;
	    if (ai.af >= 0) af = ai.af ;
	    if (ai.portspec != NULL) {
	        portspec = ai.portspec ;
	        rs = getaf(svc,-1) ;
	        af = rs ;
	        hostname = ai.hostname ;
	    } else {
		portspec = ai.hostname ;
	        hostname = svc ;
	    }
	    svc = ai.svcspec ;

	    if (rs >= 0) {
		cint	esize = szof(cchar *) ;
		int	size ;
		char	*bp ;
		size = ((argc+1) * esize) ;
		if ((rs = uc_malloc(size,&bp)) >= 0) {
		    int		n = 0 ;
		    cchar	**av = (cchar **) bp ;

#if	CF_DEBUGS
		    debugprintf("opendialer_tcpmux: svc=%s\n",
			svc) ;
#endif

		    if (argc > 0) {
			int	i ;
		        for (i = 1 ; argv[i] != NULL ; i += 1) 
			    av[n++] = argv[i] ;
		    }
		    av[n] = NULL ;

#if	CF_DEBUGS
	            for (n = 0 ; av[n] != NULL ; n += 1)
	                debugprintf("opendialer_tcpmux: a[%u]=%s\n",n,av[n]) ;
#endif

/* continue */

	    	    if (rs >= 0) {
			rs = dialtcpmux(hostname,portspec,af,svc,av,to,opts) ;
			fd = rs ;
	    	    } /* end if */

		    uc_free(bp) ;
		} /* end if (memory allocation) */

	    } /* end if */

	    argparse_finish(&ai) ;
	} /* end if (argparse) */

ret0:

#if	CF_DEBUGS
	debugprintf("opendialer_tcpmux: ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opendialer_tcpmux) */


/* local subroutines */


/*
	tcpmux�[<af>]:<host>[:<port>]:<svc>[,to=<to>][,af=<af>][�<arg(s)>]
	tcpmux�[<af>]:<host>:<svc>[,to=<to>][,af=<af>][�<arg(s)>]
*/

static int argparse_start(struct argparse *app,cchar *args)
{
	int	rs = SR_OK ;
	int	hostl = 0 ;
	int	portl = 0 ;
	int	svcl = 0 ;
	int	opl = 0 ;

	cchar	*tp, *sp ;
	cchar	*hostp = NULL ;
	cchar	*portp = NULL ;
	cchar	*svcp = NULL ;
	cchar	*opp = NULL ;


	memclear(app) ;
	app->to = -1 ;
	app->af = -1 ;

	if (args[0] == '\0') goto ret0 ;

	if ((tp = strbrk(args,",:")) != NULL) {
	    int		oi ;
	    int		v ;
	    int		kl, vl ;
	    int		ch ;
	    cchar	*nsp ;
	    cchar	*kp, *vp ;
	    sp = (tp+1) ;
	    if (tp[0] == ':') {
	        hostp = args ;
	        hostl = (tp-args) ;
	        portp = (tp+1) ;
	        portl = -1 ;
	        svcp = (tp+1) ;
	        svcl = -1 ;
#if	CF_DEBUGS
	    debugprintf("opendialer_tcpmux/argparse_start: s=%s\n",sp) ;
#endif
	        if ((tp = strbrk(sp,":,")) != NULL) {
		    portl = (tp-sp) ;
		    svcl = (tp-sp) ;
	    	    if (tp[0] == ':') {
			svcp = (tp+1) ;
			svcl = -1 ;
	    	        sp = (tp+1) ;
	                if ((tp = strchr(sp,',')) != NULL) {
	                    svcl = (tp-sp) ;
	                    sp = (tp+1) ;
			}
	            } else {
			portp = NULL ;
	                svcp = sp ;
	                svcl = (tp-sp) ;
	    	        sp = (tp+1) ;
		    }
		} else {
		    portp = NULL ;
	            svcp = sp ;
	            svcl = -1 ;
		}
	    } else {
	        svcp = args ;
	        svcl = (tp-args) ;
	    }
#if	CF_DEBUGS
	    debugprintf("opendialer_tcpmux/argparse_start: ss=>%r<\n",
		svcp,svcl) ;
	    debugprintf("opendialer_tcpmux/argparse_start: s=>%s<\n",sp) ;
	    debugprintf("opendialer_tcpmux/argparse_start: p=>%r<\n",
		portp,portl) ;
#endif
	    ch = (sp[0] & 0xff) ;
	    while (ch) {
	        opp = sp ;
	        opl = -1 ;
	        if ((tp = strchr(sp,',')) != NULL) {
	            opl = (tp-sp) ;
		    nsp = (tp+1) ;
	        } else {
	            opl = lenstr(sp) ;
		    nsp = (sp+opl) ;
	        }
#if	CF_DEBUGS
	    debugprintf("opendialer_tcpmux/argparse_start: o=>%r<\n",
		opp,opl) ;
#endif
		kp = opp ;
		kl = opl ;
		vp = NULL ;
		vl = 0 ;
		if ((tp = strnchr(opp,opl,'=')) != NULL) {
		    kl = (tp-opp) ;
		    vp = (tp+1) ;
		    vl = (opp+opl) - (tp+1) ;
		}
#if	CF_DEBUGS
	    debugprintf("opendialer_tcpmux/argparse_start: k=%r\n",kp,kl) ;
		if (vp != NULL) 
	    debugprintf("opendialer_tcpmux/argparse_start: v=%r\n",vp,vl) ;
#endif
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
#if	CF_DEBUGS
	        debugprintf("opendialer_tcpmux/argparse_start: "
		    "while-bot rs=%d\n",rs) ;
#endif
		if (rs < 0) break ;
	        ch = (sp[0] & 0xff) ;
	    } /* end while */
	    if ((rs >= 0) && 
		((hostp != NULL) || (portp != NULL) || (svcp != NULL))) {
	        int	size = 0 ;
	        char	*bp ;
	        if (hostp != NULL) {
	            if (hostl < 0) hostl = lenstr(hostp) ;
	            size += (hostl + 1) ;
	        }
	        if (portp != NULL) {
	            if (portl < 0) portl = lenstr(portp) ;
	            size += (portl + 1) ;
	        }
	        if (svcp != NULL) {
	            if (svcl < 0) svcl = lenstr(svcp) ;
	            size += (svcl + 1) ;
	        }
	        if ((rs = uc_malloc(size,&bp)) >= 0) {
	            app->a = bp ;
	            if (hostp != NULL) {
	                app->hostname = bp ;
	                bp = strwcpy(bp,hostp,hostl) + 1 ;
	            }
	            if (portp != NULL) {
	                app->portspec = bp ;
	                bp = strwcpy(bp,portp,portl) + 1 ;
	            }
	            if (svcp != NULL) {
	                app->svcspec = bp ;
	                bp = strwcpy(bp,svcp,svcl) + 1 ;
	            }
	        }
	    } /* end if */
	} else
	    app->svcspec = args ;

ret0:
	return rs ;
}
/* end subroutine (argparse_start) */


static int argparse_finish(struct argparse *app)
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (app->a != NULL) {
	    rs1 = uc_free(app->a) ;
	    if (rs >= 0) rs = rs1 ;
	    app->a = NULL ;
	}

	app->hostname = NULL ;
	app->portspec = NULL ;
	return rs ;
}
/* end subroutine (argparse_finish) */



