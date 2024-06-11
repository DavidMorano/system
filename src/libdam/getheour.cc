/* getheour SUPPORT (Get Our Host Entry) */
/* lang=C++20 */

/* get a host name that has an INET address (of some sort: name or address) */
/* version %I% last-modified %G% */

#define	CF_SNSDS	1		/* use 'snsds(3dam)' */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Get a host entry from the Name Server databases.

	This subroutine is used to get a host entry struct for a
	host name.  It is not too fancy but will try to apply some
	standard defaults in order to get an entry back.  Names given
	to lookup will assume the current domain if one is not
	supplied with the name.  A NULL supplied name is assumed to
	refer to the current host.  A name specified in the INET
	style dotted-decimal format is also acceptable.

	Remember that a design goal is to MINIMIZE the number of
	DNS lookups used.  In general, DNS lookups are very slow.

	Synopsis:
	int getheour(name,hostname,hep,hbuf,hlen)
	cchar	name[] ;
	char		hostname[] ;
	HOSTENT	*hep ;
	char		hbuf[] ;
	int		hlen ;

	Arguments:
	name		name to lookup an entry for
	hostname	optional buffer to hold name actually used for lookup
	hep		pointer to 'hostent' structure
	hbuf		user specified storage area for returned data
	hlen		length of user specified storage buffer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"getheour.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	gethename(HOSTENT *,char *,int,cchar *) noex ;
    extern int	getheaddr(HOSTENT *,char *,int,cvoid *) noex ;
}


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */

constexpr in_addr_t	anyaddr = 0 ;


/* exported variables */


/* exported subroutines */

int getheour(HOSTENT *hep,char *hbuf,int hlen,char *hostname,cc *name) noex {
	UTSNAME		uts ;
	in_addr_t	addr ;
	int		rs ;
	int		len ;
	char		localnode[NODENAMELEN + 1] ;
	char		localdomain[MAXHOSTNAMELEN + 1] ;
	char		newname[MAXHOSTNAMELEN + 1] ;
	char		altname[MAXHOSTNAMELEN + 1] ;
	char		*np = (char *) name ;
	char		*ap ;
	char		*cp ;

/* are we "doing" ourselves? */

	np = (char *) name ;
	if (name == NULL) {

	    if (u_uname(&uts) < 0) {
	        np = "localhost" ;
	    } else
	        np = uts.nodename ;

	} else {

/* strip trailing dots */

	    len = strnlen(np,MAXHOSTNAMELEN) ;

	    if (name[len - 1] == '.') {

	        while (len && (np[len - 1] == '.'))
			len -= 1 ;

	        snwcpy(altname,MAXHOSTNAMELEN,np,len) ;

	        np = altname ;

	    } /* end if */

	} /* end if (stuff on the end) */

/* is it a numeric address? */

	if ((addr = inet_addr(np)) != (~ 0)) {

	    ap = (char *) &addr ;
	    rs = getheaddr(ap,hep,hbuf,hlen) ;

#ifdef	COMMENT
	     if ((rs < 0) && 
		(memcmp(&addr,&anyaddr,sizeof(struct in_addr_t)) == 0)) {


	    }
#endif /* COMMENT */

	} else {
		rs = gethename(hep,hbuf,hlen,np) ;
	}

	if (rs >= 0) {

	    if (hostname != NULL)
	        sncpy1(hostname,MAXHOSTNAMELEN,np) ;

	    return SR_OK ;

	} else if (addr != (~ 0))
		return SR_NOTFOUND ;

/* get our local domain name */

	localdomain[0] = '\0' ;
	getnodedomain(localnode,localdomain) ;

/* does the given name have a domain already? */

	if ((cp = strchr(np,'.')) != NULL) {

/* is the given domain our local domain name? */

	    if (strcmp((cp + 1),localdomain) == 0) {

/* yes */

	        snwcpy(newname,MAXHOSTNAMELEN,name,(cp - name)) ;

/* try it without our trailing local domain name */

	        rs = gethename(hep,hbuf,hlen,peername) ;
		if (rs >= 0) {

	            if (hostname != NULL)
	                sncpy1(hostname,MAXHOSTNAMELEN,hep->h_name) ;

	            return SR_OK ;
	        }

	    } /* end if (domains are the same) */

	} else {

/* it didn't have a domain so we try it with our local domain on it */

#if	CF_SNSDS
	    snsds(newname,MAXHOSTNAMELEN,name,localdomain) ;
#else /* CF_SNSDS */
	    sncpy3(newname,MAXHOSTNAMELEN,name,".",localdomain) ;
#endif /* CF_SNSDS */

	    if ((rs = gethename(hep,hbuf,hlen,newname)) >= 0) {
	        if (hostname) {
	            sncpy1(hostname,MAXHOSTNAMELEN,hep->h_name) ;
		}
	        return OK ;
	    }

	} /* end if */

	if (hostname) {
	    hostname[0] = '\0' ;
	}

	return SR_NOTFOUND ;
}
/* end subroutine (getheour) */


