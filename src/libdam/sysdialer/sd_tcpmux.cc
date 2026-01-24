/* sd_tcpmux SUPPORT (Sys-Dialer) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "tcpmux" dialer module */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 2003-11-04, David A­D­ Morano
	This was created as one of the first dialer modules for the
	SYSDIALER object.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	tcpmux

	Description:
	This is a SYSDIALER module.

	Synopsis:
	tcpmux [[<host>:]<port>] [-f <af>]

	Arguments:
	+ host		override hostname
	+ port		service port
	+ af		address family

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<baops.h>
#include	<localmisc.h>

#include	"sysdialer.h"
#include	"sd_tcpmux.h"
#include	"sd_tcp.h"


/* local defines */

#define	TCPMUX_MNAME	"tcpmux"
#define	TCPMUX_VERSION	"0"
#define	TCPMUX_INAME	""
#define	TCPMUX_MF1	(SYSDIALER_MFULL | SYSDIALER_MHALFOUT)
#define	TCPMUX_MF2	(SYSDIALER_MCOR | SYSDIALER_MCO)
#define	TCPMUX_MF3	(SYSDIALER_MARGS)
#define	TCPMUX_MF4	(SYSDIALER_MHALFIN)
#define	TCPMUX_MF	(TCPMUX_MF1 | TCPMUX_MF2 | TCPMUX_MF3|TCPMUX_MF4)

#define	TCPMUX_PORTNAME	"1"

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

SYSDIALER_INFO	tcpmux = {
	TCPMUX_MNAME,
	TCPMUX_VERSION,
	TCPMUX_INAME,
	szof(TCPMUX),
	TCPMUX_MF
} ;


/* exported subroutines */

int tcpmux_open(op,ap,hostname,svcname,av)
TCPMUX		*op ;
SYSDIALER_ARGS	*ap ;
cchar	hostname[] ;
cchar	svcname[] ;
cchar	*av[] ;
{
	int	rs = SR_OK ;
	int	fd ;
	int	to = -1 ;
	int	af = 0 ;
	int	opts = 0 ;

	cchar	*pr = nullptr ;
	cchar	*portname = TCPMUX_PORTNAME ;

	char	hostnamebuf[MAXHOSTNAMELEN + 1] ;
	char	svcnamebuf[SVCNAMELEN + 1] ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic == TCPMUX_MAGIC)
	    return SR_INUSE ;

#if	CF_DEBUGS
	debugprintf("tcpmux_open: entered hostname=%s svcname=%s\n",
		hostname,svcname) ;
#endif

	if (ap != nullptr) {
	    int	argr, argl, aol, avl ;
	    int	maxai, pan, npa, kwi, i ;
	    int	argnum ;
	    int	cl ;
	    int	f_optminus, f_optplus, f_optequal ;
	    int	f_extra = false ;
	    int	f_bad = false ;

	    cchar	**argv, *argp, *aop, *avp ;
	    char	argpresent[NARGPRESENT] ;
	    cchar	*afspec = nullptr ;
	    cchar	*hostport = nullptr ;
	    cchar	*cp ;


#if	CF_DEBUGS
	debugprintf("tcpmux_open: arguments\n") ;
#endif

	    hostnamebuf[0] = '\0' ;
	    svcnamebuf[0] = '\0' ;

	    to = ap->timeout ;
	    opts = ap->options ;
	    argv = ap->argv ;
	    if (ap->pr != nullptr)
	        pr = (char *) ap->pr ;

/* process program arguments */

	    for (i = 0 ; i < NARGPRESENT ; i += 1) argpresent[i] = 0 ;

	    npa = 0 ;			/* number of positional so far */
	    maxai = 0 ;
	    i = 0 ;
	    while ((argv[i] != nullptr) && (argv[i + 1] != nullptr)) {

	        argp = argv[++i] ;
	        argl = strlen(argp) ;

#if	CF_DEBUGS
	debugprintf("tcpmux_open: argl=%u argp=%p\n",argl,argp) ;
#endif

	        f_optminus = (*argp == '-') ;
	        f_optplus = (*argp == '+') ;
	        if ((argl > 0) && (f_optminus || f_optplus)) {

	            if (argl > 1) {
			const int	ach = MKCHAR(argp[1]) ;

	                if (isdigitlatin(ach)) {

	                    if (cfdeci(argp + 1,argl - 1,&argnum))
	                        goto badargval ;

	                } else {

#if	CF_DEBUGS
	                    debugprintf("main: got an option\n") ;
#endif

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

#if	CF_DEBUGS
	                        debugprintf("main: key letter\n") ;
#endif

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
	                                f_extra = true ;
	                                f_bad = true ;

	                            } /* end switch */

	                            aop += 1 ;
				    if (rs < 0)
					break ;

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

	                f_extra = true ;
	                f_bad = true ;
	            }

	        } /* end if (key letter/word or positional) */

		if (f_bad)
			break ;

	    } /* end while (all command line argument processing) */

	    if (f_bad)
	        goto badarg ;

		if (npa > 0) {

		pan = 0 ;
		for (i = 0 ; i <= maxai ; i += 1) {
		    if (BATST(argpresent,i)) {
			switch (pan) {
			case 0:
				hostport = argv[i] ;
				break ;
			case 1:
				svcname = argv[i] ;
				break ;
			} /* end switch */
			pan += 1 ;
                    } /* end if (argument present) */
		} /* end for */

	    } /* end if (positional arguments) */

	    if ((hostport != nullptr) && (hostport[0] != '\0')) {

	        if ((cp = strchr(hostport,':')) != nullptr) {

	            cl = MIN((cp - hostport),SVCNAMELEN) ;
	            if (cl > 0) {
	                hostname = hostnamebuf ;
	                strwcpy(hostnamebuf,hostport,cl) ;
	            }

	            cp += 1 ;
	            if (cp[0] != '\0')
	                portname = cp ;

	        } else
	            portname = hostport ;

	    } /* end if */

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
	debugprintf("tcpmux_open: done w/ arguments\n") ;
	debugprintf("tcpmux_open: hostname=%s portname=%s svcname=%s\n",
		hostname,portname,svcname) ;
	debugprintf("tcpmux_open: af=%u\n",af) ;
#endif

/* OK, do the dial */

	if (rs >= 0) {
	    rs = dialtcpmux(hostname,portname,af,svcname,av,to,opts) ;
	    op->fd = rs ;
	}

	if (rs >= 0) {
	    op->magic = TCPMUX_MAGIC ;
	    uc_closeonexec(op->fd,true) ;
	}

ret0:

#if	CF_DEBUGS
	debugprintf("tcpmux_open: ret rs=%d fd=%d\n",rs,op->fd) ;
#endif

	return (rs >= 0) ? op->fd : rs ;

/* bad stuff */
badargval:
badargnum:
badarg:
	rs = SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("tcpmux_open: ret BAD \n") ;
#endif

	goto ret0 ;
}
/* end subroutine (tcpmux_open) */


