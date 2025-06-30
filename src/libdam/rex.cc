/* rex SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to open a FD to a remotely executing command */
/* version %I% last-modified %G% */

#define	CF_REXECL	1		/* use |rexecl(3dam)| */
#define	CF_RCMDU	1		/* use |rcmdu(3dam)| */

/* revision history:

	= 1996-11-21, David A­D­ Morano
	This program was started by copying from the RSLOW program.

	= 1996-12-12, David A­D­ Morano
	I modified the program to take the username and password
	from a specified file (for better security).

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rex

	Description:
	The subroutine either returns a FD for the standard-input
	of the remote command or it returns an error that is indicated
	by a negative valued return.  If 'fd2p' is non-nullptr, a
	secondary channel to the standard error of the remote command
	is created also.  Depending on the arguments to the subroutine
	call, both the INET 'exec' or 'shell' services may be invoked
	to try and make a connection to the remote host.

	Synopsis:
	int rex(cc *rhost,rex_au *ap,int ro,cc *pg,mv argv,int *fd2p,
		netfile_ent **mpp) noex

	Arguments:
	rhost		remote host
	ap		pointer to AUTH object
	ro		REX options
	pg		program to execute
	mv		arguments to program (to execute)
	fd2p		pointer to integer to get error-FD
	mpp		result pointer to NETFILE_ENT object

	Returns:
	>=0		resulting socket to remote program STDIN & STDOUT
	<0		error (system-return)

	Notes:
	struct rex_auth {
		char	*restrict ;
		char	*rusername ;
		char	*rpassword ;
		NETFILE_ENT	**machinev ;
	} *auth ;

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<sys/param.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<userinfo.h>
#include	<getxx.h>
#include	<getehostname.h>
#include	<netfile.h>
#include	<bfile.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"rex.h"

import libutil ;

/* local defines */

#define	BUFLEN		(8 * 1024)
#define	USERBUFLEN	(2 * 1024)
#define	CMDBUFLEN	8192

#ifndef	LEQUIV
#define	LEQUIV(a,b)	(((a) && (b)) || ((! (a)) && (! (b))))
#endif

#ifndef	INETADDRBAD
#define	INETADDRBAD	((unsigned int) (~ 0))
#endif


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;
typedef NETFILE_ENT	ent ;
typedef NETFILE_ENT *	entp ;
typedef rex_au		au ;
typedef rex_fl		fl ;


/* external subroutines */

extern "C" {
    extern int mkquoted(char *,int,cchar *,int) noex ;
    extern int bufprintf(char *,int,cchar *,...) noex ;
    extern int rexecl(char **,int,cc *,cc *,cc *,int *) noex ;
    extern int rcmdu(cc *,cc *,cc *,int *) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct rexer {
	rex_au		*auth ;
	cchar		*rhost ;
	cchar		*program ;
	mainv		argv ;
	mainv		envv ;
	int		*fd2p ;
	entp		*mpp ;
	userinfo	u ;
	int		ro ;
	rexer(cc *r,au *a,int f,cc *p,mv av,mv ev,int *d,entp *pp) noex {
	    rhost = r ;
	    auth = a ;
	    ro = f ;
	    program = p ;
	    argv = av ;
	    envv = ev ;
	    fd2p = d ;
	    mpp = pp ;
	} ;
	operator int () noex ;
	int mainsub() noex ;
    } ; /* end struct (rexer) */
}


/* forward subroutines */

