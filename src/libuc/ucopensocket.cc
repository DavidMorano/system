/* ucopensocket SUPPORT */
/* lang=C++20 */

/* open a UNIX® domain socket */
/* version %I% last-modified %G% */

#define	CF_OPENTMPFILE	1		/* bind a local address for DGRAM */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is used to open a UNIX® domain socket pretty
	much as if it was a regular UNIX® file.	This is something that
	(sadly) cannot be done with sockets normally.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<hostent.h>
#include	<sockaddress.h>
#include	<inetaddr.h>
#include	<opentmp.h>
#include	<mkpathx.h>
#include	<strwcpyx.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOMEM	60
#define	TO_NOSR		(5 * 60)

#define	BUFLEN		(MAXPATHLEN + 4)

#if	defined(IRIX) && (! defined(TYPEDEF_INADDRT))
#define	TYPEDEF_INADDRT	1

typedef unsigned int	in_addr_t ;

#endif

#ifndef	POLLINTMULT
#define	POLLINTMULT	1000
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif


/* external subroutines */


/* local structures */


/* forward references */

static int	makeconn(int,int,SOCKADDR *,int, int) noex ;


/* local variables */


/* exported subroutines */

int uc_opensocket(cchar *fname,int oflags,int timeout) noex {
	SOCKADDR	*sap ;
	cint		pf = PF_UNIX ;
	cint		st = SOCK_STREAM ;
	int		rs ;
	int		cl ;
	int		s = -1 ;
	char		buf[BUFLEN + 1] ;

	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	sap = (SOCKADDR *) buf ;
	sap->sa_family = htons(AF_UNIX) ;
	cl = (strwcpy(sap->sa_data,fname,MAXPATHLEN) - sap->sa_data) ;

#ifdef	OPTIONAL
	sap->sa_data[MAXPATHLEN] = '\0' ;
#endif

	rs = makeconn(pf,st,sap,(cl + 2),timeout) ;
	s = rs ;
	if (rs == SR_PROTOTYPE) {
	    rs = makeconn(pf,st,sap,(cl + 2),timeout) ;
	    s = rs ;
	}

	if ((rs >= 0) && ((oflags & O_RDWR) != O_RDWR)) {
	    if ((oflags & O_WRONLY) == O_WRONLY) {
	        u_shutdown(s,SHUT_RD) ;
	    } else {
	        u_shutdown(s,SHUT_WR) ;
	    }
	} /* end if (successful open) */

	return (rs >= 0) ? s : rs ;
}
/* end subroutine (uc_opensocket) */


/* local subroutines */

static int makeconn(int pf,int ptype,SOCKADDR *sap,int sal,int timeout) noex {
	int		rs ;
	int		s = 0 ;
	char	sfname[MAXPATHLEN+1] = { 0 } ;

#if	CF_OPENTMPFILE
	if (ptype == SOCK_DGRAM) {
	    cchar	*cp ;
	    char	template[MAXPATHLEN + 1] ;
	    if ((cp = getenv(VARTMPDNAME)) == NULL) cp = TMPDNAME ;
	    mkpath2(template,cp,"openXXXXXXXXXX") ;
	    rs = opentmpusd(template,O_RDWR,0666,sfname) ;
	    s = rs ;
	} else {
	    rs = u_socket(pf,ptype,0) ;
	    s = rs ;
	}
#else /* CF_OPENTMPFILE */
	rs = u_socket(pf,ptype,0) ;
	s = rs ;
#endif /* CF_OPENTMPFILE */

	if (rs >= 0) {
	    rs = uc_connecte(s,sap,sal,timeout) ;
	}
	if (rs < 0) u_close(s) ;

	if (ptype == SOCK_DGRAM) {
	    if (sfname[0] != '\0') u_unlink(sfname) ;
	}

	return (rs >= 0) ? s : rs ;
}
/* end subroutine (makeconn) */


