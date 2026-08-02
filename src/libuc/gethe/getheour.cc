/* getheour SUPPORT (Get Our Host Entry) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a host name that has an INET address (of some sort: name or address) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getheour

	Description:
	Get a host entry from the Name Server databases.  This
	subroutine is used to get a host entry object for a host
	name.  It is not too fancy but will try to apply some
	standard defaults in order to get an entry back.  Names
	given to lookup will assume the current domain if one is
	not supplied with the name.  A NULL supplied name is assumed
	to refer to the current host.  A name specified in the INET
	style dotted-decimal format is also acceptable.  Remember
	that a design goal is to MINIMIZE the number of DNS lookups
	used.  In general, DNS lookups are very slow.

	Synopsis:
	int getheour(ucentho *hep,char *hbuf,int hlen,char *hb,cc *name) noex

	Arguments:
	hep		pointer to 'hostent' structure
	hbuf		user specified storage area for returned data
	hlen		length of user specified storage buffer
	hb		optional buffer to hold name actually used for lookup
	name		name to lookup an entry for

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uinet.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucent.h>		/* LIBUC |ucentho(3uc)| */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC |getnodename(3uc)| */
#include	<getxx.h>		/* LIBUC */
#include	<hostent.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<isinetaddr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getheour.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef hostent *	hostentp ;


/* external subroutines */

extern "C" {
    extern int	gethename(ucentho *,char *,int,cchar *) noex ;
    extern int	getheaddr(ucentho *,char *,int,cvoid *) noex ;
}


/* external variables */


/* local typedefs */


/* local structures */

namespace {
    struct gether {
	ucentho		*hep ;
	char		*hbuf ;
	char		*hebuf ;
	cchar		*hn ;
	int		helen ;
	gether(ucentho *e,char *b,int l,char *h,cc *n) noex {
	    hep = e ;
	    hebuf = b ;
	    helen = l ;
	    hbuf = h ;
	    hn = n;
	} ; /* end ctor */
	operator int () noex ;
	int trylocal() noex ;
	int gethox(cchar *) noex ;
	int tryname(cchar *) noex ;
	int findcanonical() noex ;
    } ; /* end struct (gether) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr in_addr_t	anyaddr = 0 ;


/* exported variables */


/* exported subroutines */

int getheour(ucentho *hep,char *hebuf,int helen,char *hbuf,cc *hn) noex {
	int		rs = SR_FAULT ;
	if (hep && hebuf && hn) ylikely {
	    gether go(hep,hebuf,helen,hbuf,hn) ;
	    rs = go ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getheour) */


/* private subroutines */

gether::operator int () noex {
	int		rs = SR_OK ;
	if (hn) ylikely {
	    if (hn[0]) {
	        rs = tryname(hn) ;
	    } else {
	        rs = trylocal() ;
	    }
	} else {
	    rs = trylocal() ;
	}
	return rs ;
} /* end method (gether::operator) */

int gether::trylocal() noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *nbuf ; (rs = lm_nn(&nbuf)) >= 0) ylikely {
	    cint	nlen = rs ;
	    if ((rs = getnodename(nbuf,nlen)) >= 0) ylikely {
		rs = tryname(nbuf) ;
		len = rs ;
	    } /* end if (getnodename) */
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end method (gether::trylocal) */

int gether::gethox(cchar *name) noex {
    	int		rs ;
	if (isinetaddr(name)) {
	    cint	af = AF_UNSPEC ;
	    rs = getho_addr(hep,hebuf,helen,af,name,-1) ;
	} else {
	    rs = getho_name(hep,hebuf,helen,name) ;
	}
	return rs ;
} /* end method (gether::gethox) */

int gether::tryname(cchar *name) noex {
	int		rs ;
	int		len = 0 ;
	if (((rs = gethox(name)) >= 0) && hbuf) {
	    rs = findcanonical() ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
} /* end method (gether::tryname) */

int gether::findcanonical() noex {
	int		rs = SR_OK ;
	if (hbuf) {
	    if ((rs = bufsizeget(bufsize_hostname)) >= 0) ylikely {
		cint	hlen = rs ;
	        hostent	*hop = cast_static<hostentp>(hep) ;
	        cchar	*canp ;
	        if ((rs = hostent_getcanonical(hop,&canp)) >= 0) {
		    rs = sncpy(hbuf,hlen,canp) ;
	        } /* end if (hostent_getcanonical) */
	    } /* end if (bufsizeget) */
	} /* end if (wanted) */
	return rs ;
} /* end method (gether::findcanonical) */


