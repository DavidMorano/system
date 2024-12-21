/* ucgethost SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* subroutine to get a single "host" entry (raw) */
/* version %I% last-modified %G% */

#define	CF_USERENT	1		/* use reentrant API if available */

/* revision history:

	= 1998-03-21, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_gethostbyname

	Description:
	This subroutine is a platform independent subroutine to get
	an INET host address entry, but does it dumbly on purpose.

	Synopsis:
	int uc_gethostbyname(ucentho *hep,char *hebuf,int helen,cc *name) noex

	Arguments:
	- hep		pointer to 'hostent' structure
	- hebuf		user supplied buffer to hold result
	- helen		length of user supplied buffer
	- name		name to lookup

	Returns:
	0		host was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found


	Name:
	uc_gethostbyaddr

	Description:
	This subroutine is a platform independent subroutine to get
	an INET host address entry, but does it dumbly on purpose.

	Synopsis:
	int uc_gethostbyaddr(ucentho *hep,char *hebuf,int helen,
			int af,cc *ap,int al) noex

	Arguments:

	- hep		pointer to 'hostent' structure
	- hebuf		user supplied buffer to hold result
	- helen		length of user supplied buffer
	- af		address-family of address to look up
	- ap		address to lookup
	- al		length if the supplied address

	Returns:
	0		host was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<netdb.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<storeitem.h>
#include	<hostent.h>
#include	<localmisc.h>

#include	"ucgethost.h"


/* local defines */

#if	(! defined(SYSHAS_GETHOSTXXXR)) || (SYSHAS_GETHOSTXXXR == 0)
#undef	CF_USERENT
#define	CF_USERENT	0
#endif /* SYSHAS_GETHOSTXXXR */

#define	NLOOPS		3


/* external subroutines */


/* external variables */

#if	(CF_USERENT == 0)
extern int	h_errno ;
#endif


/* local structures */


/* forward references */