int tcpmux_reade(op,buf,buflen,to,opts)
TCPMUX		*op ;
char		buf[] ;
int		buflen ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_reade(op->fd,buf,buflen,to,opts) ;

	return rs ;
}
/* end subroutine (tcpmux_reade) */


int tcpmux_recve(op,buf,buflen,flags,to,opts)
TCPMUX		*op ;
char		buf[] ;
int		buflen ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recve(op->fd,buf,buflen,flags,to,opts) ;

	return rs ;
}
/* end subroutine (tcpmux_recve) */


int tcpmux_recvfrome(op,buf,buflen,flags,sap,salenp,to,opts)
TCPMUX		*op ;
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

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recvfrome(op->fd,buf,buflen,flags,sap,salenp,to,opts) ;

	return rs ;
}
/* end subroutine (tcpmux_recvfrome) */


int tcpmux_recvmsge(op,msgp,flags,to,opts)
TCPMUX		*op ;
struct msghdr	*msgp ;
int		flags ;
int		to, opts ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_recvmsge(op->fd,msgp,flags,to,opts) ;

	return rs ;
}
/* end subroutine (tcpmux_recvmsge) */


int tcpmux_write(op,buf,buflen)
TCPMUX		*op ;
cchar	buf[] ;
int		buflen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = uc_writen(op->fd,buf,buflen) ;

	return rs ;
}
/* end subroutine (tcpmux_write) */


int tcpmux_send(op,buf,buflen,flags)
TCPMUX		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_send(op->fd,buf,buflen,flags) ;

	return rs ;
}
/* end subroutine (tcpmux_send) */


int tcpmux_sendto(op,buf,buflen,flags,sap,salen)
TCPMUX		*op ;
cchar	buf[] ;
int		buflen ;
int		flags ;
void		*sap ;
int		salen ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendto(op->fd,buf,buflen,flags,sap,salen) ;

	return rs ;
}
/* end subroutine (tcpmux_sendto) */


int tcpmux_sendmsg(op,msgp,flags)
TCPMUX		*op ;
struct msghdr	*msgp ;
int		flags ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_sendmsg(op->fd,msgp,flags) ;

	return rs ;
}
/* end subroutine (tcpmux_sendmsg) */


int tcpmux_shutdown(op,cmd)
TCPMUX		*op ;
int		cmd ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_shutdown(op->fd,cmd) ;

	return rs ;
}
/* end subroutine (tcp_shutdown) */


/* close the connection */
int tcpmux_close(op)
TCPMUX		*op ;
{
	int	rs ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != TCPMUX_MAGIC)
	    return SR_NOTOPEN ;

	rs = u_close(op->fd) ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (tcpmux_close) */


/* local subroutines */


