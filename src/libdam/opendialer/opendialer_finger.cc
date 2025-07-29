/* opendialer_finger SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open-dialer (finger) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

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
		finger¥[<af>:]<host>[:<port>]:<svc>[,to=<to>][­<arg(s)>]

	Example:
		finger¥rca:daytime

	Synopsis:
	int opendialer_finger(pr,prn,svc,of,om,argv,envv,to)
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
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<baops.h>
#include	<ids.h>
#include	<keyopt.h>
#include	<vecstr.h>
#include	<vechand.h>
#include	<sbuf.h>
#include	<nulstr.h>
#include	<logfile.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strx.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"opendialer_finger.h"
#include	"defs.h"

import libutil ;

/* local defines */

#ifndef	LOCALHOST
#define	LOCALHOST	"localhost"
#endif

#ifndef	PORTSPEC_FINGER
#define	PORTSPEC_FINGER	"finger"
#endif

#define	ARGPARSE	struct argparse

#ifndef	SVCLEN
#define	SVCLEN		MAXNAMELEN
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct argparse {
	cchar	*s1 ;
	cchar	*s2 ;
	cchar	*s3 ;
	cchar	*a ;		/* memory allocation */
	int		af ;
	int		to ;
	int		f_long ;
} ;


/* local variables */

enum ops {
	op_to,
	op_af,
	op_long,
	op_overlast
} ;

constexpr cpcchar	ops[] = {
	"to",
	"af",
	"long",
	NULL
} ;


/* forward references */

static int argparse_start(struct argparse *,cchar *) noex ;
static int argparse_finish(struct argparse *) noex ;


/* exported variables */


/* exported subroutines */

int opendialer_finger(pr,prn,svc,of,om,argv,envv,to)
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
	int		argc = 0 ;
	int		af = AF_UNSPEC ;
	int		opts = 0 ;
	int		fd = -1 ;
	cchar	*argz = NULL ;
	cchar	*hostname = NULL ;
	cchar	*portspec = NULL ;

