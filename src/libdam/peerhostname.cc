/* peerhostname SUPPORT */
/* lang=C++20 */

/* get a peer host name if there is one */
/* version %I% last-modified %G% */


/* revision history:

	= 1986-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine will take an INET socket and a local domain
	name and find the hostname of the remote end of the socket.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<uinet.h>		/* |INETXADDRLEN| */
#include	<getbufsize.h>
#include	<getxx.h>
#include	<hostent.h>
#include	<sockaddress.h>
#include	<inetaddr.h>
#include	<strwcpy.h>
#include	<isindomain.h>
#include	<isnot.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int peerhostname(int s,cchar *domainname,char *peername) noex {
	sockaddress	sa ;
	int		alen = sizeof(sockaddress) ;
	int		rs ;

	if (peername == NULL) return SR_FAULT ;

	peername[0] = '\0' ;
	if ((rs = u_getpeername(s,&sa,&alen)) >= 0) {
	    if ((rs = sockaddress_getaf(&sa)) >= 0) {
		cint	af = rs ;
		if (af == AF_INET) {
		    INADDR	naddr ;
		    cint	helen = getbufsize(getbufsize_ho) ;
		    cint	ialen = INETXADDRLEN ;
		    char	*hebuf ;
	    	    sockaddress_getaddr(&sa,&naddr,ialen) ;
		    if ((rs = uc_malloc((helen+1),&hebuf)) >= 0) {
			ucentho	he ;
			cchar	*na = charp(&naddr) ;
			cint	nlen = MAXHOSTNAMELEN ;

/* lookup this IP (INET4) address */

	    	        rs = getho_addr(&he,hebuf,helen,af,na,ialen) ;
	    		if (rs >= 0) {
			    /* hostent	*hep = ((hostent *) &he) ; */
	        	    hostent_cur	hc ;
	        	    cchar	*np ;

	        if (domainname != NULL) {
	            if ((rs = hostent_curbegin(&he,&hc)) >= 0) {
	                while ((rs = hostent_curenumname(&he,&hc,&np)) > 0) {
	                    if (isindomain(np,domainname)) break ;
	                } /* end while */
	                hostent_curend(&he,&hc) ;
		    } /* end if (hostent) */
	        } else {
	            rs = SR_HOSTUNREACH ;
		}
	        if (rs >= 0) {
		    cchar	*tp ;
	            strwcpy(peername,np,nlen) ;
	            if ((tp = strchr(peername,'.')) != NULL) {
			peername[tp-peername] = '\0' ;
		    }
	        } else {
	            hostent_getcanonical(&he,&np) ;
	            strwcpy(peername,np,nlen) ;
	        } /* end if */

	    } else if (isNotPresent(rs)) {
	        inetaddr	ia ;
	        if ((rs = inetaddr_start(&ia,&na)) >= 0) {
	            rs = inetaddr_getdotaddr(&ia,peername,nlen) ;
	            inetaddr_finish(&ia) ;
		} /* end if */
	    } /* end if */

			uc_free(hebuf) ;
		    } /* end if (m-a) */
		} /* end if (INET) */
	    } /* end if (sockaddress_getaf) */
	} /* end if (got an INET host entry) */

	return (peername[0] != '\0') ? strlen(peername) : SR_NOTFOUND ;
}
/* end subroutine (peerhostname) */


