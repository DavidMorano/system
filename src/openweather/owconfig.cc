/* owconfig SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* handle open-weather (OW) configuration functions */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* non-switchable debug print-outs */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2004,2005,2011 David A­D­ Morano.  All rights reserved. */

/**************************************************************************

  	Description:
	These subroutines form part of the OW subroutine (yes, getting
	a little bit more complicated every day now).

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>
#include	<clanguage.h>
#include	<csysbase.h>
#include	<keyopt.h>
#include	<vecstr.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<logfile.h>
#include	<lfm.h>
#include	<getusername.h>
#include	<getutmpent.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"ow.h"
#include	"owconfig.h"


/* local defines */

#ifndef	PBUFLEN
#define	PBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif

#ifndef	IPCBUFLEN
#define	IPCBUFLEN	MSGBUFLEN
#endif

#define	CMSGBUFLEN	256
#define	NIOVECS		1

#undef	TMPDMODE
#define	TMPDMODE	0777


/* external subroutines */

extern int	ow_setfname(OW *,char *,cchar *,int,
			int,cchar *,cchar *,cchar *) ;


/* external variables */

extern char	**environ ;


/* local structures */


/* forward references */

local int	owconfig_setcookies(OWCONFIG *) ;


/* local variables */

#ifdef	COMMENT
static cchar	*sched1[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	NULL
} ;
#endif /* COMMENT */

static cchar	*params[] = {
	"whost",
	"host",
	"wprefix",
	"prefix",
	"ws",
	"defws",
	"intpoll",
	"poll",
	"logfile",
	"logsys",
	NULL
} ;

enum params {
	param_whost,
	param_host,
	param_wprefix,
	param_prefix,
	param_ws,
	param_defws,
	param_intpoll,
	param_poll,
	param_logfile,
	param_logsys,
	param_overlast
} ;


/* exported subroutines */


int owconfig_start(cfp,lip,cfname)
OWCONFIG	*cfp ;
OW		*lip ;
cchar	*cfname ;
{
	int	rs = SR_OK ;
	int	f_open = FALSE ;


	if (cfp == NULL)
	    return SR_FAULT ;

	memset(cfp,0,sizeof(OWCONFIG)) ;
	cfp->lip = lip ;

#if	CF_DEBUG
	{
	    debugprintf("owconfig_start: cfname=%s\n",cfname) ;
	    debugprintf("owconfig_start: nodename=%s\n",lip->nodename) ;
	    debugprintf("owconfig_start: domainname=%s\n",lip->domainname) ;
	}
#endif

	if (cfname == NULL)
	    goto ret0 ;

	if (cfname[0] == '\0')
	    goto ret0 ;

	rs = uc_mallocstrw(cfname,-1,&cfp->cfname) ;
	if (rs < 0) goto bad0 ;

	rs = paramfile_open(&cfp->p,NULL,cfname) ;
	if (rs < 0) goto bad1 ;

	rs = expcook_start(&cfp->cooks) ;
	if (rs < 0) goto bad2 ;

	rs = owconfig_setcookies(cfp) ;
	if (rs < 0) goto bad3 ;

	f_open = TRUE ;
	cfp->fl.p = TRUE ;
	rs = owconfig_read(cfp) ;
	if (rs < 0) goto bad3 ;

ret0:

#if	CF_DEBUG
	    debugprintf("owconfig_start: ret rs=%d \n",rs) ;
#endif

	return (rs >= 0) ? f_open : rs ;

/* bad stuff */
bad3:
	expcook_finish(&cfp->cooks) ;

bad2:
	paramfile_close(&cfp->p) ;

bad1:
	if (cfp->cfname != NULL) {
	    uc_free(cfp->cfname) ;
	    cfp->cfname = NULL ;
	}

bad0:
	goto ret0 ;
}
/* end subroutine (owconfig_start) */


int owconfig_finish(cfp)
OWCONFIG	*cfp ;
{
	int	rs = SR_NOTOPEN ;
	int	rs1 ;


	if (cfp == NULL)
	    return SR_FAULT ;

	if (cfp->fl.p) {

	    rs = SR_OK ;
	    rs1 = expcook_finish(&cfp->cooks) ;
	    if (rs >= 0) rs = rs1 ;

	    rs1 = paramfile_close(&cfp->p) ;
	    if (rs >= 0) rs = rs1 ;

	} /* end if */

	if (cfp->cfname != NULL) {
	    uc_free(cfp->cfname) ;
	    cfp->cfname = NULL ;
	}

	return rs ;
}
/* end subroutine (owconfig_finish) */