#if	CF_DEBUGS
	{
	    int	i ;
	    debugprintf("opendialer_finger: svc=%s\n",svc) ;
	    if (argv != NULL) {
	        for (i = 0 ; argv[i] != NULL ; i += 1) {
	            debugprintf("opendialer_finger: a[%u]=%s\n",i,argv[i]) ;
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
	finger¥<af>:<host>:<port>[,to=<to>][­<arg(s)>]
*/

#if	CF_DEBUGS
	debugprintf("opendialer_finger: argz=%s\n",argz) ;
#endif

	if ((rs = argparse_start(&ai,argz)) >= 0) {

#if	CF_DEBUGS
	    debugprintf("opendialer_finger: svc=%s\n",svc) ;
	    debugprintf("opendialer_finger: ai.s1=%s\n",ai.s1) ;
	    debugprintf("opendialer_finger: ai.s2=%s\n",ai.s2) ;
	    debugprintf("opendialer_finger: ai.s3=%s\n",ai.s3) ;
	    debugprintf("opendialer_finger: ai.to=%d\n",ai.to) ;
	    debugprintf("opendialer_finger: ai.af=%d\n",ai.af) ;
	    debugprintf("opendialer_finger: ai.f_long=%u\n",ai.f_long) ;
#endif /* CF_DEBUGS */

	    if (ai.to >= 0) to = ai.to ;
	    if (ai.af >= 0) af = ai.af ;
	    if ((ai.s1 != NULL) && (strcmp(svc,argz) != 0)) {
	        if (ai.s2 != NULL) {
	            if (ai.s3 != NULL) {
			hostname = ai.s1 ;
	                portspec = ai.s2 ;
	                rs = getaf(svc,-1) ;
	                af = rs ;
	                svc = ai.s3 ;
	            } else {
		        hostname = svc ;
	                portspec = ai.s1 ;
			svc = ai.s2 ;
	            }
		} else {
		    hostname = svc ;
		    svc = ai.s1 ;
		}
	    } else {
		hostname = LOCALHOST ;
	    }

#if	CF_DEBUGS
	debugprintf("opendialer_finger: args rs=%d \n",rs) ;
	debugprintf("opendialer_finger: hn=%s ps=%s svc=%s\n",
		hostname,portspec,svc) ;
#endif

	    if (rs >= 0) {
		cint	esize = szof(cchar *) ;
		int		size ;
		char		*bp ;
		size = ((argc+1) * esize) ;
		if ((rs = uc_malloc(size,&bp)) >= 0) {
		    int		n = 0 ;
		    cchar	**av = (cchar **) bp ;

#if	CF_DEBUGS
		    debugprintf("opendialer_finger: svc=%s\n",
			svc) ;
#endif

		    if (argc > 0) {
			int	i ;
		        for (i = 1 ; argv[i] != NULL ; i += 1) {
			    av[n++] = argv[i] ;
			}
		    }
		    av[n] = NULL ;

#if	CF_DEBUGS
	            for (n = 0 ; av[n] != NULL ; n += 1) {
	                debugprintf("opendialer_finger: a[%u]=%s\n",n,av[n]) ;
		    }
#endif

/* continue */

#if	CF_DEBUGS
		    debugprintf("opendialer_finger: mid rs=%d f_long=%u\n",
			rs,ai.f_long) ;
		    debugprintf("opendialer_finger: hostname=%s\n",hostname) ;
		    debugprintf("opendialer_finger: portspec=%s\n",portspec) ;
		    debugprintf("opendialer_finger: svc=%s\n",svc) ;
#endif

	    	    if (rs >= 0) {
			if (ai.f_long) opts |= O_NOCTTY ;
			rs = dialfinger(hostname,portspec,af,svc,av,to,opts) ;
			fd = rs ;
	    	    } /* end if */

		    uc_free(bp) ;
		} /* end if (memory allocation) */

	    } /* end if */

	    argparse_finish(&ai) ;
	} /* end if (argparse) */

ret0:

#if	CF_DEBUGS
	debugprintf("opendialer_finger: ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opendialer_finger) */


/* local subroutines */


/*
	finger¥[<af>]:<host>[:<port>]:<svc>[,to=<to>][,af=<af>][­<arg(s)>]
	finger¥[<af>]:<host>:<svc>[,to=<to>][,af=<af>][­<arg(s)>]
*/

static int argparse_start(struct argparse *app,cchar *args)
{
	int		rs = SR_OK ;
	int		s1l = 0 ;
	int		s2l = 0 ;
	int		s3l = 0 ;
	int		opl = 0 ;
	cchar	*tp, *sp ;
	cchar	*s1p = NULL ;
	cchar	*s2p = NULL ;
	cchar	*s3p = NULL ;
	cchar	*opp = NULL ;

	memclear(app) ;
	app->to = -1 ;
	app->af = -1 ;

#if	CF_DEBUGS
	    debugprintf("opendialer_finger/argparse_start: args=%s\n",args) ;
#endif

	if ((args == NULL) || (args[0] == '\0')) goto ret0 ;

	if ((tp = strbrk(args,",:")) != NULL) {
	    int		oi ;
	    int		v ;
	    int		kl, vl ;
	    int		ch ;
	    cchar	*nsp ;
	    cchar	*kp, *vp ;
	    s1p = args ;
	    s1l = (tp-args) ;
	    sp = (tp+1) ;
	    if (tp[0] == ':') {
	        s2p = sp ;
	        s2l = -1 ;
#if	CF_DEBUGS
	    debugprintf("opendialer_finger/argparse_start: s=%s\n",sp) ;
#endif
	        if ((tp = strbrk(sp,":,")) != NULL) {
		    s2l = (tp-sp) ;
	    	    sp = (tp+1) ;
	    	    if (tp[0] == ':') {
			s3p = sp ;
			s3l = -1 ;
	                if ((tp = strchr(sp,',')) != NULL) {
	                    s3l = (tp-sp) ;
	                    sp = (tp+1) ;
			}
		    }
		}
	    }
#if	CF_DEBUGS
	    debugprintf("opendialer_finger/argparse_start: s1=>%r<\n",
		s1p,s1l) ;
	    debugprintf("opendialer_finger/argparse_start: s2=>%r<\n",
		s2p,s2l) ;
	    debugprintf("opendialer_finger/argparse_start: s3=>%r<\n",
		s3p,s3l) ;
	    debugprintf("opendialer_finger/argparse_start: s=>%s<\n",sp) ;
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
	    debugprintf("opendialer_finger/argparse_start: op=>%r<\n",opp,opl) ;
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
	        debugprintf("opendialer_finger/argparse_start: k=%r\n",kp,kl) ;
		if (vp != NULL) 
	            debugprintf("opendialer_finger/argparse_start: v=%r\n",
			vp,vl) ;
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
	            case op_long:
	                app->f_long = TRUE ;
		        if (vl > 0) {
	                    rs = optbool(vp,vl) ;
	                    app->f_long = (rs > 0) ;
		        }
			break ;
	            } /* end switch */
		} /* end if */
	        sp = nsp ;
#if	CF_DEBUGS
	        debugprintf("opendialer_finger/argparse_start: "
		    "while-bot rs=%d\n",rs) ;
#endif
		if (rs < 0) break ;
	        ch = (sp[0] & 0xff) ;
	    } /* end while */
	    if ((rs >= 0) && 
		((s1p != NULL) || (s2p != NULL) || (s3p != NULL))) {
	        int	size = 0 ;
	        char	*bp ;
	        if (s1p != NULL) {
	            if (s1l < 0) s1l = lenstr(s1p) ;
	            size += (s1l + 1) ;
	        }
	        if (s2p != NULL) {
	            if (s2l < 0) s2l = lenstr(s2p) ;
	            size += (s2l + 1) ;
	        }
	        if (s3p != NULL) {
	            if (s3l < 0) s3l = lenstr(s3p) ;
	            size += (s3l + 1) ;
	        }
	        if ((rs = uc_malloc(size,&bp)) >= 0) {
	            app->a = bp ;
	            if (s1p != NULL) {
	                app->s1 = bp ;
	                bp = strwcpy(bp,s1p,s1l) + 1 ;
	            }
	            if (s2p != NULL) {
	                app->s2 = bp ;
	                bp = strwcpy(bp,s2p,s2l) + 1 ;
	            }
	            if (s3p != NULL) {
	                app->s3 = bp ;
	                bp = strwcpy(bp,s3p,s3l) + 1 ;
	            }
	        }
	    } /* end if */
	} else
	    app->s1 = args ;

ret0:
	return rs ;
}
/* end subroutine (argparse_start) */


static int argparse_finish(struct argparse *app)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (app->a != NULL) {
	    rs1 = uc_free(app->a) ;
	    if (rs >= 0) rs = rs1 ;
	    app->a = NULL ;
	}

	app->s1 = NULL ;
	app->s2 = NULL ;
	app->s3 = NULL ;
	return rs ;
}
/* end subroutine (argparse_finish) */


