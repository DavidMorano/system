/* sd_ussnls SUPPORT (Sys-Dialer) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "ussnls" dialer */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 1998-11-04, David A­D­ Morano
	This was created as one of the first dialer modules for the
	SYSDIALER object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This is a SYSDIALER module.

	Synopsis:
	ussnls <path>

	Arguments:
	<path>		pathto socket file

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<baops.h>
#include	<keyopt.h>
#include	<field.h>
#include	<ids.h>
#include	<userinfo.h>
#include	<nulstr.h>
#include	<localmisc.h>

#include	"sysdialer.h"
#include	"sd_ussinfo.h"
#include	"sd_ussnls.h"
#include	"envs.h"


/* local defines */

#define	USSNLS_MNAME	"ussnls"
#define	USSNLS_VERSION	"0"
#define	USSNLS_INAME	""
#define	USSNLS_MF1	(SYSDIALER_MFULL | SYSDIALER_MHALFOUT)
#define	USSNLS_MF2	(SYSDIALER_MCOR | SYSDIALER_MCO)
#define	USSNLS_MF3	(SYSDIALER_MHALFIN)
#define	USSNLS_MF	(USSNLS_MF1 | USSNLS_MF2|USSNLS_MF3)

#define	USSNLS_VARPR		"LOCAL"
#define	USSNLS_PR		"/usr/add-on/local"
#define	USSNLS_LOGDNAME		"log"
#define	USSNLS_LOGFNAME		SYSDIALER_LF

#ifndef	SVCNAMELEN
#define	SVCNAMELEN	32
#endif

#define	ARGBUFLEN	(MAXPATHLEN + 35)

#define	NPARG		2	/* number of positional arguments */
#define	MAXARGINDEX	100
#define	NARGPRESENT	(MAXARGINDEX/8 + 1)


/* external subroutines */


/* external variables */

extern char	**environ ;


/* local structures */


/* forward references */

static int ussnls_logbegin(USSNLS *,cchar *,cchar *) ;
static int ussnls_logend(USSNLS *) ;
static int ussnls_logstuff(USSNLS *,USSINFO *) ;


/* local variables */


/* exported variables */

SYSDIALER_INFO	ussnls = {
	USSNLS_MNAME,
	USSNLS_VERSION,
	USSNLS_INAME,
	sizeof(USSNLS),
	USSNLS_MF
} ;


/* exported subroutines */

