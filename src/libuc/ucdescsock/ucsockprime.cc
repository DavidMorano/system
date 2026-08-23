/* ucsockprime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* reuse an existing (perhaps) socket address */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	sockmgr (module)
	uc_bind
	uc_listen
	uc_sockoptset
	uc_sockoptget
	uc_getsockname
	uc_getpeername
	uc_send
	uc_sendmsg
	uc_sendto
	uc_sendfile
	uc_recv
	uc_recvmsg
	uc_recvfrom
	uc_shutdown

	Description:
	This module contains the UNIX® socket system calls (yes,
	they are sustem calls now-a-days).

	Name:
	uc_send

	Description:
	This is the famous (infamous) old |send(2)| system call.
	Of course, it has now been a system call for almost two
	decades (on System V UNIX®) but that is still OK! :-)

	Name:
	uc_shutdown

	Description:
	Shut down one or both directions of a (connected) socket.
	Possible shutdown commands are:
		SHUT_RD
		SHUT_WR
		SHUT_RDWR

	Synopsis:
	int uc_shutdown(int fd,int dir) noex

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

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/socket.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucdescbase.hh"
#include	"ucsockprime.h"

/* local defines */

#define	CSA	CSOCKADDR


/* imported namespaces */

using namespace	usys ;			/* namespace */

using std::nullptr_t ;			/* type */
using libuc::ucdescbase ;		/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local structures */

const nullptr_t		np{} ;		/* needed file-scope */

