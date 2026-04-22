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
	usocket (module)
	uc_bind
	uc_listen
	uc_setsockopt
	uc_getsockopt
	uc_getsockname
	uc_getpeername
	uc_send
	uc_sendmsg
	uc_sendto
	uc_sendfiler
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

#include	<envstandards.h>
#include	<sys/socket.h>
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibsubs.h>
#include	<localmisc.h>

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
    struct usocket ;
    struct usender ;
    typedef int (usocket::*usocket_m)(int) noex ;
    typedef int (usender::*usender_m)(int) noex ;
    struct usocket : ucdescbase {
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
} /* end namespace */


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

sysret_t usocket::ibind(int fd) noex {
	int		rs = SR_FAULT ;
	if (sap) {
	    rs = u_bind(fd,sap,sal) ;
	} /* end if (non-null) */
	return rs ;
} /* end method (usocket::bind) */

sysret_t usocket::ilisten(int fd) noex {
	return listen(fd,sal) ;
} /* end method (usocket::listen) */

sysret_t usocket::isetsockopt(int fd) noex {
	return u_setsockopt(fd,level,name,valp,len) ;
} /* end method (usocket::isetsockopt) */

sysret_t usocket::igetsockopt(int fd) noex {
	int		rs = SR_FAULT ;
	if (valp && lenp) {
	    rs = getsockopt(fd,level,name,valp,lenp) ;
	} /* end if (non-null) */
	return rs ;
} /* end method (usocket::igetsockopt) */

sysret_t usocket::igetpeername(int fd) noex {
	return u_getpeername(fd,fromp,lenp) ;
} /* end method (usocket::igetpeername) */

sysret_t usocket::igetsockname(int fd) noex {
	return u_getsockname(fd,fromp,lenp) ;
} /* end method (usocket::igetsockname) */

sysret_t usocket::isend(int fd) noex {
	return u_send(fd,wbuf,wsize,flags) ;
} /* end method (usocket::isend) */

sysret_t usocket::isendmsg(int fd) noex {
	return u_sendmsg(fd,msgp,flags) ;
} /* end method (usocket::isendmsg) */

sysret_t usocket::isendto(int fd) noex {
	return u_sendto(fd,wbuf,wsize,flags,sap,slen) ;
} /* end method (usocket::isendmsg) */

sysret_t usocket::irecv(int fd) noex {
	return u_recv(fd,rbuf,rlen,flags) ;
} /* end method (usocket::irecv) */

sysret_t usocket::irecvmsg(int fd) noex {
	return u_recvmsg(fd,mp,flags) ;
} /* end method (usocket::irecvmsg) */

sysret_t usocket::irecvfrom(int fd) noex {
	return u_recvfrom(fd,rbuf,rsz,flags,sp,&slen) ;
} /* end method (usocket::irecvfrom) */

sysret_t usocket::ishutdown(int fd) noex {
	return u_shutdown(fd,sal)
} /* end method (usocket::ishutdown) */

sysret_t usender::isendfile(int fd) noex {
	return u_sendfile(fd,s,fo,c) ;
} /* end method (usender::isendfile) */


