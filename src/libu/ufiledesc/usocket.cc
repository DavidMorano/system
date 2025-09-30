/* usocket SUPPORT */
/* charset=ISO8859-1 */
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
	usocket (module)
	u_bind
	u_listen
	u_setsockopt
	u_getsockopt
	u_getsockname
	u_getpeername
	u_send
	u_sendmsg
	u_sendto
	u_sendfiler
	u_recv
	u_recvmsg
	u_recvfrom
	u_shutdown

	Description:
	This module contains the UNIX® socket system calls (yes,
	they are sustem calls now-a-days).

	Name:
	u_send

	Description:
	This is the famous (infamous) old |send(2)| system call.
	Of course, it has now been a system call for almost two
	decades (on System V UNIX®) but that is still OK! :-)

	Name:
	u_shutdown

	Description:
	Shut down one or both directions of a (connected) socket.
	Possible shutdown commands are:
		SHUT_RD
		SHUT_WR
		SHUT_RDWR

	Synopsis:
	int u_shutdown(int fd,int dir) noex

	Arguments:
	fd	socket file-descriptor
	dir	shutdown command; one of:
			SHUT_RD
			SHUT_WR
			SHUT_RDWR

	Returns:
	>=0	OK
	<0	error code (system-return)

	Notes:
	On the |sendfile(3c)| subroutine signature, although Apple
	Darwin pretty much claims that they invented this interface,
	no one else has an interface (function signature) that
	matches that on Darwin.  Both Linux and Solaris® (and its
	derivatives) both have the signature that I am provinding 
	below.  The Apple Darwin subroutine (signature) is a sort
	of "IO-vector" flavored animal.  In as such, it is a superset
	of what everyone else has implemented.  Solaris® has implemented
	a seperate "IO-vector" flavored version, but theirs is
	called |sendfilev(3c)| (good for Solaris®).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<climits>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"ufiledesc.h"


/* local defines */

#define	CSA	CSOCKADDR


/* imported namespaces */

using namespace	ufiledesc ;		/* namespace */
using namespace	usys ;			/* namespace */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr nullptr_t		np{} ;

namespace {
    struct usocket ;
    struct usender ;
    typedef int (usocket::*usocket_m)(int) noex ;
    typedef int (usender::*usender_m)(int) noex ;
    struct usocket : ufiledescbase {
	usocket_m	m = nullptr ;
	CSOCKADDR	*sap ;
	CMSGHDR		*msgp ;
	cvoid		*valp ;
	int		*lenp ;
	int		sal ;
	int		len ;
	int		level ;
	int		name ;
	int		flags ;
	usocket() noex { } ;
	usocket(int backlog) noex {
	    sal = backlog ;
	} ;
	usocket(cvoid *vp,int l) noex {
	    sap = (CSOCKADDR *) vp ;
	    sal = l ;
	} ;
	usocket(int lev,int n,cvoid *vp,int vl) noex : level(lev) {
	    name = n ;
	    valp = vp ;
	    len = vl ;
	} ;
	usocket(int lev,int n,cvoid *vp,int *vlp) noex : level(lev) {
	    name = n ;
	    valp = vp ;
	    lenp = vlp ;
	} ;
	usocket(cvoid *vp,int *lp) noex : lenp(lp) { 
	    sap = (CSOCKADDR *) vp ;
	} 
	usocket(cvoid *wb,int wl,int fl,cvoid *vp = nullptr,int l = 0) noex {
	    sap = (CSA *) vp ;
	    sal = l ;
	    wbuf = wb ;
	    wlen = wl ;
	    flags = fl ;
	} ;
	usocket(void *rb,int rl,int fl,void *vp = np,int *lp = np) noex {
	    sap = (CSA *) vp ;
	    lenp = lp ;
	    rbuf = rb ;
	    rlen = rl ;
	    flags = fl ;
	} ;
	usocket(CMSGHDR *mp,int fl) noex : flags(fl) {
	    msgp = mp ;
	} ;
	int callstd(int fd) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ;
	int ibind(int) noex ;
	int ilisten(int) noex ;
	int isetsockopt(int) noex ;
	int igetsockopt(int) noex ;
	int igetpeername(int) noex ;
	int igetsockname(int) noex ;
	int isend(int) noex ;
	int isendmsg(int) noex ;
	int isendto(int) noex ;
	int irecv(int) noex ;
	int irecvmsg(int) noex ;
	int irecvfrom(int) noex ;
	int ishutdown(int) noex ;
    } ; /* end struct (usocket) */
    struct usender : usocket {
	usender_m	m = nullptr ;
	SFHDTR		*hdrp ;
	off_t		fo ;
	size_t		c ;
	int		s ;
	usender(int as,off_t afo,size_t ac) noex {
	    s = as ;
	    fo = afo ;
	    c = ac ;
	} ;
	int callstd(int fd) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ;
	int isendfile(int) noex ;
    } ; /* end struct (usender) */
}