namespace {
    struct sockmgr ;
    struct usender ;
    typedef int (sockmgr::*sockmgr_m)(int) noex ;
    typedef int (usender::*usender_m)(int) noex ;
    struct sockmgr : ucdescbase {
	sockmgr_m	m = nullptr ;
	CSOCKADDR	*sap ;
	CMSGHDR		*msgp ;
	cvoid		*valp ;
	int		*lenp ;
	cvoid		*wbuf ;
	void		*rbuf ;
	int		sal ;
	int		len ;
	int		level ;
	int		name ;
	int		flags ;
	int		wlen ;
	int		rlen ;
	int		cnt ;
	sockmgr() noex { } ;
	sockmgr(int backlog) noex {
	    sal = backlog ;
	} ;
	sockmgr(cvoid *vp,int l) noex {
	    sap = (CSOCKADDR *) vp ;
	    sal = l ;
	} ;
	sockmgr(int lev,int n,cvoid *vp,int vl) noex : level(lev) {
	    name = n ;
	    valp = vp ;
	    len = vl ;
	} ;
	sockmgr(int lev,int n,cvoid *vp,int *vlp) noex : level(lev) {
	    name = n ;
	    valp = vp ;
	    lenp = vlp ;
	} ;
	sockmgr(cvoid *vp,int *lp) noex : lenp(lp) { 
	    sap = (CSOCKADDR *) vp ;
	} 
	sockmgr(cvoid *wb,int wl,int fl,cvoid *vp = nullptr,int l = 0) noex {
	    sap = (CSA *) vp ;
	    sal = l ;
	    wbuf = wb ;
	    wlen = wl ;
	    flags = fl ;
	} ;
	sockmgr(void *rb,int rl,int fl,void *vp = np,int *lp = np) noex {
	    sap = (CSA *) vp ;
	    lenp = lp ;
	    rbuf = rb ;
	    rlen = rl ;
	    flags = fl ;
	} ;
	sockmgr(CMSGHDR *mp,int fl) noex : flags(fl) {
	    msgp = mp ;
	} ;
	int callstd(int fd) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ; /* end method (callstd) */
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
    } ; /* end struct (sockmgr) */
    struct usender : sockmgr {
	usender_m	m = nullptr ;
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
} /* end namespace */


/* forward references */


/* local variables */


/* external subroutines */


/* exported subroutines */

int uc_bind(int fd,cvoid *sap,int sal) noex {
	sockmgr		so(sap,sal) ;
	so.m = &sockmgr::ibind ;
	return so(fd) ;
} /* end subroutine (uc_bind) */

int uc_listen(int fd,int backlog) noex {
	sockmgr		so(backlog) ;
	so.m = &sockmgr::ilisten ;
	return so(fd) ;
} /* end subroutine (uc_listen) */

int uc_sockoptset(int fd,int level,int optname,cvoid *valp,int len) noex {
	sockmgr		so(level,optname,valp,len) ;
	so.m = &sockmgr::isetsockopt ;
	return so(fd) ;
} /* end subroutine (uc_sockoptset) */

int uc_sockoptget(int fd,int level,int optname,void *valp,int *lenp) noex {
	sockmgr		so(level,optname,valp,lenp) ;
	so.m = &sockmgr::igetsockopt ;
	return so(fd) ;
} /* end subroutine (uc_sockoptget) */

int uc_getpeername(int fd,void *sap,int *lenp) noex {
	int		rs = SR_FAULT ;
	if (sap && lenp) {
	    sockmgr	so(sap,lenp) ;
	    so.m = &sockmgr::igetpeername ;
	    rs = so(fd) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_getpeername) */

int uc_getsockname(int fd,void *sap,int *lenp) noex {
	int		rs = SR_FAULT ;
	if (sap && lenp) {
	    sockmgr	so(sap,lenp) ;
	    so.m = &sockmgr::igetsockname ;
	    rs = so(fd) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_getsockname) */

int uc_send(int fd,cvoid *wbuf,int wlen,int flags) noex {
	int		rs = SR_FAULT ;
	if (wbuf) {
	    sockmgr	so(wbuf,wlen,flags) ;
	    so.m = &sockmgr::isend ;
	    rs = so(fd) ;
	}
	return rs ;
} 
/* end subroutine (uc_send) */

int uc_sendmsg(int fd,CMSGHDR *msgp,int flags) noex {
	int		rs = SR_FAULT ;
	if (msgp) {
	    sockmgr	so(msgp,flags) ;
	    so.m = &sockmgr::isendmsg ;
	    rs = so(fd) ;
	}
	return rs ;
} /* end subroutine (uc_sendmsg) */

int uc_sendto(int fd,cvoid *wbuf,int wlen,int flags,cvoid *sap,int sal) noex {
	int		rs = SR_FAULT ;
	if (wbuf && sap) {
	    sockmgr	so(wbuf,wlen,flags,sap,sal) ;
	    so.m = &sockmgr::isendto ;
	    rs = so(fd) ;
	}
	return rs ;
} /* end subroutine (uc_sendto) */

int uc_sendfile(int s,int fd,off_t fo,size_t c) noex {
	int		rs = SR_FAULT ;
	if (fo >= 0) {
	    usender	so(s,fo,c) ;
	    so.m = &usender::isendfile ;
	    rs = so(fd) ;
	}
	return rs ;
} /* end subroutine (uc_sendfile) */

int uc_recv(int fd,void *rbuf,int rlen,int flags) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    sockmgr	so(rbuf,rlen,flags) ;
	    so.m = &sockmgr::irecv ;
	    rs = so(fd) ;
	}
	return rs ;
} /* end subroutine (uc_recv) */

int uc_recvmsg(int fd,MSGHDR *msgp,int flags) noex {
	int		rs = SR_FAULT ;
	if (msgp) {
	    sockmgr	so(msgp,flags) ;
	    so.m = &sockmgr::irecvmsg ;
	    rs = so(fd) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_recvmsg) */

int uc_recvfrom(int fd,void *rbuf,int rlen,int flags,void *vp,int *lenp) noex {
	int		rs = SR_FAULT ;
	if (rbuf && vp && lenp) {
	    sockmgr	so(rbuf,rlen,flags,vp,lenp) ;
	    so.m = &sockmgr::irecvfrom ;
	    rs = so(fd) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_recvfrom) */

int uc_shutdown(int fd,int dir) noex {
	sockmgr	so(dir) ;
	so.m = &sockmgr::ishutdown ;
	return so(fd) ;
} /* end subroutine (uc_shutdown) */


/* local subroutines */

sysret_t sockmgr::ibind(int fd) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    rs = u_bind(fd,sap,sal) ;
	} /* end if (non-null) */
	return rs ;
} /* end method (sockmgr::bind) */

sysret_t sockmgr::ilisten(int fd) noex {
	return listen(fd,sal) ;
} /* end method (sockmgr::listen) */

sysret_t sockmgr::isetsockopt(int fd) noex {
	return u_setsockopt(fd,level,name,valp,len) ;
} /* end method (sockmgr::isetsockopt) */

sysret_t sockmgr::igetsockopt(int fd) noex {
	int		rs = SR_FAULT ;
	if (valp && lenp) {
	    socklen_t	*slp = (socklen_t *) lenp ;
	    voidp	vp = voidp(valp) ;
	    rs = getsockopt(fd,level,name,vp,slp) ;
	} /* end if (non-null) */
	return rs ;
} /* end method (sockmgr::igetsockopt) */

sysret_t sockmgr::igetpeername(int fd) noex {
    	SOCKADDR *fromp = (SOCKADDR *) sap ;
	return u_getpeername(fd,fromp,lenp) ;
} /* end method (sockmgr::igetpeername) */

sysret_t sockmgr::igetsockname(int fd) noex {
    	SOCKADDR *fromp = (SOCKADDR *) sap ;
	return u_getsockname(fd,fromp,lenp) ;
} /* end method (sockmgr::igetsockname) */

sysret_t sockmgr::isend(int fd) noex {
	return u_send(fd,wbuf,wlen,flags) ;
} /* end method (sockmgr::isend) */

sysret_t sockmgr::isendmsg(int fd) noex {
	return u_sendmsg(fd,msgp,flags) ;
} /* end method (sockmgr::isendmsg) */

sysret_t sockmgr::isendto(int fd) noex {
	return u_sendto(fd,wbuf,wlen,flags,sap,sal) ;
} /* end method (sockmgr::isendmsg) */

sysret_t sockmgr::irecv(int fd) noex {
	return u_recv(fd,rbuf,rlen,flags) ;
} /* end method (sockmgr::irecv) */

sysret_t sockmgr::irecvmsg(int fd) noex {
	MSGHDR *mp = (MSGHDR *) msgp ;
	return u_recvmsg(fd,mp,flags) ;
} /* end method (sockmgr::irecvmsg) */

sysret_t sockmgr::irecvfrom(int fd) noex {
    	SOCKADDR *fromp = (SOCKADDR *) sap ;
	return u_recvfrom(fd,rbuf,rlen,flags,fromp,lenp) ;
} /* end method (sockmgr::irecvfrom) */

sysret_t sockmgr::ishutdown(int fd) noex {
	return u_shutdown(fd,sal) ;
} /* end method (sockmgr::ishutdown) */

sysret_t usender::isendfile(int fd) noex {
	return u_sendfile(fd,s,fo,c) ;
} /* end method (usender::isendfile) */


