/* uc_getnet */
/* lang=+20 */

/* interface component for UNIX® library-3c */
/* subroutine to get a network entry (raw) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getnetbyname

	Description:
	This subroutine is a platform independent subroutine to get an
	INET 'network' (by name).

	Synopsis:
	int uc_getnetbyname(NW *nep,char *rbuf,int rlen,cchar *name) noex

	Arguments:
	- nep		pointer to 'hostent' structure
	- rbuf		user supplied buffer to hold result
	- rlen		length of user supplied buffer
	- name		network name to lookup

	Returns:
	0		host was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found
	SR_OVERFLOW	buffer overflow


	Name:
	uc_getnetbyaddr

	Description:
	This subroutine is a platform independent subroutine to get an
	INET 'network' (by number).

	Synopsus:
	int uc_getnetbyaddr(NW *eep,char *rbuf,int rlen,long addr,int type) noex

	Arguments:
	- nep		pointer to 'hostent' structure
	- rbuf		user supplied buffer to hold result
	- rlen		length of user supplied buffer
	- addr		address to lookup
	- type		type of address to lookup

	Returns:
	0		host was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found
	SR_OVERFLOW	buffer overflow

	Notes:

	- structure

	struct netent {
		char		*n_name;	# official name of net
		char		**n_aliases;	# alias list
		int		n_addrtype;	# net address type
		in_addr_t	n_net;		# network number
	} ;

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<errno.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<storeitem.h>
#include	<intceil.h>
#include	<localmisc.h>


/* local defines */

#define	TO_AGAIN	10		/* seconds */

#define	RF_NETCOPY	0		/* flag to set need of 'netcopy()' */
#if	defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0)
#else
#undef	RF_NETCOPY
#define	RF_NETCOPY	1		/* flag to set need of 'netcopy()' */
#endif

#define	NW		struct netent


/* external subroutines */


/* external variables */


/* local structures */

#if	defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0)
static int	netsize(NETENT *) noex ;
#endif

#if	RF_NETCOPY
static int	netcopy(NETENT *,char *,int,const NETENT *) noex ;
static int	si_copystr(STOREITEM *,char **,cchar *) noex ;
#endif


/* forward references */


/* local variables */


/* exported subroutines */

int uc_setnetent(int stayopen) noex {
	setnetent(stayopen) ;
	return SR_OK ;
}

int uc_endnetent() noex {
	endnetent() ;
	return SR_OK ;
}