static int	gethosterr(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_gethostbegin(int stayopen) noex {
	int		rs = SR_INVALID ;
	if (stayopen >= 0) {
	    sethostent(stayopen) ;
	}
	return rs ;
}
/* end subrouttine (uc_gethostbegin) */

int uc_gethostend() noex {
	endhostent() ;
	return SR_OK ;
}
/* end subrouttine (uc_gethostend) */

int uc_gethostent(HOSTENT *hep,char *hebuf,int helen) noex {
	HOSTENT		*lp ;
	int		rs = SR_OK ;

	if (hep == NULL) return SR_FAULT ;
	if (hebuf == NULL) return SR_FAULT ;

	if (helen <= 0) return SR_OVERFLOW ;

/* do the real work */

#if	CF_USERENT
	{
	    int		h_errno ;

	    for (int i = 0 ; i < NLOOPS ; i += 1) {
	        if (i > 0) msleep(1000) ;

	        rs = SR_OK ;
	        errno = 0 ;
	        h_errno = 0 ;
	        lp = gethostent_r(hep,hebuf,helen,&h_errno) ;

	        if (lp != NULL) break ;
	        rs = gethosterr(h_errno) ;
	        if (rs != SR_AGAIN) break ;

	    } /* end for */

	    if (rs >= 0) {
	        rs = hostent_size(hep) ;
	    } else if (rs == SR_AGAIN) {
	        rs = SR_TIMEDOUT ;
	    }

	}
#else /* CF_USERENT */
	{
	    for (int i = 0 ; i < NLOOPS ; i += 1) {
	        if (i > 0) msleep(1000) ;

	        rs = SR_OK ;
	        errno = 0 ;
	        h_errno = 0 ;
	        lp = gethostent() ;
	        if (lp != NULL) break ;

	        rs = gethosterr(h_errno) ;
	        if (rs != SR_AGAIN) break ;

	    } /* end for */

	    if (rs >= 0) {
	        rs = hostent_load(hep,hebuf,helen,lp) ;
	    } else if (rs == SR_AGAIN) {
	        rs = SR_TIMEDOUT ;
	    }

	}
#endif /* CF_USERENT */

	if (rs == SR_PROTONOSUPPORT) rs = SR_OK ;

	return rs ;
}
/* end subroutine (uc_gethostent) */

int uc_gethostbyname(HOSTENT *hep,char *hebuf,int helen,cc *name) noex {
	HOSTENT		*lp ;
	int		rs = SR_OK ;

	if (name == NULL) return SR_FAULT ;
	if (hep == NULL) return SR_FAULT ;
	if (hebuf == NULL) return SR_FAULT ;

	if (helen <= 0) return SR_OVERFLOW ;

/* do the real work */

#if	CF_USERENT
	{
	    int	h_errno ;
	    for (int i = 0 ; i < NLOOPS ; i += 1) {
	        if (i > 0) msleep(1000) ;

	        rs = SR_OK ;
	        errno = 0 ;
	        h_errno = 0 ;
	        lp = gethostbyname_r(name,hep,hebuf,helen,&h_errno) ;

	        if (lp != NULL) break ;
	        rs = gethosterr(h_errno) ;
	        if (rs != SR_AGAIN) break ;

	    } /* end for */

	    if (rs >= 0) {
	        rs = hostent_size(hep) ;
	    } else if (rs == SR_AGAIN) {
	        rs = SR_TIMEDOUT ;
	    }

	}
#else /* CF_USERENT */
	{

	    for (int i = 0 ; i < NLOOPS ; i += 1) {
	        if (i > 0) msleep(1000) ;

	        rs = SR_OK ;
	        errno = 0 ;
	        h_errno = 0 ;
	        lp = gethostbyname(name) ;

	        if (lp != NULL) break ;
	        rs = gethosterr(h_errno) ;
	        if (rs != SR_AGAIN) break ;

	    } /* end for */

	    if (rs >= 0) {
	        rs = hostent_load(hep,hebuf,helen,lp) ;
	    } else if (rs == SR_AGAIN) {
	        rs = SR_TIMEDOUT ;
	    }

	}
#endif /* CF_USERENT */

	return rs ;
}
/* end subroutine (uc_gethostbyname) */

int uc_gethostbyaddr(HOSTENT *hep,char *hebuf,int helen,
		int af,cc *ap,int al) noex {
	HOSTENT		*lp ;
	int		rs = SR_OK ;

	if (hep == NULL) return SR_FAULT ;
	if (hebuf == NULL) return SR_FAULT ;
	if (ap == NULL) return SR_FAULT ;

	if (helen <= 0) return SR_OVERFLOW ;

/* do the real work */

#if	CF_USERENT
	{
	    int	h_errno ;
	    for (int i = 0 ; i < NLOOPS ; i += 1) {
	        if (i > 0) msleep(1000) ;

	        rs = SR_OK ;
	        errno = 0 ;
	        h_errno = 0 ;
	        lp = gethostbyaddr_r(ap,al,af,
	            hep,hebuf,helen,&h_errno) ;

	        if (lp != NULL) break ;

	        rs = gethosterr(h_errno) ;
	        if (rs != SR_AGAIN) break ;

	    } /* end for */

	    if (rs >= 0) {
	        rs = hostent_size(hep) ;
	    } else if (rs == SR_AGAIN) {
	        rs = SR_TIMEDOUT ;
	    }

	}
#else /* CF_USERENT */
	{

	    for (int i = 0 ; i < NLOOPS ; i += 1) {
	        if (i > 0) msleep(1000) ;

	        rs = SR_OK ;
	        errno = 0 ;
	        h_errno = 0 ;
	        lp = gethostbyaddr(ap,al,af) ;
	        if (lp != NULL) break ;

	        rs = gethosterr(h_errno) ;
	        if (rs != SR_AGAIN) break ;

	    } /* end for */

	    if (rs >= 0) {
	        rs = hostent_load(hep,hebuf,helen,lp) ;
	    } else if (rs == SR_AGAIN) {
	        rs = SR_TIMEDOUT ;
	    }

	}
#endif /* CF_USERENT */

	return rs ;
}
/* end subroutine (uc_gethostbyaddr) */


/* local subroutines */

static int gethosterr(int h_errno) noex {
	int		rs = SR_OK ;
	if (errno != 0) {
	    rs = (- errno) ;
	} else {
	    switch (h_errno) {
	    case TRY_AGAIN:
	        rs = SR_AGAIN ;
	        break ;
	    case NO_DATA:
	        rs = SR_HOSTUNREACH ;
	        break ;
	    case NO_RECOVERY:
	        rs = SR_IO ;
	        break ;
	    case HOST_NOT_FOUND:
	        rs = SR_NOTFOUND ;
	        break ;
#ifdef	NETDB_INTERNAL /* not everyone (MacOS) has this! */
	    case NETDB_INTERNAL:
	        rs = SR_NOANODE ;
	        break ;
#endif /* NETDB_INTERNAL */
	    default:
	        rs = SR_PROTONOSUPPORT ;
	        break ;
	    } /* end switch */
	} /* end if (errno) */
	return rs ;
}
/* end subroutine (gethosterr) */


