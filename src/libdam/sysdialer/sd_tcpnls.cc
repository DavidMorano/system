/* sd_tcpnls SUPPORT (Sys-Dialer) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "tcpnls" dialer */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_SOLARISBUG	1		/* bad bug in Solaris TLI */

/* revision history:

	= 2003-11-04, David A­D­ Morano
	This was created as one of the first dialer modules for the
	SYSDIALER object.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This is a SYSDIALER module.

	Synopsis:
	tcpnls [[<host>:]<port>] [-f <af>]

	Arguments:
	host		override hostname
	port		service port
	af		address family

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<baops.h>
#include	<localmisc.h>

#include	"sysdialer.h"
#include	"sd_tcpnls.h"
#include	"sd_tcp.h"


/* local defines */

#define	TCPNLS_MNAME	"tcpnls"
#define	TCPNLS_VERSION	"0"
#define	TCPNLS_INAME	""
#define	TCPNLS_MF1	(SYSDIALER_MFULL | SYSDIALER_MHALFOUT)
#define	TCPNLS_MF2	(SYSDIALER_MCOR | SYSDIALER_MCO)
#define	TCPNLS_MF3	(SYSDIALER_MHALFIN)
#define	TCPNLS_MF	(TCPNLS_MF1 | TCPNLS_MF2|TCPNLS_MF3)

#define	TCPNLS_PORTNAME	"listen"

#ifndef	SVCNAMELEN
#define	SVCNAMELEN	32
#endif

#define	ARGBUFLEN	(MAXPATHLEN + 35)

#define	NPARG		2	/* number of positional arguments */
#define	MAXARGINDEX	100
#define	NARGPRESENT	(MAXARGINDEX/8 + 1)


/* external subroutines */


/* external variables */


/* local structures */

struct afamily {
	cchar	*name ;
	int		af ;
} ; /* end struct */


/* forward references */


/* local variables */

enum argopts {
	argopt_root,
	argopt_af,
	argopt_overlast
} ; /* end enum (argopts) */

constexpr cpcchar	argopts[] = {
	"ROOT",
	"af",
	nullptr
} ; /* end array (argopts) */

constexpr afamily	afs[] = {
	{ "inet", AF_INET },
	{ "inet4", AF_INET },
#ifdef	AF_INET6
	{ "inet6", AF_INET6 },
#endif
	{ nullptr, 0 }
} ; /* end array (afs) */


/* external variables (module information) */

SYSDIALER_INFO	tcpnls = {
	TCPNLS_MNAME,
	TCPNLS_VERSION,
	TCPNLS_INAME,
	szof(TCPNLS),
	TCPNLS_MF
} ;


/* exported subroutines */

int tcpnls_open(op,ap,hostname,svcname,av)
TCPNLS		*op ;
SYSDIALER_ARGS	*ap ;
cchar	hostname[] ;
cchar	svcname[] ;
cchar	*av[] ;
{
	int		rs = SR_OK ;
	int		to = -1 ;
	int		af = 0 ;
	int		opts = 0 ;
	cchar	*portname = TCPNLS_PORTNAME ;
	cchar	*pr = nullptr ;
	cchar	*tp ;
	char		hostnamebuf[MAXHOSTNAMELEN + 1] ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic == TCPNLS_MAGIC) return SR_INUSE ;

#if	CF_DEBUGS
	debugprintf("tcpnls_open: ent hostname=%s svcname=%s\n",
	    hostname,svcname) ;
