/* opendialer_prog SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open-dialer (prog) */
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

  	Name:
	opendialer_prog

	Description:
	This is an open-dialer.

	Synopsis:
	int opendialer_prog(pr,prn,svc,of,om,argv,envv,to)
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

	= Notes

	Q. Why use |vecstr_addpath()| rather than |vecstr_addpathclean()|?
	A. We only need to traverse the path list once, so the overhead
	   of using |vecstr_addpathclean()| is probably not justified.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"opendialer_prog.h"
#include	"defs.h"

import libutil ;

/* local defines */

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#define	EXTRABIN	"/usr/extra/bin"

#define	NDF		"opendialer_prog.nd"


/* external subroutines */

extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	findxfile(ids *,char *,cchar *) ;
extern int	getpwd(char *,int) ;
extern int	getprogpath(ids *,vecstr *,char *,cchar *,int) ;
extern int	vecstr_addpath(vecstr *,cchar *,int) ;
extern int	vecstr_addpathclean(vecstr *,cchar *,int) ;
extern int	vecstr_addcspath(vecstr *) ;

#if	CF_DEBUGS || CF_DEBUGN
extern int	debugprintf(cchar *,...) ;
extern int	nprintf(cchar *,cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* forward references */

static int findprog(cchar *,char *,cchar *) ;
static int vecstr_addpr(vecstr *,char *,cchar *) ;


/* local variables */

static cchar	*bins[] = {
	"bin",
	"sbin",
	NULL
} ;


/* exported subroutines */


/* ARGSUSED */
int opendialer_prog(pr,prn,svc,of,om,argv,envv,to)
cchar	*pr ;
cchar	*prn ;
cchar	*svc ;
int		of ;
mode_t		om ;
cchar	**argv ;
cchar	**envv ;
int		to ;
{
	int		rs = SR_OK ;
	char		progfname[MAXPATHLEN+1] = { 0 } ;

#if	CF_DEBUGS
	debugprintf("opendialer_prog: svc=%s\n",svc) ;
#endif

	if (svc[0] == '\0') return SR_INVALID ;

	if (strchr(svc,'/') == NULL) {
	    rs = findprog(pr,progfname,svc) ;
	    svc = progfname ;
	} else if (svc[0] != '/') {
	    char	pwd[MAXPATHLEN+1] ;
	    if ((rs = getpwd(pwd,MAXPATHLEN)) >= 0) {
		rs = mkpath2(progfname,pwd,svc) ;
		svc = progfname ;
	    }
	} /* end if (simple name) */

	if (rs >= 0) {
	    rs = uc_openprog(svc,of,argv,envv) ;
	}

	return rs ;
}
/* end subroutine (opendialer_prog) */


/* local subroutines */


static int findprog(cchar *pr,char *progfname,cchar *svc)
{
	ids		id ;
	int		rs ;
	int		rs1 ;
	int		pl = 0 ;
	if ((rs = ids_load(&id)) >= 0) {
	    vecstr	p ;
	    if ((rs = vecstr_start(&p,20,0)) >= 0) {
		cchar	*pp = getenv(VARPATH) ;
		if ((pp != NULL) && (pp[0] != '\0')) {
		    rs = vecstr_addpath(&p,pp,-1) ;
		} else {
		    if ((rs = vecstr_addcspath(&p)) >= 0) {
			rs = vecstr_add(&p,EXTRABIN,-1) ;
		    }
		}
		if (rs >= 0) {
		    const int	rsn = SR_NOENT ;
		    if ((rs = getprogpath(&id,&p,progfname,svc,-1)) == rsn) {
			if ((rs = vecstr_addpr(&p,progfname,pr)) > 0) {
		    	    rs = getprogpath(&id,&p,progfname,svc,-1) ;
		            pl = rs ;
			} else if (rs == 0) {
			    rs = rsn ;
			}
		    } else {
			pl = rs ;
		    }
		} /* end if (ok) */
		rs1 = vecstr_finish(&p) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (findprog) */


static int vecstr_addpr(vecstr *plp,char *rbuf,cchar *pr)
{
	vecstr		mores ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = vecstr_start(&mores,2,0)) >= 0) {
	    const int	rsn = SR_NOTFOUND ;
	    int		rl ;
	    int		i ;
	    for (i = 0 ; bins[i] != NULL ; i += 1) {
		if ((rs = mkpath2(rbuf,pr,bins[i])) > 0) {
		    rl = rs ;
		    if ((rs = vecstr_find(plp,rbuf)) == rsn) {
			n += 1 ;
			rs = vecstr_add(&mores,rbuf,rl) ;
		    }
		}
		if (rs < 0) break ;
	    } /* end for */
	    if (rs >= 0) {
		if (n > 0) {
		    if ((rs = vecstr_delall(plp)) >= 0) {
		        cchar	*pp ;
		        for (i = 0 ; vecstr_get(&mores,i,&pp) >= 0 ; i += 1) {
			    if (pp != NULL) {
				rs = vecstr_add(plp,pp,-1) ;
			    }
			    if (rs < 0) break ;
		        } /* end for */
		    } /* end if (vecstr_delall) */
		} /* end if (positive) */
	    } /* end if (ok) */
	    rs1 = vecstr_finish(&mores) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (vecstr_addpr) */