/* forward references */


/* local variables */


/* external subroutines */


/* exported subroutines */

int u_bind(int fd,cvoid *sap,int sal) noex {
	usocket		so(sap,sal) ;
	so.m = &usocket::ibind ;
	return so(fd) ;
}
/* end subroutine (u_bind) */

int u_listen(int fd,int backlog) noex {
	usocket		so(backlog) ;
	so.m = &usocket::ilisten ;
	return so(fd) ;
}
/* end subroutine (u_listen) */

int u_setsockopt(int fd,int level,int optname,cvoid *valp,int len) noex {
	usocket		so(level,optname,valp,len) ;
	so.m = &usocket::isetsockopt ;
	return so(fd) ;
}
/* end subroutine (u_setsockopt) */

int u_getsockopt(int fd,int level,int optname,void *valp,int *lenp) noex {
	usocket		so(level,optname,valp,lenp) ;
	so.m = &usocket::igetsockopt ;
	return so(fd) ;
}
/* end subroutine (u_getsockopt) */

int u_getpeername(int fd,void *sap,int *lenp) noex {
	int		rs = SR_FAULT ;
	if (sap && lenp) {
	    usocket	so(sap,lenp) ;
	    so.m = &usocket::igetpeername ;
	    rs = so(fd) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_getpeername) */

int u_getsockname(int fd,void *sap,int *lenp) noex {
	int		rs = SR_FAULT ;
	if (sap && lenp) {
	    usocket	so(sap,lenp) ;
	    so.m = &usocket::igetsockname ;
	    rs = so(fd) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_getsockname) */

int u_send(int fd,cvoid *wbuf,int wlen,int flags) noex {
	int		rs = SR_FAULT ;
	if (wbuf) {
	    usocket	so(wbuf,wlen,flags) ;
	    so.m = &usocket::isend ;
	    rs = so(fd) ;
	}
	return rs ;
} 
/* end subroutine (u_send) */

int u_sendmsg(int fd,CMSGHDR *msgp,int flags) noex {
	int		rs = SR_FAULT ;
	if (msgp) {
	    usocket	so(msgp,flags) ;
	    so.m = &usocket::isendmsg ;
	    rs = so(fd) ;
	}
	return rs ;
}
/* end subroutine (u_sendmsg) */

int u_sendto(int fd,cvoid *wbuf,int wlen,int flags,cvoid *sap,int sal) noex {
	int		rs = SR_FAULT ;
	if (wbuf && sap) {
	    usocket	so(wbuf,wlen,flags,sap,sal) ;
	    so.m = &usocket::isendto ;
	    rs = so(fd) ;
	}
	return rs ;
}
/* end subroutine (u_sendto) */

int u_sendfiler(int s,int fd,off_t fo,size_t c) noex {
	int		rs = SR_FAULT ;
	if (fo >= 0) {
	    usender	so(s,fo,c) ;
	    so.m = &usender::isendfile ;
	    rs = so(fd) ;
	}
	return rs ;
}
/* end subroutine (u_sendfile) */

int u_recv(int fd,void *rbuf,int rlen,int flags) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    usocket	so(rbuf,rlen,flags) ;
	    so.m = &usocket::irecv ;
	    rs = so(fd) ;
	}
	return rs ;
}
/* end subroutine (u_recv) */

