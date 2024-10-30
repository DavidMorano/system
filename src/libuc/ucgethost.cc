/* ucgethost SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* subroutine to get a single "host" entry (raw) */
/* version %I% last-modified %G% */

#define	CF_USEREENT	1		/* use reentrant API if available */
#define	CF_GETHOSTENT	1		/* compile |uc_gethostent(3uc)| */

/* revision history:

	= 1998-11-01, David A­D­ Morano
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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<storeitem.h>
#include	<hostent.h>
#include	<localmisc.h>


/* local defines */

#if	(! defined(SYSHAS_GETHOSTXXXR)) || (SYSHAS_GETHOSTXXXR == 0)
#undef	CF_USEREENT
#define	CF_USEREENT	0
#endif /* SYSHAS_GETHOSTXXXR */

#define	NLOOPS		3


/* external subroutines */


/* external variables */

#if	(CF_USEREENT == 0)
extern int	h_errno ;
#endif


/* local structures */

#if	CF_DEBUGS
struct herrno {
	int		herrno ;
	cchar		*name ;
} ;
#endif /* CF_DEBUGS */


/* forward references */

static int	gethosterr(int) ;

#if	CF_DEBUGS
static cchar	*strherrno(int) ;
#endif /* CF_DEBUGS */


/* local variables */

#if	CF_DEBUGS
static struct herrno	herrnos[] = {
	{ TRY_AGAIN, "try_again" },
	{ NO_RECOVERY, "no_recovery" },
	{ NO_DATA, "no_data" },
	{ NO_ADDRESS, "no_address" },
	{ HOST_NOT_FOUND, "host_not_found" },
	{ NETDB_INTERNAL, "netdb_internal" },
	{ 0, "zero" }
} ;
#endif /* CF_DEBUGS */


/* exported variables */


/* exported subroutines */

int uc_sethostent(int stayopen) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = sethostent(stayopen)) != 0) {
	    rs = (errno == 0) ? SR_IO : (- errno) ;
	}
	return rs ;
}
/* end subrouttine (uc_sethostent) */

int uc_endhostent() noex {
	int		rs ;
	errno = 0 ;
	if ((rs = endhostent()) != 0) {
	    rs = (errno == 0) ? SR_IO : (- errno) ;
	}
	return rs ;
}
/* end subrouttine (uc_endhostent) */

#if	CF_GETHOSTENT
int uc_gethostent(HOSTENT *hep,char *hebuf,int helen) noex {
	OSTENT		*lp ;
	int		rs = SR_OK ;
	int		i ;

	if (hep == NULL) return SR_FAULT ;
	if (hebuf == NULL) return SR_FAULT ;

	if (helen <= 0) return SR_OVERFLOW ;

/* do the real work */

#if	CF_USEREENT
	{
	    int		h_errno ;

	    for (i = 0 ; i < NLOOPS ; i += 1) {
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
#else /* CF_USEREENT */
	{

	    for (i = 0 ; i < NLOOPS ; i += 1) {
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
#endif /* CF_USEREENT */

	if (rs == SR_PROTONOSUPPORT) rs = SR_OK ;

	return rs ;
}
/* end subroutine (uc_gethostent) */
#endif /* CF_GETHOSTENT */

int uc_gethostbyname(HOSTENT *hep,char *hebuf,int helen,cc *name) noex {
	HOSTENT		*lp ;
	int		rs = SR_OK ;
	int		i ;

	if (name == NULL) return SR_FAULT ;
	if (hep == NULL) return SR_FAULT ;
	if (hebuf == NULL) return SR_FAULT ;

	if (helen <= 0) return SR_OVERFLOW ;

/* do the real work */

#if	CF_USEREENT
	{
	    int	h_errno ;

	    for (i = 0 ; i < NLOOPS ; i += 1) {
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
#else /* CF_USEREENT */
	{

	    for (i = 0 ; i < NLOOPS ; i += 1) {
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
#endif /* CF_USEREENT */

	return rs ;
}
/* end subroutine (uc_gethostbyname) */

int uc_gethostbyaddr(HOSTENT *hep,char *hebuf,int helen,
		int af,cc *ap,int al) noex {
	HOSTENT		*lp ;
	int		rs = SR_OK ;
	int		i ;

	if (addr == NULL) return SR_FAULT ;
	if (hep == NULL) return SR_FAULT ;
	if (hebuf == NULL) return SR_FAULT ;

	if (helen <= 0) return SR_OVERFLOW ;

/* do the real work */

#if	CF_USEREENT
	{
	    int	h_errno ;

	    for (i = 0 ; i < NLOOPS ; i += 1) {
	        if (i > 0) msleep(1000) ;

	        rs = SR_OK ;
	        errno = 0 ;
	        h_errno = 0 ;
	        lp = gethostbyaddr_r(addr,addrlen,af,
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
#else /* CF_USEREENT */
	{

	    for (i = 0 ; i < NLOOPS ; i += 1) {
	        if (i > 0) msleep(1000) ;

	        rs = SR_OK ;
	        errno = 0 ;
	        h_errno = 0 ;
	        lp = gethostbyaddr(addr,addrlen,af) ;
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
#endif /* CF_USEREENT */

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