/* ARGSUSED */
int ussnls_open(op,ap,hostname,svcname,av)
USSNLS		*op ;
SYSDIALER_ARGS	*ap ;
cchar	hostname[] ;
cchar	svcname[] ;
cchar	*av[] ;
{
	USSINFO		si, *sip = &si ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	opts = 0 ;


	if (op == nullptr) return SR_FAULT ;
	if (hostname == nullptr) return SR_FAULT ;

	if (hostname[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(USSNLS)) ;

#if	CF_DEBUGS
	{
	    int	i ;
	    debugprintf("ussnls_open: entered hostname=%s svcname=%s\n",
	        hostname,svcname) ;
	    if (ap->argv != nullptr) {
	        for (i = 0 ; ap->argv[i] != nullptr ; i += 1) {
	            debugprintf("ussnls_open: a%u=>%s<\n",i,ap->argv[i]) ;
	        }
	    }
	}
#endif /* CF_DEBUGS */

	if ((rs = ussinfo_start(sip,op,&ussnls,ap,hostname,svcname)) >= 0) {

	    rs = ussinfo_defaults(sip) ;

#if	CF_DEBUGS
	debugprintf("ussnls_open: ussinfo_logstuff()\n") ;
	debugprintf("ussnls_open: f_log=%u\n",sip->fl.log) ;
#endif

	if ((rs >= 0) && sip->fl.log)
	    rs = ussnls_logstuff(op,sip) ;

/* parse the port-specification if we have one */

	if (rs >= 0)
	    rs = ussinfo_addrparse(sip) ;

#if	CF_DEBUGS
	debugprintf("ussnls_open: af=%u\n",sip->af) ;
	debugprintf("ussnls_open: hostname=%s\n",hostname) ;
	debugprintf("ussnls_open: portspec=%s\n",sip->portspec) ;
	debugprintf("ussnls_open: svcname=%s\n",svcname) ;
#endif

	if ((rs >= 0) && (sip->portspec == nullptr))
	    rs = SR_NOENT ;

/* OK, do the dial */

	if (rs >= 0) {
	    switch (sip->af) {
	    case AF_UNSPEC:
	    case AF_UNIX:
	        if (sip->portspec != nullptr) {
		    cchar *portspec = sip->portspec ;
	            rs = dialussnls(portspec,svcname,sip->to,opts) ;
		} else
	            rs = SR_NOENT ;
	        break ;

	    } /* end switch */

#if	CF_DEBUGS
	    debugprintf("ussnls_open: dial() rs=%d\n",rs) ;
#endif

	} /* end if */
	op->fd = rs ;

	if (rs >= 0) {
	    op->magic = USSNLS_MAGIC ;
	    uc_closeonexec(op->fd,true) ;
	}

	    rs1 = ussinfo_finish(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ussinfo) */

	if ((rs < 0) && (op->fd >= 0)) {
	    u_close(op->fd) ;
	    op->fd = -1 ;
	    op->magic = 0 ;
	}

#if	CF_DEBUGS
	debugprintf("ussnls_open: ret rs=%d fd=%d\n",rs,op->fd) ;
#endif

	return (rs >= 0) ? op->fd : rs ;
}
/* end subroutine (ussnls_open) */


int ussnls_reade(op,buf,buflen,to,opts)
USSNLS		*op ;
char		buf[] ;
int		buflen ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_reade(op->fd,buf,buflen,to,opts) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (ussnls_reade) */


int ussnls_recve(op,buf,buflen,flags,to,opts)
USSNLS		*op ;
char		buf[] ;
int		buflen ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recve(op->fd,buf,buflen,flags,to,opts) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (ussnls_recve) */


int ussnls_recvfrome(op,buf,buflen,flags,sap,salenp,to,opts)
USSNLS		*op ;
char		buf[] ;
int		buflen ;
int		flags ;
void		*sap ;
int		*salenp ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recvfrome(op->fd,buf,buflen,flags,sap,salenp,to,opts) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (ussnls_recvfrome) */


int ussnls_recvmsge(op,msgp,flags,to,opts)
USSNLS		*op ;
struct msghdr	*msgp ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recvmsge(op->fd,msgp,flags,to,opts) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (ussnls_recvmsge) */


int ussnls_write(op,buf,buflen)
USSNLS		*op ;
cchar	buf[] ;
int		buflen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_writen(op->fd,((void *) buf),buflen) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (ussnls_write) */


int ussnls_send(op,buf,buflen,flags)
USSNLS		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_send(op->fd,buf,buflen,flags) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (ussnls_send) */


int ussnls_sendto(op,buf,buflen,flags,sap,salen)
USSNLS		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
void		*sap ;
int		salen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendto(op->fd,buf,buflen,flags,sap,salen) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (ussnls_sendto) */


int ussnls_sendmsg(op,msgp,flags)
USSNLS		*op ;
struct msghdr	*msgp ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendmsg(op->fd,msgp,flags) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (ussnls_sendmsg) */


/* shutdown */
int ussnls_shutdown(op,cmd)
USSNLS		*op ;
int		cmd ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_shutdown(op->fd,cmd) ;

	if (rs > 0)
	    op->tlen += rs ;

	return rs ;
}
/* end subroutine (ussnls_shutdown) */


/* close the connection */
int ussnls_close(op)
USSNLS		*op ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USSNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs1 = u_close(op->fd) ;
	if (rs >= 0) rs = rs1 ;

	if (op->open.log) {
	    logfile_printf(&op->lh,"bytes=%u",op->tlen) ;
	    rs1 = ussnls_logend(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (ussnls_close) */


/* private subroutines */


static int ussnls_logbegin(USSNLS *op,cchar *lfname,cchar *logid)
{
	int	rs = SR_OK ;
	int	f = op->open.log ;

	if (! op->open.log) {
	    if ((rs = logfile_open(&op->lh,lfname,0,0666,logid)) >= 0) {
	        op->open.log = true ;
		f = true ;
	    } else if (isNotPresent(rs))
	        rs = SR_OK ;
	} /* end if (needed opening) */

	return (rs >= 0) ? f : rs ;
}
/* end if (ussnls_logbegin) */


static int ussnls_logend(USSNLS *op)
{
	int	rs = SR_OK ;
	int	rs1 ;

	if (op->open.log) {
	    op->open.log = false ;
	    rs1 = logfile_close(&op->lh) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end if (ussnls_logend) */


int ussnls_logstuff(USSNLS *op,USSINFO *sip)
{
	int	rs ;
	int	f = false ;

	if ((rs = ussinfo_logfile(sip)) > 0) {
	    cchar	*lfname = sip->lfname ;
	    cchar	*logid = sip->logid ;
	    if ((rs = ussnls_logbegin(op,lfname,logid)) > 0) {
		USERINFO	*uip = &sip->u ;
		f = true ;

	                    logfile_userinfo(&op->lh,uip,0L,
	                        sip->searchname,sip->version) ;

	                    logfile_printf(&op->lh,"pid=%d",uip->pid) ;

	                    logfile_printf(&op->lh,"pr=%s",sip->pr) ;

	                    logfile_printf(&op->lh,"host=%s",sip->hostname) ;

	                    logfile_printf(&op->lh,"svc=%s",sip->svcname) ;

	    } /* end if (ussnls-logbegin) */
	} /* end if (ussinfo-logfile) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ussnls_logstuff) */