static int	hostequiv(cc *,cc *,cc *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int rex(cc *rh,au *auth,int ro,cc *pg,mv av,mv ev,int *fd2p,entp *mpp) noex {
	rexer		rexo(rh,auth,ro,pg,av,ev,fd2p,mpp) ;
	return rexo ;
}

rexer::operator int () noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		s = -1 ;
	if ((rs = u.start) >= 0) {
	    {
		rs = mainsub() ;
		s = rs ;
	    }
	    rs1 = u.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (userinfo) */
	return (rs >= 0) ? s : rs ;
} ;

int rexer::mainsub() noex {
	ucentsv		se{} ;
	ucentsv		*sp{} ;
	NETFILE_ENT	*mp ;
	int		rs = SR_OK ;
	int		srs ;
	int		i ;
	int		len, l ;
	int		rfd = -1 ;
	int		port = -1 ;
	cchar	*args[2] ;
	cchar	*username = nullptr ;
	char		buf[BUFLEN + 1], *bp ;
	char		cmdbuf[CMDBUFLEN + 1] ;
	char		hostname[MAXHOSTNAMELEN + 1] ;
	char		ahostname[MAXHOSTNAMELEN + 1] ;
	cchar	*password = nullptr ;
	cchar	*authorization = "any" ;
	char		*ahost = ahostname ;

	if ((rhost == nullptr) || (rhost[0] == '\0'))
	    goto badhost ;

	if ((program == nullptr) || (program[0] == '\0'))
	    goto badprog ;

	if (argv == nullptr) {
	    cchar *cp ;
	    if (int cl ; (cl = sfbasename(program,-1,&cp)) > 0) {
	        args[0] = cp ;
	    } else {
		args[0] = "unknown" ;
	    }
	    args[1] = nullptr ;
	    argv = args ;
	} /* end if */

/* laod up some authorization information that we have */

	if (auth != nullptr) {

	    if (auth->username != nullptr) 
		username = auth->username ;

	    if (auth->password != nullptr) 
		password = auth->password ;

	    if (auth->res != nullptr) 
		authorization = auth->res ;

	} /* end if */

	if ((username == nullptr) || (username[0] == '\0'))
	    username = u.username ;

/* process the 'rhost' to see if there is a port (or service) also */

	if (cc *tp ; (tp = strchr(rhost,':')) != nullptr) {

	    cint tl = intconv(tp - rhost) ;
	    strwcpy(hostname,rhost,tl) ;

	    tp += 1 ;
	    if (cfdec(tp,-1,&port) < 0) {

	        port = -1 ;

#ifdef	COMMENT
	        sp = getservbyname_r(cp, "tcp", &se,cmdbuf,CMDBUFLEN) ;
#else
		rs = getsv_name(&se,cmdbuf,CMDBUFLEN,tp,"tcp") ;
#endif
	        if (sp != nullptr) {
	            port = ntohs(sp->s_port) ;
	        }

	    } /* end if (bad decimal conversion) */

	} else {
	    strcpy(hostname,rhost) ;
	}

/* get the INET service port for the REXEC service */

	if (port < 0) {

#ifdef	COMMENT
	    sp = getservbyname_r("exec", "tcp", &se,cmdbuf,CMDBUFLEN) ;
#else
	    rs = getsv_name(&se,cmdbuf,CMDBUFLEN,"exec","tcp") ;
#endif
	    if (sp == nullptr) {
#ifdef	COMMENT
	        sp = getservbyname_r("rexec", "tcp", &se,cmdbuf,CMDBUFLEN) ;
#else
	        rs = getsv_name(&se,cmdbuf,CMDBUFLEN,"rexec","tcp") ;
#endif
	    }

	    if (sp != nullptr) {
	        port = ntohs(sp->s_port) ;
	    } else {
	        port = REX_DEFEXECSERVICE ;
	    }

	} /* end if (default port) */

/* create the remote command */

	len = bufprintf(cmdbuf,CMDBUFLEN,"%s",program) ;

	if ((argv != nullptr) && (argv[0] != nullptr)) {

	    for (i = 1 ; argv[i] != nullptr ; i += 1) {

		bp = buf ;
	        l = mkquoted(buf,BUFLEN,argv[i],-1) ;

	        if ((l < 0) || (l > (CMDBUFLEN - len)))
	            goto badtoomuch ;

	        cmdbuf[len++] = ' ' ;
	        strwcpy(cmdbuf + len,bp,l) ;

	        len += l ;

	    } /* end for */

	} /* end if ('argv' not nullptr) */

/* now we want to make sure that our remote hostname is INET translatable */

	{
	    in_addr_t	addr ;

	    addr = inet_addr(hostname) ;
	    if (addr == INETADDRBAD) {
	        if (getehostname(buf,hostname) < 0) 
		    goto badunreach ;

	        if (strcmp(hostname,buf) != 0) {
	            strcpy(hostname,buf) ;
		}

	    } /* end if */
	} /* end block */

/* try the supplied password that we have been given */

	if (password != nullptr) {

	    ahost = ahostname ;
	    strcpy(ahostname,hostname) ;

#if	CF_REXECL
	    rs = rexecl(&ahost,port,username,
	        password, cmdbuf,fd2p) ;
#else
	    if (password == nullptr) password = "" ;

	    rs = rexec(&ahost,port,username,
	        password, cmdbuf,fd2p) ;
#endif /* R_REXECL */

	    rfd = rs ;
	    if (rs >= 0) goto goodsupply ;

	} /* end if (tried supplied password) */

	if (strncmp(authorization,"supplied",1) == 0) 
		goto badrexec ;

/* process any NETRC files that we can find */
/* try to find a matching NETRC entry for the host/user pair that we have */

	if ((auth != nullptr) && (auth->machinev != nullptr) &&
	    (strncmp(authorization,"r",1) != 0)) {

	    rs = -1 ;
	    for (i = 0 ; (mp = auth->machinev[i]) != nullptr ; i += 1) {

	        if ((mp->machine == nullptr) ||
	            (mp->login == nullptr) ||
	            (mp->password == nullptr)) continue ;

	        if ((hostequiv(hostname,mp->machine,u.domainname)) &&
	            (strcmp(mp->login,username) == 0)) {

	            ahost = ahostname ;
	            strcpy(ahostname,hostname) ;

#if	CF_REXECL
	            rs = rexecl(&ahost,port,username,
	                mp->password, cmdbuf,fd2p) ;
#else
	            password = mp->password ;
	            if (password == nullptr) password = "" ;

	            rs = rexec(&ahost,port,username,
	                password, cmdbuf,fd2p) ;
#endif

	            rfd = rs ;
	            if (rs >= 0) break ;

	        } /* end if */

	    } /* end for */

	    if (rs >= 0) goto goodrexec ;

	} /* end if (we have authorization machine entries) */

	if (strncmp(authorization,"password",1) == 0) 
		goto badrexec ;

#if	CF_RCMDU

/* try RCMDU with the supplied username */

	if (! (ro & rexopt_keepalive)) {

	    rs = rcmdu(hostname,username, cmdbuf,fd2p) ;
	    rfd = rs ;
	    if (rs >= 0) {

	        goto goodrcmdu ;
	    }

	    if ((rs == SR_HOSTDOWN) || (rs == SR_HOSTUNREACH))
	        goto hostdown ;

	} /* end if */

#endif

	if (strncmp(authorization,"user",1) == 0) 
		goto badrexec ;

/* try no password on the specified username */

	ahost = ahostname ;
	strcpy(ahostname,hostname) ;

#if	CF_REXECL
	rs = rexecl(&ahost,port,username,
	    "", cmdbuf,fd2p) ;
#else
	rs = rexec(&ahost,port,username,
	    "", cmdbuf,fd2p) ;
#endif

	rfd = rs ;
	if (rs >= 0) 
		goto goodrexec ;

/* we couldn't get through with a nullptr password */


/* try to find a matching NETRC entry for the host only */

	if ((auth != nullptr) && (auth->machinev != nullptr) &&
	    (strncmp(authorization,"r",1) != 0)) {

	    rs = -1 ;
	    for (i = 0 ; (mp = auth->machinev[i]) != nullptr ; i += 1) {

	        if ((mp->machine == nullptr) ||
	            (mp->login == nullptr)) continue ;

	        if (hostequiv(hostname,mp->machine,u.domainname) &&
	            (strcasecmp(mp->login,username) != 0)) {

	            ahost = ahostname ;
	            strcpy(ahostname,hostname) ;

#if	CF_REXECL
	            rs = rexecl(&ahost,port,mp->login,
	                mp->password, cmdbuf,fd2p) ;
#else
	            password = mp->password ;
	            if (password == nullptr) password = "" ;

	            rs = rexec(&ahost,port,mp->login,
	                password, cmdbuf,fd2p) ;
#endif /* CF_REXECL */

	            rfd = rs ;
	            if (rs >= 0) break ;

	        }

	    } /* end for */

	    if (rs >= 0) goto goodrexec ;

	} /* end if (we had authorization NETRC information) */

/* done with this try */

#if	CF_RCMDU

/* finally we try to connect using RCMDU with our own username */

	if ((! (ro & rexopt_keepalive)) && 
	    (strcmp(username,u.username) != 0)) {

	    rs = rcmdu(hostname,u.username,
	        cmdbuf,fd2p) ;

	    rfd = rs ;
	    if (rs >= 0) goto goodrcmdu ;

	    if ((rs == SR_HOSTDOWN) || (rs == SR_HOSTUNREACH))
	        goto hostdown ;

	} /* end if */

#endif /* CF_RCMDU */

/* we failed all attempts */

	goto badrexec ;

/* we got in! */
goodsupply:
goodrcmdu:
	if (mpp != nullptr)
	    *mpp = nullptr ;

	goto done ;

goodrexec:
	if (mpp != nullptr)
	    *mpp = mp ;

done:

	return rfd ;

/* bad returns come here */
badret:
	return srs ;

badtoomuch:
	srs = SR_2BIG ;
	goto badret ;

badhost:
	srs = SR_INVAL ;
	goto badret ;

badprog:
	srs = SR_INVAL ;
	goto badret ;

badrexec:
	srs = SR_ACCES ;
	goto badret ;

hostdown:
	srs = rs ;
	goto badret ;

badunreach:
	srs = SR_HOSTUNREACH ;
	goto badret ;

}
/* end subroutine (rexer::mainsub) */


/* local subroutines */

static int hostequiv(cc *h1,cc *h2,cc *localdomain) noex {
	int	f_h1 = false, f_h2 = false ;
	int	len1, len2 ;
	char	*cp1, *cp2 ;

	if ((cp1 = strchr(h1,'.')) != nullptr) 
		f_h1 = true ;

	if ((cp2 = strchr(h2,'.')) != nullptr) 
		f_h2 = true ;

	if (LEQUIV(f_h1,f_h2))
	    return (! strcasecmp(h1,h2)) ;

	if (f_h1) {

	    len1 = intconv(cp1 - h1) ;
	    len2 = lenstr(h2) ;

	    if (len1 != len2) return false ;

	    cp1 += 1 ;
	    if (strcasecmp(cp1,localdomain) != 0) return false ;

	    return (strncasecmp(h1,h2,len1) == 0) ;

	} /* end if */

	len1 = lenstr(h1) ;

	len2 = intconv(cp2 - h2) ;
	if (len1 != len2) return false ;

	cp2 += 1 ;
	if (strcasecmp(cp2,localdomain) != 0) return false ;

	return (strncasecmp(h1,h2,len2) == 0) ;
}
/* end subroutine (hostequiv) */


