/* opendialer_tcpnls */

/* open-dialer (tcpnls) */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */


/* revision history:

	= 2003-11-04, David A�D� Morano
        This code was started by taking the corresponding code from the
        TCP-family module. In retrospect, that was a mistake. Rather I should
        have started this code by using the corresponding UUX dialer module.

*/

/* Copyright � 2003 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is an open-dialer.

	The file-name corresponding to this dialer looks like:

		tcpnls�[<af>:]<host>[:<port>]:<svc>[,to=<to>][�<arg(s)>]

	Example:

		tcpnls�rca:daytime

	Synopsis:

	int opendialer_tcpnls(pr,prn,svc,of,om,argv,envv,to)
	const char	*pr ;
	const char	*prn ;
	const char	*svc ;
	int		of ;
	mode_t		om ;
	const char	**argv ;
	const char	**envv ;
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
	<0		error


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>

#include	<usystem.h>
#include	<localmisc.h>

#include	"opendialer_tcpnls.h"
#include	"defs.h"


/* local defines */

#define	ARGPARSE	struct argparse


/* external subroutines */

extern int	sncpy2(char *,int,const char *,const char *) ;
extern int	matstr(const char **,const char *,int) ;
extern int	cfdecti(const char *,int,int *) ;
extern int	cfdeci(const char *,int,int *) ;
extern int	findxfile(IDS *,char *,const char *) ;
extern int	getaf(const char *,int) ;
extern int	getpwd(char *,int) ;
extern int	dialtcp(const char *,const char *,int,int,int) ;
extern int	dialtcpnls(cchar *,cchar *,int,cchar *, int,int) ;

#if	CF_DEBUGS
extern int	debugprintf(const char *,...) ;
extern int	strlinelen(const char *,int,int) ;
#endif

extern char	*strwcpy(char *,const char *,int) ;
extern char	*strnchr(const char *,int,int) ;
extern char	*strnpbrk(const char *,int,const char *) ;


/* local structures */

struct argparse {
	const char	*hostname ;
	const char	*portspec ;
	const char	*svcspec ;
	const char	*a ;		/* memory allocation */
	int		af ;
	int		to ;
} ;


/* local variables */

static const char	*ops[] = {
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

static int argparse_start(struct argparse *,const char *) ;
static int argparse_finish(struct argparse *) ;


/* exported subroutines */


int opendialer_tcpnls(pr,prn,svc,of,om,argv,envv,to)
const char	*pr ;
const char	*prn ;
const char	*svc ;
int		of ;
mode_t		om ;
const char	**argv ;
const char	**envv ;
int		to ;
{
	ARGPARSE	ai ;
	const int	opts = 0 ;
	int		rs = SR_OK ;
	int		argc = 0 ;
	int		af = AF_UNSPEC ;
	int		fd = -1 ;
	const char	*argz = NULL ;
	const char	*hostname = NULL ;
	const char	*portspec = NULL ;


#if	CF_DEBUGS
	{
	    int	i ;
	    debugprintf("opendialer_tcpnls: svc=%s\n",svc) ;
	    if (argv != NULL) {
	        for (i = 0 ; argv[i] != NULL ; i += 1) {
	            debugprintf("opendialer_tcpnls: a[%u]=%s\n",i,argv[i]) ;
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
	tcpnls�<af>:<host>:<port>[,to=<to>][�<arg(s)>]
*/

	if ((rs = argparse_start(&ai,argz)) >= 0) {

#if	CF_DEBUGS
	debugprintf("opendialer_tcpnls: ai.to=%d\n",ai.to) ;
	debugprintf("opendialer_tcpnls: ai.af=%d\n",ai.af) ;
	debugprintf("opendialer_tcpnls: ai.hostname=%s\n",ai.hostname) ;
	debugprintf("opendialer_tcpnls: ai.portspec=%s\n",ai.portspec) ;
	debugprintf("opendialer_tcpnls: ai.svcspec=%s\n",ai.svcspec) ;
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
		const int	esize = sizeof(const char *) ;
		int	size ;
		char	*bp ;
		size = ((argc+1) * esize) ;
		if ((rs = uc_malloc(size,&bp)) >= 0) {
		    int		n = 0 ;
		    const char	**av = (const char **) bp ;

#if	CF_DEBUGS
		    debugprintf("opendialer_tcpnls: svc=%s\n",
			svc) ;
#endif

/* continue */

	    	    if (rs >= 0) {
			rs = dialtcpnls(hostname,portspec,af,svc,to,opts) ;
			fd = rs ;
	    	    } /* end if */

		    uc_free(bp) ;
		} /* end if (memory allocation) */

	    } /* end if */

	    argparse_finish(&ai) ;
	} /* end if (argparse) */

ret0:

#if	CF_DEBUGS
	debugprintf("opendialer_tcpnls: ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opendialer_tcpnls) */


/* local subroutines */


/*
	tcpnls�[<af>]:<host>[:<port>]:<svc>[,to=<to>][,af=<af>][�<arg(s)>]
	tcpnls�[<af>]:<host>:<svc>[,to=<to>][,af=<af>][�<arg(s)>]
*/

static int argparse_start(struct argparse *app,const char *args)
{
	int	rs = SR_OK ;
	int	hostl = 0 ;
	int	portl = 0 ;
	int	svcl = 0 ;
	int	opl = 0 ;

	const char	*tp, *sp ;
	const char	*hostp = NULL ;
	const char	*portp = NULL ;
	const char	*svcp = NULL ;
	const char	*opp = NULL ;


	memset(app,0,sizeof(struct argparse)) ;
	app->to = -1 ;
	app->af = -1 ;

	if (args[0] == '\0') goto ret0 ;

	if ((tp = strpbrk(args,",:")) != NULL) {
	    int		oi ;
	    int		v ;
	    int		kl, vl ;
	    int		ch ;
	    const char	*nsp ;
	    const char	*kp, *vp ;
	    sp = (tp+1) ;
	    if (tp[0] == ':') {
	        hostp = args ;
	        hostl = (tp-args) ;
	        portp = (tp+1) ;
	        portl = -1 ;
	        svcp = (tp+1) ;
	        svcl = -1 ;
#if	CF_DEBUGS
	    debugprintf("opendialer_tcpnls/argparse_start: s=%s\n",sp) ;
#endif
	        if ((tp = strpbrk(sp,":,")) != NULL) {
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
	    debugprintf("opendialer_tcpnls/argparse_start: ss=>%t<\n",
		svcp,svcl) ;
	    debugprintf("opendialer_tcpnls/argparse_start: s=>%s<\n",sp) ;
	    debugprintf("opendialer_tcpnls/argparse_start: p=>%t<\n",
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
	            opl = strlen(sp) ;
		    nsp = (sp+opl) ;
	        }
#if	CF_DEBUGS
	    debugprintf("opendialer_tcpnls/argparse_start: o=>%t<\n",
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
	    debugprintf("opendialer_tcpnls/argparse_start: k=%t\n",kp,kl) ;
		if (vp != NULL) 
	    debugprintf("opendialer_tcpnls/argparse_start: v=%t\n",vp,vl) ;
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
	        debugprintf("opendialer_tcpnls/argparse_start: "
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
	            if (hostl < 0) hostl = strlen(hostp) ;
	            size += (hostl + 1) ;
	        }
	        if (portp != NULL) {
	            if (portl < 0) portl = strlen(portp) ;
	            size += (portl + 1) ;
	        }
	        if (svcp != NULL) {
	            if (svcl < 0) svcl = strlen(svcp) ;
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



