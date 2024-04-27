/* usocket SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:
	This module contains the UNIX® socket system calls (yes, they 
	are sustem calls now-a-days).

	Name:
	u_send

	Description:
	This is the famous (infamous) old |send(2)| system call.
	Of course, it has not been a system call for almost two
	decades (on System V UNIX) but that is still OK! :-)

	Name:
	u_shutdown

	Description:
	Shut down one or both directions of a (connected) socket.
	Possible shutdown commands are:
		SHUT_RD
		SHUT_WR
		SHUT_RDWR

	Synopsis:
	int u_shutdown(int fd,int dir) noex{

	Arguments:
	fd	socket file-descriptor
	dir	shutdown command; one of:
			SHUT_RD
			SHUT_WR
			SHUT_RDWR

	Returns:
	>=0	OK
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<climits>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */

using namespace	ufiledesc ;		/* namespace */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct usocket : ufiledesc {
	void		*sap ;
	int		sal ;
	usocket(int afd) noex : ufiledesc(afd) { } ;
} ; /* end struct (usocket) */


/* forward references */


/* local variables */


/* external subroutines */


/* exported subroutines */

int u_bind(int fd,void *sap,int sal) noex {
	usocket		so(sa,sal) ;
	so.m = &usocket::ibind ;
	return oo(fd) ;
}
/* end subroutine (u_bind) */

int usocket::ibind(int fd) noex {

	SOCKADDR	*sap = (SOCKADDR *) asap ;
	SALEN_T		sal ;
	int		rs ;

	    sal = (SALEN_T) asal ;
	    if ((rs = bind(s,sap,sal)) < 0) rs = (- errno) ;

	return rs ;
}
/* end subroutine (u_bind) */

int u_listen(int s,int backlog) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = listen(s,backlog)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_NOBUFS:
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_listen) */

int u_setsockopt(int s,int level,int optname,cvoid *optvalp,int optlen) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = setsockopt(s,level,optname,optvalp,optlen)) < 0) {
	        rs = (- errno) ;
	    }
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_setsockopt) */

int u_getsockopt(int s,int level,int optname,int *optvalp,int *optlen) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = getsockopt(s,level,optname,optvalp,optlen)) < 0) {
	        rs = (- errno) ;
	    }
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_getsockopt) */

int u_getpeername(int s,void *fromp,int *lenp) noex {
	SOCKADDR	*sap = (SOCKADDR *) fromp ;
	SALEN_T		sal ;
	int		rs ;
	int		dummy ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		len = 0 ;
	int		f_exit = FALSE ;

	if (lenp == NULL) {
	    dummy = 0 ;
	    lenp = &dummy ;
	}

	repeat {
	    sal = (SALEN_T) *lenp ;
	    if ((rs = getpeername(s,sap,&sal)) < 0) rs = (- errno) ;
	    *lenp = (int) sal ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	len = *lenp ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (u_getpeername) */

int u_getsockname(int s,void *fromp,int *lenp) noex {
	SOCKADDR	*sap = (SOCKADDR *) fromp ;
	SALEN_T		sal ;
	int		rs ;
	int		dummy ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		len = 0 ;
	int		f_exit = FALSE ;

	if (lenp == NULL) {
	    dummy = 0 ;
	    lenp = &dummy ;
	}

	repeat {
	    sal = *lenp ;
	    if ((rs = getsockname(s,sap,&sal)) < 0) rs = (- errno) ;
	    *lenp = sal ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	len = *lenp ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (u_getsockname) */

int u_send(int fd,char *rbuf,int rlen,int flags) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = FALSE ;

	repeat {
	    rs = send(fd,rbuf,(size_t) rlen,flags) ;
	    if (rs < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs)  {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_send) */

int u_sendmsg(int fd,MSGHDR *msgp,int flags) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = FALSE ;


	repeat {
	    rs = sendmsg(fd,msgp,flags) ;
	    if (rs < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs)  {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_sendmsg) */

int u_sendto(int fd,cvoid *mbuf,int mlen,int flags,void *asap,int sal) noex {
	SOCKADDR	*sap = (SOCKADDR *) asap ;
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = FALSE ;

	repeat {
	    rs = sendto(fd,mbuf,mlen,flags,sap,sal) ;
	    if (rs < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs)  {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_sendto) */

int u_recv(int fd,void *vbuf,int vlen,int flags) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		to_nobufs = TO_NOBUFS ;
	int		f_exit = FALSE ;


	repeat {
	    if ((rs = recv(fd,vbuf,vlen,flags)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs)  {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_NOBUFS:
	            if (to_nobufs-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_recv) */

int u_recvfrom(int fd,void *dbuf,int dlen,int fl,void *asap,int *asalp) noex {
	SOCKADDR	*sap = (SOCKADDR *) asap ;
	SALEN_T		sal ;
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		to_nobufs = TO_NOBUFS ;
	int		f_exit = FALSE ;

	repeat {
	    if (sap != NULL) sal = (SALEN_T) *asalp ;
	    if ((rs = recvfrom(fd,dbuf,dlen,fl,sap,&sal)) < 0) {
		rs = (- errno) ;
	    }
	    if (rs < 0) {
	        switch (rs)  {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_NOBUFS:
	            if (to_nobufs-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	if (asalp != NULL) *asalp = (int) sal ;

	return rs ;
}
/* end subroutine (u_recvfrom) */

int u_recvmsg(int fd,MSGHDR *msgp,int fl) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		to_nobufs = TO_NOBUFS ;
	int		f_exit = FALSE ;

	repeat {
	    rs = recvmsg(fd,msgp,fl) ;
	    if (rs < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs)  {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_NOBUFS:
	            if (to_nobufs-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_recvmsg) */

int u_shutdown(int fd,int dir) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = shutdown(fd,dir)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_AGAIN:
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_shutdown) */