int uc_getnetent(NW *nep,char *rbuf,int rlen) noex {
	struct netent	*rp ;
	int		rs = SR_OK ;
	int		to_again = TO_AGAIN ;
	int		f_exit = false ;

	if (nep == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (rlen <= 0) return SR_OVERFLOW ;

/* note (carefully) that there is NO POSIX standard version of this funtion */

	repeat {

#if	defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0)
	    {
	        errno = 0 ;
	        rp = getnetent_r(nep,rbuf,rlen) ;
	        if (rp != nullptr) {
	            rs = netsize(nep) ;
	        } else {
	            rs = (- errno) ;
	        }
	    }
#else /* defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0) */
	    {
	        errno = 0 ;
	        rp = getnetent() ;
	        if (rp != nullptr) {
	            rs = netcopy(nep,rbuf,rlen,rp) ;
	        } else {
	            rs = (- errno) ;
	        }
	    }
#endif /* defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0) */

	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
	                msleep(1000) ;
	            } else {
	                rs = SR_TIMEDOUT ;
	                f_exit = true ;
	            }
	            break ;
	        case SR_INTR:
	            break ;
	        default:
	            f_exit = true ;
	            break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (uc_getnetent) */

int uc_getnetbyname(NW *nep,char *rbuf,int rlen,cchar *name) noex {
	struct netent	*rp ;
	int		rs = SR_OK ;
	int		to_again = TO_AGAIN ;
	int		f_exit = false ;

	if (name == nullptr) return SR_FAULT ;
	if (nep == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (rlen <= 0) return SR_OVERFLOW ;

/* do the real work */

	repeat {

#if	defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0)
	    {
	        errno = 0 ;
	        rp = getnetbyname_r(name,nep,rbuf,rlen) ;
	        if (rp != nullptr) {
	            rs = netsize(nep) ;
	        } else {
	            rs = (errno == 0) ? SR_NOTFOUND : (- errno) ;
	        }
	    }
#else /* defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0) */
	    {
	        errno = 0 ;
	        rp = getnetbyname(name) ;
	        if (rp != nullptr) {
	            rs = netcopy(nep,rbuf,rlen,rp) ;
	        } else {
	            rs = (errno == 0) ? SR_NOTFOUND : (- errno) ;
	        }
	    }
#endif /* defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0) */

	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
	                msleep(1000) ;
	            } else {
	                rs = SR_TIMEDOUT ;
	                f_exit = true ;
	            }
	            break ;
	        case SR_INTR:
	            break ;
	        default:
	            f_exit = true ;
	            break ;
	        } /* end switch */
	    } /* end if */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (uc_getnetbyname) */

int uc_getnetbyaddr(NW *nep,char *rbuf,int rlen,long addr,int type) noex {
	struct netent	*rp ;
	int		rs = SR_OK ;
	int		to_again = TO_AGAIN ;
	int		f_exit = false ;

	if (nep == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (rlen <= 0) return SR_OVERFLOW ;

/* do the real work */

	repeat {

#if	defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0)
	    {
	        errno = 0 ;
	        rp = getnetbyaddr_r(addr,type,nep,rbuf,rlen) ;
	        if (rp != nullptr) {
	            rs = netsize(nep) ;
	        } else {
	            rs = (errno == 0) ? SR_NOTFOUND : (- errno) ;
	        }
	    }
#else /* defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0) */
	    {
	        errno = 0 ;
	        rp = getnetbyaddr(addr,type) ;
	        if (rp != nullptr) {
	            rs = netcopy(nep,rbuf,rlen,rp) ;
	        } else {
	            rs = (errno == 0) ? SR_NOTFOUND : (- errno) ;
	        }
	    }
#endif /* defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0) */

	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
	                msleep(1000) ;
	            } else {
	                rs = SR_TIMEDOUT ;
	                f_exit = true ;
	            }
	            break ;
	        case SR_INTR:
	            break ;
	        default:
	            f_exit = true ;
	            break ;
	        } /* end switch */
	    } /* end if */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (uc_getnetaddr) */


/* local subroutines */

#if	defined(SYSHAS_GETNETXXXR) && (SYSHAS_GETNETXXXR > 0)
static int netsize(NW *nep) noex {
	int		rs = SR_FAULT ;
	if (nep) {
	    int		size = 0 ;
	    if (nep->n_name) {
	        size += (strlen(nep->n_name)+1) ;
	    }
	    if (nep->n_aliases) {
	        int	i ;
	        for (i = 0 ; nep->n_aliases[i] ; i += 1) {
	            size += (strlen(nep->n_aliases[i])+1) ;
	        } /* end for */
	        size += (i*sizeof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(size,sizeof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (netsize) */
#endif

#if	RF_NETCOPY

static int netcopy(NW *nep,char *rbuf,int rlen,const NW *snep) noex {
	STOREITEM	ib ;
	int		rs ;
	int		rs1 ;
	memcpy(nep,snep,sizeof(NETENT)) ;
	if ((rs = storeitem_start(&ib,rbuf,rlen)) >= 0) {
	    if (snep->n_aliases) {
	        int	n ;
	        void	**tab ;
	        for (n = 0 ; snep->n_aliases[n] ; n += 1) ;
	        if ((rs = storeitem_ptab(&ib,n,&tab)) >= 0) {
	            int		i ;
	            cchar	**aliases = (cchar **) snep->n_aliases ;
	            nep->n_aliases = (char **) tab ;
	            for (i = 0 ; snep->n_aliases[i] ; i += 1) {
	                rs = si_copystr(&ib,(nep->n_aliases + i),aliases[i]) ;
	                if (rs < 0) break ;
	            }
	            nep->n_aliases[i] = nullptr ;
	        } /* end if */
	    } /* end if (aliases) */
	    si_copystr(&ib,&nep->n_name,snep->n_name) ;
	    rs1 = storeitem_finish(&ib) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (storeitem) */
	return rs ;
}
/* end subroutine (netcopy) */

static int si_copystr(STOREITEM *ibp,char **pp,cchar *p1) noex {
	int		rs = SR_OK ;
	cchar		**cpp = (cchar **) pp ;
	*cpp = nullptr ;
	if (p1) {
	    rs = storeitem_strw(ibp,p1,-1,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copystr) */

#endif /* RF_NETCOPY */


