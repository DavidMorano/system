/* getserv */

/* get a network service number (port) given protocol and service name */


#define	CF_DEBUGS	0		/* compile-time debug print-outs */


/* revision history:

	= 1998-07-10, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

        Get a service number (a port number really) given a protocol name and a
        service name. Remember that each protocol can have its own port
        associated with any given service name. So therefore each service name
        can have a separate port depending on what protocol name it is
        associated with.

	Synopsis:

	int getserv_name(protoname,svc)
	cchar		*protoname ;
	cchar		*svc ;

	Arguments:

	protoname	protocol name
	svc		service name

	Returns:

	>=0		port number
	<0		error


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>

#include	<usystem.h>
#include	<getbufsize.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(const char *,...) ;
extern int	strlinelen(const char *,int,int) ;
#endif


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


int getserv_name(cchar *pn,cchar *svc)
{
	int		rs ;
	int		rs1 ;
	int		port = 0 ;
	if ((rs = getbufsize(getbufsize_se)) >= 0) {
	    struct servent	se ;
	    const int		selen = rs ;
	    char		*sebuf ;
	    if ((rs = uc_malloc((selen+1),&sebuf)) >= 0) {
	        if ((rs = uc_getservbyname(svc,pn,&se,sebuf,selen)) >= 0) {
	            port = (int) ntohs(se.s_port) ;
	        }
	        rs1 = uc_free(sebuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (getbufsize) */
	return (rs >= 0) ? port : rs ;
}
/* end subroutine (getserv_name) */