int owconfig_check(cfp)
OWCONFIG	*cfp ;
{
	OW	*lip = cfp->lip ;

	int	rs = SR_NOTOPEN ;
	int	f = FALSE ;


	if (cfp == NULL)
	    return SR_FAULT ;

	if (cfp->fl.p) {

#if	CF_DEBUG
	debugprintf("msumain/owconfig_check: paramfile_check()\n") ;
#endif

	    rs = paramfile_check(&cfp->p,lip->daytime) ;

#if	CF_DEBUG
	debugprintf("msumain/owconfig_check: paramfile_check() rs=%d\n",rs) ;
#endif

	    if (rs > 0) {

	        f = TRUE ;
	        rs = owconfig_read(cfp) ;

#if	CF_DEBUG
	debugprintf("msumain/owconfig_check: owconfig_read() rs=%d\n",rs) ;
#endif

	    }

	} /* end if */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (owconfig_check) */


int owconfig_read(cfp)
OWCONFIG	*cfp ;
{
	paramfile		*pfp = &cfp->p ;
	paramfile_cur		cur ;
	paramfile_ent		pe ;

	OW		*lip = cfp->lip ;
	EXPCOOK		*ckp = &cfp->cooks ;

	cint	elen = EBUFLEN ;

	int	rs = SR_NOTOPEN ;
	int	rs1 ;
	int	pi ;
	int	kl, vl, el ;
	int	v ;

	char	pbuf[PBUFLEN + 1] ;
	char	ebuf[EBUFLEN + 1] ;

	cchar	*kp, *vp ;
	cchar	*ep ;


#if	CF_DEBUG
	    debugprintf("owconfig_read: f_p=%u\n",cfp->fl.p) ;
#endif

	if (cfp == NULL)
	    return SR_FAULT ;

	if (! cfp->fl.p)
	    goto ret0 ;

	ep = ebuf ;
	if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {

	    while (rs >= 0) {

	        kl = paramfile_curenum(pfp,&cur,&pe,pbuf,PBUFLEN) ;
	        if (kl == SR_NOTFOUND)
		    break ;

		rs = kl ;
		if (rs < 0)
		    break ;

	    kp = pe.key ;
	    vp = pe.value ;
	    vl = pe.vlen ;

	    pi = matostr(params,2,kp,kl) ;
	    if (pi < 0) continue ;

	    ebuf[0] = '\0' ;
	    el = 0 ;
	    if (vl > 0) {
	        el = expcook_exp(ckp,0,ebuf,elen,vp,vl) ;
	        if (el >= 0)
	            ebuf[el] = '\0' ;
	    } /* end if */

#if	CF_DEBUG
	    {
	        debugprintf("progconfigread: ebuf=>%r<\n",ebuf,el) ;
	        debugprintf("progconfigread: param=%s(%u)\n",
	            params[pi],pi) ;
	    }
#endif

	    if (el < 0)
	        continue ;

	    switch (pi) {

		param_whost:
		param_host:
		    if (el > 0)
			rs = ow_setentry(lip,&lip->whost,ep,el) ;
		    break ;

		param_wprefix:
		param_prefix:
		    if (el > 0)
			rs = ow_setentry(lip,&lip->wprefix,ep,el) ;
		    break ;

		param_ws:
		param_defws:
		    if (el > 0)
			rs = ow_setentry(lip,&lip->ws,ep,el) ;
		    break ;

	        case param_intpoll:
	        case param_poll:
		    if (el > 0) {
		        rs1 = cfdecti(ebuf,el,&v) ;
	                if ((rs1 >= 0) && (v >= 0))
			    lip->intpoll = v ;
		    }
		    break ;

		param_logfile:
		    if (el > 0)
			rs = ow_setentry(lip,&lip->logfname,ep,el) ;
		    break ;

		param_logsys:
		    if (el > 0)
			rs = ow_setentry(lip,&lip->logfacility,ep,el) ;
		    break ;

		} /* end switch */

	    } /* end while (fetching) */

	    paramfile_curend(&cfp->p,&cur) ;
	} /* end if (parameters) */

ret0:
	return rs ;
}
/* end subroutine (owconfig_read) */


local int owconfig_setcookies(cfp)
OWCONFIG	*cfp ;
{
	OW	*lip = cfp->lip ;

	EXPCOOK	*ckp = &cfp->cooks ;

	int	rs ;


	rs = expcook_add(ckp,"R",lip->pr,-1) ;

	if ((rs >= 0) && (lip->rn != NULL))
	    rs = expcook_add(ckp,"RN",lip->rn,-1) ;

	if (lip->nodename != NULL) {
	    rs = expcook_add(ckp,"N",lip->nodename,-1) ;
	    if (rs >= 0)
		rs = expcook_add(ckp,"D",lip->domainname,-1) ;
	}

	if (rs >= 0) {
	    int		hnl ;
	    char	hostname[MAXHOSTNAMELEN + 1] ;
	    hnl = snsds(hostname,MAXHOSTNAMELEN,lip->nodename,lip->domainname) ;
	    rs = expcook_add(ckp,"H",hostname,hnl) ;
	}

	return rs ;
}
/* end subroutine (config_setcookies) */