#endif

	if (ap != nullptr) {
	    int	argr, argl, aol, avl ;
	    int	maxai, pan, npa, kwi, ai, i ;
	    int	cl, ml ;
	    int	f_optminus, f_optplus, f_optequal ;
	    int	f_bad = false ;

	    cchar	**argv, *argp, *aop, *avp ;
	    cchar	*argval = nullptr ;
	    char	argpresent[NARGPRESENT] ;
	    cchar	*afspec = nullptr ;
	    cchar	*hostspec = nullptr ;

#if	CF_DEBUGS
	    debugprintf("tcpnls_open: arguments\n") ;
#endif

	    hostnamebuf[0] = '\0' ;

	    to = ap->timeout ;
	    opts = ap->options ;
	    argv = ap->argv ;
	    if (ap->pr != nullptr)
	        pr = (char *) ap->pr ;

	    for (argr = 0 ; argv[argr] != nullptr ; argr += 1) ;

#if	CF_DEBUGS
	    {
	        debugprintf("tcpnls_open: argr=%u\n",argr) ;
	        for (ai = 0 ; argv[ai] != nullptr ; ai += 1)
	            debugprintf("tcpnls_open: argv[%u]=%s\n",ai,argv[ai]) ;
	    }
#endif

/* process program arguments */

	    for (i = 0 ; i < NARGPRESENT ; i += 1)
	        argpresent[i] = 0 ;

	    npa = 0 ;			/* number of positional so far */
	    maxai = 0 ;
	    i = 0 ;
	    while ((argv[i] != nullptr) && (argv[i + 1] != nullptr)) {

	        argp = argv[++i] ;
	        argl = strlen(argp) ;

	        f_optminus = (*argp == '-') ;
	        f_optplus = (*argp == '+') ;
	        if ((argl > 0) && (f_optminus || f_optplus)) {

	            if (argl > 1) {
			const int	ach = MKCHAR(argp[1]) ;

	                if (isdigitlatin(ach)) {

	                    argval = (argp+1) ;

	                } else {

	                    aop = argp + 1 ;
	                    aol = argl - 1 ;
	                    f_optequal = false ;
	                    if ((avp = strchr(aop,'=')) != nullptr) {

#if	CF_DEBUGS
	                        debugprintf("main: key w/ value\n") ;
#endif

	                        aol = avp - aop ;
	                        avp += 1 ;
	                        avl = aop + argl - 1 - avp ;
	                        f_optequal = true ;

	                    } else
	                        avl = 0 ;

	                    if ((kwi = matostr(argopts,2,aop,aol)) >= 0) {

	                        switch (kwi) {

/* program root */
	                        case argopt_root:
	                            if (f_optequal) {

	                                f_optequal = false ;
	                                if (avl)
	                                    pr = avp ;

	                            } else {

	                                if (argv[i + 1] == nullptr)
	                                    goto badargnum ;

	                                argp = argv[++i] ;
	                                argl = strlen(argp) ;

	                                if (argl)
	                                    pr = argp ;

	                            }

	                            break ;

	                        case argopt_af:
	                            if (f_optequal) {

	                                f_optequal = false ;
	                                if (avl)
	                                    afspec = avp ;

	                            } else {

	                                if (argv[i + 1] == nullptr)
	                                    goto badargnum ;

	                                argp = argv[++i] ;
	                                argl = strlen(argp) ;

	                                if (argl)
	                                    afspec = argp ;

	                            }

	                            break ;

	                        } /* end switch (key words) */

	                    } else {

	                        while (aol--) {

	                            switch ((int) *aop) {

	                            case 'f':
	                                if (argv[i + 1] == nullptr)
	                                    goto badargnum ;

	                                argp = argv[++i] ;
	                                argl = strlen(argp) ;

	                                if (argl)
	                                    afspec = argp ;

	                                break ;

/* service name */
	                            case 's':
	                                if (argv[i + 1] == nullptr)
	                                    goto badargnum ;

	                                argp = argv[++i] ;
	                                argl = strlen(argp) ;

	                                if (argl)
	                                    svcname = argp ;

	                                break ;

/* timeout */
	                            case 't':
	                                if (argv[i + 1] == nullptr)
	                                    goto badargnum ;

	                                argp = argv[++i] ;
	                                argl = strlen(argp) ;

	                                if (argl) {

	                                    rs = cfdeci(argp,argl,&to) ;

	                                    if (rs < 0)
	                                        goto badargval ;

	                                }

	                                break ;

	                            default:
	                                f_bad = true ;
					break ;

	                            } /* end switch */
	                            aop += 1 ;

				    if (rs < 0) break ;
	                        } /* end while */

	                    } /* end if (individual option key letters) */

	                } /* end if (digits as argument or not) */

	            } else {

	                if (i < MAXARGINDEX) {

	                    BASET(argpresent,i) ;
	                    maxai = i ;
	                    npa += 1 ;	/* increment position count */

	                }

	            } /* end if */

	        } else {

	            if (i < MAXARGINDEX) {
	                BASET(argpresent,i) ;
	                maxai = i ;
	                npa += 1 ;
	            } else {
	                f_bad = true ;
	            }

	        } /* end if (key letter/word or positional) */

	        if (f_bad) break ;
	    } /* end while (all command line argument processing) */

	    if (f_bad)
	        goto badarg ;

	    if (npa > 0) {

	        pan = 0 ;
	        for (i = 0 ; i <= maxai ; i += 1) {

	            if (BATST(argpresent,i)) {

	                switch (pan) {

	                case 0:
	                    hostspec = argv[i] ;

#if	CF_DEBUGS
	                    debugprintf("tcpnls_open: pan=%d hostspec=%s\n",
				pan,hostspec) ;
#endif

	                    break ;

	                case 1:
	                    svcname = argv[i] ;
	                    break ;

	                default:
	                     ;
	                } /* end switch */

	                pan += 1 ;

	            } /* end if (argument present) */

	        } /* end for */

	    } /* end if (positional arguments) */

	    if ((hostspec != nullptr) && (hostspec[0] != '\0')) {

#if	CF_DEBUGS
	        debugprintf("tcpnls_open: hostspec=%s\n",hostspec) ;
#endif

	        if ((tp = strchr(hostspec,':')) != nullptr) {

	            ml = MIN((tp - hostspec),MAXHOSTNAMELEN) ;
	            if (ml > 0) {

	                hostname = hostnamebuf ;
	                strwcpy(hostnamebuf,hostspec,ml) ;

#if	CF_DEBUGS
	                debugprintf("tcpnls_open: hostname=%s\n",hostname) ;
#endif

	            }

	            tp += 1 ;
	            if (tp[0] != '\0')
	                portname = tp ;

	        } else
	            portname = hostspec ;

	    } /* end if */

#if	CF_DEBUGS
	    debugprintf("tcpnls_open: hostname=%s portname=%s\n",
	        hostname,portname) ;
#endif

	    if ((afspec != nullptr) && (afspec[0] != '\0')) {

	        for (i = 0 ; afs[i].name != nullptr ; i += 1) {

	            if (strcmp(afs[i].name,afspec) == 0)
	                break ;

	        }

	        if (afs[i].name != nullptr) {
	            af = afs[i].af ;
	            } else
	            rs = SR_INVALID ;

	    } /* end if (address family specification) */

	} /* end if (had arguments) */