int u_recvmsg(int fd,MSGHDR *msgp,int flags) noex {
	int		rs = SR_FAULT ;
	if (msgp) {
	    usocket	so(msgp,flags) ;
	    so.m = &usocket::irecvmsg ;
	    rs = so(fd) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_recvmsg) */

int u_recvfrom(int fd,void *rbuf,int rlen,int flags,void *vp,int *lenp) noex {
	int		rs = SR_FAULT ;
	if (rbuf && vp && lenp) {
	    usocket	so(rbuf,rlen,flags,vp,lenp) ;
	    so.m = &usocket::irecvfrom ;
	    rs = so(fd) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_recvfrom) */

int u_shutdown(int fd,int dir) noex {
	usocket	so(dir) ;
	so.m = &usocket::ishutdown ;
	return so(fd) ;
}
/* end subroutine (u_shutdown) */


/* local subroutines */

int usocket::ibind(int fd) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    if ((rs = bind(fd,sap,sal)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end method (usocket::bind) */

int usocket::ilisten(int fd) noex {
	int		rs ;
	if ((rs = listen(fd,sal)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (usocket::listen) */

int usocket::isetsockopt(int fd) noex {
	int		rs ;
	socklen_t	slen = socklen_t(len) ;
	if ((rs = setsockopt(fd,level,name,valp,slen)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (usocket::isetsockopt) */

int usocket::igetsockopt(int fd) noex {
	int		rs = SR_FAULT ;
	if (valp && lenp) {
	    void	*vp = cast_const<void *>(valp) ;
	    socklen_t	slen = socklen_t(*lenp) ;
	    if ((rs = getsockopt(fd,level,name,vp,&slen)) < 0) {
	        rs = (- errno) ;
	    } else {
	        *lenp = intsat(slen) ;
		rs = intsat(slen) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end method (usocket::igetsockopt) */

int usocket::igetpeername(int fd) noex {
	SOCKADDR	*fromp = cast_const<SOCKADDR *>(sap) ;
	socklen_t	slen = socklen_t(*lenp) ;
	int		rs ;
	if ((rs = getpeername(fd,fromp,&slen)) < 0) {
	    rs = (- errno) ;
	} else {
	    *lenp = intsat(slen) ;
	    rs = intsat(slen) ;
	}
	return rs ;
}
/* end method (usocket::igetpeername) */

int usocket::igetsockname(int fd) noex {
	SOCKADDR	*fromp = cast_const<SOCKADDR *>(sap) ;
	socklen_t	slen = socklen_t(*lenp) ;
	int		rs ;
	if ((rs = getsockname(fd,fromp,&slen)) < 0) {
	    rs = (- errno) ;
	} else {
	    *lenp = intsat(slen) ;
	    rs = intsat(slen) ;
	}
	return rs ;
}
/* end method (usocket::igetsockname) */

int usocket::isend(int fd) noex {
	int		rs ;
	csize		wsize = size_t(wlen) ;
	if (ssize_t rsize ; (rsize = send(fd,wbuf,wsize,flags)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (usocket::isend) */

int usocket::isendmsg(int fd) noex {
	int		rs ;
	if (ssize_t rsize ; (rsize = sendmsg(fd,msgp,flags)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (usocket::isendmsg) */

int usocket::isendto(int fd) noex {
	int		rs ;
	csize		wsize = size_t(wlen) ;
	csocklen	slen = socklen_t(sal) ;
	ssize_t		rsize ;
	if ((rsize = sendto(fd,wbuf,wsize,flags,sap,slen)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (usocket::isendmsg) */

int usocket::irecv(int fd) noex {
	int		rs ;
	if (ssize_t rsize ; (rsize = recv(fd,rbuf,rlen,flags)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (usocket::irecv) */

int usocket::irecvmsg(int fd) noex {
	int		rs ;
	MSGHDR		*mp = cast_const<MSGHDR *>(msgp) ;
	if (ssize_t rsize ; (rsize = recvmsg(fd,mp,flags)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (usocket::irecvmsg) */

int usocket::irecvfrom(int fd) noex {
	int		rs ;
	csize		rsz = size_t(rlen) ;
	socklen_t	slen = socklen_t(*lenp) ;
	SOCKADDR	*sp = cast_const<SOCKADDR *>(sap) ;
	ssize_t		rsize ;
	if ((rsize = recvfrom(fd,rbuf,rsz,flags,sp,&slen)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	    *lenp = intsat(slen) ;
	}
	return rs ;
}
/* end method (usocket::irecvfrom) */

int usocket::ishutdown(int fd) noex {
	int		rs ;
	if ((rs = shutdown(fd,sal)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (usocket::ishutdown) */

int usender::isendfile(int fd) noex {
	return usendfile(fd,s,fo,c) ;
}
/* end method (usender::isendfile) */


