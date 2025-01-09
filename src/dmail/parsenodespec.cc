/* parsenodespec SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* parse a COMSAT node specification into node and port */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	parsenodespec

	Description:
	This little subroutine just parses out a node specification.
	A node specification looks like:

		hostname[:port]

	The port can be either numeric or all alpha.  If the parse
	is successful, the port number is returned.  If no port
	number (or name) was within the specification, the default
	port that was passed by the caller is returned instead.

	Synopsis:
	int parsenodespec(proginfo *pip,char *rbuf,cc *nsp,int nsl) noex

	Arguments:
	pip		pointer to program information
	rbuf		user supplied buffer
	nsp		pointer to node-specification
	nsl		length of node-specification

	Returns:
	>=0		port number that was parsed out
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<netinet/in.h>
#include	<unistd.h>
#include	<csignal>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getportnum.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	BUFLEN		(2 * MAXHOSTNAMELEN)
#define	HOSTBUFLEN	(10 * MAXHOSTNAMELEN)

#define	PROTONAME	"udp"
#define	LOCALHOST	"localhost"


/* external subroutines */

extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	hasalldig(cchar *,int) ;
extern int	isdigitlatin(int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;
extern char	*strdcpy1(char *,int,cchar *) ;
extern char	*strdcpy1w(char *,int,cchar *,int) ;
extern char	*timestr_log(time_t,char *) ;
extern char	*timestr_logz(time_t,char *) ;


/* external variables */


/* local structures */


/* local typedefs */

#if	defined(IRIX) && (! defined(TYPEDEF_INADDRT))
#define	TYPEDEF_INADDRT	1
typedef unsigned int	in_addr_t ;
#endif


/* forward references */


/* local variables */


/* external subroutines */

int parsenodespec(PROGINFO *pip,char rbuf[],cchar *nsp,int nsl) noex {
	int		rs = SR_OK ;
	int		nl, pl ;
	int		port = 0 ;
	cchar	*tp ;
	cchar	*np, *pp ;

	if (pip == NULL) return SR_FAULT ;

	rbuf[0] = '\0' ;
	pp = NULL ;
	pl = 0 ;
	if ((tp = strnchr(nsp,nsl,':')) != NULL) {

	    nl = sfshrink(nsp,(tp - nsp),&np) ;

	    pl = sfshrink((tp + 1),-1,&pp) ;

	    if (pl == 0)
	        pp = NULL ;

	} else
	    nl = sfshrink(nsp,nsl,&np) ;

	if (nl > 0) {

	    if ((pp != NULL) && (pl > 0)) {
	        if (hasalldig(pp,pl)) {
	            rs = cfdeci(pp,pl,&port) ;
	        } else {
		    cchar	*pn = PROTONAME ;
		    rs = getportnum(pn,pp) ;
		    port = rs ;
	        } /* end if (numeric or alpha) */
	    } /* end if (had a port spec) */

	    if (rs >= 0) {
	        strdcpy1w(rbuf,NODENAMELEN,np,nl) ;
	    }

	} /* end if (positive) */

	return (rs >= 0) ? port : rs ;
}
/* end subroutine (parsenodespec) */