#if	CF_DEBUGS
	debugprintf("tcpnls_open: done w/ arguments\n") ;
	debugprintf("tcpnls_open: hostname=%s portname=%s svcname=%s\n",
	    hostname,portname,svcname) ;
	debugprintf("tcpnls_open: af=%u\n",af) ;
#endif

/* OK, do the dial */

	if (rs >= 0)
	    rs = dialtcpnls(hostname,portname,af,svcname,to,opts) ;

	op->fd = rs ;
	if (rs >= 0) {

	    uc_closeonexec(op->fd,true) ;

	    op->magic = TCPNLS_MAGIC ;
	    op->opentime = time(nullptr) ;

	}

ret0:

#if	CF_DEBUGS
	debugprintf("tcpnls_open: ret rs=%d fd=%d\n",rs,op->fd) ;
#endif

	return (rs >= 0) ? op->fd : rs ;

/* bad stuff */
badargval:
badargnum:
badarg:

#if	CF_DEBUGS
	debugprintf("tcpnls_open: ret BAD \n") ;
#endif

	goto ret0 ;
}
/* end subroutine (tcpnls_open) */


int tcpnls_reade(op,buf,buflen,to,opts)
TCPNLS		*op ;
char		buf[] ;
int		buflen ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_reade(op->fd,buf,buflen,to,opts) ;

	return rs ;
}


int tcpnls_recve(op,buf,buflen,flags,to,opts)
TCPNLS		*op ;
char		buf[] ;
int		buflen ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recve(op->fd,buf,buflen,flags,to,opts) ;

	return rs ;
}


int tcpnls_recvfrome(op,buf,buflen,flags,sap,salenp,to,opts)
TCPNLS		*op ;
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

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recvfrome(op->fd,buf,buflen,flags,sap,salenp,to,opts) ;

	return rs ;
}


int tcpnls_recvmsge(op,msgp,flags,to,opts)
TCPNLS		*op ;
struct msghdr	*msgp ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recvmsge(op->fd,msgp,flags,to,opts) ;

	return rs ;
}


int tcpnls_write(op,buf,buflen)
TCPNLS		*op ;
cchar	buf[] ;
int		buflen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_writen(op->fd,buf,buflen) ;

	return rs ;
}


int tcpnls_send(op,buf,buflen,flags)
TCPNLS		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_send(op->fd,buf,buflen,flags) ;

	return rs ;
}


int tcpnls_sendto(op,buf,buflen,flags,sap,salen)
TCPNLS		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
void		*sap ;
int		salen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendto(op->fd,buf,buflen,flags,sap,salen) ;

	return rs ;
}


int tcpnls_sendmsg(op,msgp,flags)
TCPNLS		*op ;
struct msghdr	*msgp ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendmsg(op->fd,msgp,flags) ;

	return rs ;
}


/* shutdown */
int tcpnls_shutdown(op,cmd)
TCPNLS		*op ;
int		cmd ;
{
	time_t	daytime ;

	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

/* a bad bug in the Solaris TLI module hoses on this if comes too fast */

#if	CF_SOLARISBUG
	if (cmd == SHUT_WR) {

	    daytime = time(nullptr) ;

	    if ((op->opentime - daytime) < 1)
	        sleep(1) ;

	}
#endif /* CF_SOLARISBUG */

/* OK, we should be able to do this now ! (stupid Solaris !) */

	rs = u_shutdown(op->fd,cmd) ;

	return rs ;
}
/* end subroutine (tcpnls_shutdown) */


/* close the connection */
int tcpnls_close(op)
TCPNLS		*op ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPNLS_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_close(op->fd) ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (tcpnls_close) */


/* private subroutines */


