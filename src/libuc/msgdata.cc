/* msgdata SUPPORT */
/* lang=C++20 */

/* library initialization for KSH built-in command libraries */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Message support.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<usystem.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"msgdata.h"


/* local defines */

#ifndef	CMSGBUFLEN
#define	CMSGBUFLEN	(2*256)
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	msgdata_setrecv(msgdata *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int msgdata_init(msgdata *mip,int mlen) noex {
	cint		clen = MAX(CMSGBUFLEN,sizeof(CONMSGHDR)) ;
	int		rs ;
	int		size = 0 ;
	int		ml = 0 ;
	char		*bp ;

	memset(mip,0,sizeof(MSGDATA)) ;
	mip->ns = -1 ;

	if (mlen < MSGBUFLEN) mlen = MSGBUFLEN ;

	size += (clen+1) ;
	size += (mlen+1) ;
	if ((rs = uc_libmalloc(size,&bp)) >= 0) {
	    MSGHDR	*mp = &mip->msg ;
	    mip->a = bp ;
	    ml = mlen ;

	    mip->mbuf = bp ;
	    mip->mlen = mlen ;
	    bp += (mlen+1) ;
	    mip->cmsgp = (CONMSGHDR *) bp ;
	    mip->clen = clen ;
	    bp += (clen+1) ;

	    mip->mbuf[0] = '\0' ;
	    mip->vecs[0].iov_base = mip->mbuf ;
	    mip->vecs[0].iov_len = mip->mlen ;

	    memset(mip->cmsgp,0,clen) ; /* clear control-message */

	    memset(mp,0,sizeof(MSGHDR)) ;
	    mp->msg_name = &mip->from ;
	    mp->msg_namelen = sizeof(SOCKADDRESS) ;
	    mp->msg_control = mip->cmsgp ;
	    mp->msg_controllen = 0 ;
	    mp->msg_iov = mip->vecs ;
	    mp->msg_iovlen = 1 ;

	} /* end if (m-a) */

	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (msgdata_init) */

int msgdata_fini(msgdata *mip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (mip == nullptr) return SR_FAULT ;
	if (mip->a != nullptr) {
	    rs1 = uc_libfree(mip->a) ;
	    if (rs >= 0) rs = rs1 ;
	    mip->a = nullptr ;
	    mip->mbuf = nullptr ;
	    mip->mlen = 0 ;
	    mip->ml = 0 ;
	    mip->cmsgp = nullptr ;
	    mip->clen = 0 ;
	}
	return rs ;
}
/* end subroutine (msgdata_fini) */

int msgdata_getbufsize(msgdata *mip) noex {
	return mip->mlen ;
}
/* end subroutine (msgdata_bufsize) */

int msgdata_getbuf(msgdata *mip,char **rpp) noex {
	if (rpp) {
	    *rpp = mip->mbuf ;
	}
	return mip->mlen ;
}
/* end subroutine (msgdata_get) */

int msgdata_getdatalen(msgdata *mip) noex {
	return mip->ml ;
}
/* end subroutine (msgdata_getdatalen) */

int msgdata_setdatalen(msgdata *mip,int dlen) noex {
	mip->ml = dlen ;
	return dlen ;
}
/* end subroutine (msgdata_setdatalen) */

int msgdata_getdata(msgdata *mip,char **rpp) noex {
	if (rpp) {
	    *rpp = mip->mbuf ;
	}
	return mip->ml ;
}
/* end subroutine (msgdata_getdata) */

int msgdata_recvto(msgdata *mip,int fd,int to) noex {
	MSGHDR		*mp = &mip->msg ;
	int		rs ;
	msgdata_setrecv(mip) ;
	if ((rs = uc_recvmsge(fd,mp,0,to,0)) >= 0) {
	    mip->ml = rs ;
	}
	return rs ;
}
/* end subroutine (msgdata_recvto) */

int msgdata_recv(msgdata *mip,int fd) noex {
	MSGHDR		*mp = &mip->msg ;
	int		rs ;
	msgdata_setrecv(mip) ;
	if ((rs = u_recvmsg(fd,mp,0)) >= 0) {
	    mip->ml = rs ;
	}
	return rs ;
}
/* end subroutine (msgdata_recv) */

int msgdata_send(msgdata *mip,int fd,int dl,int cl) noex {
	int		rs ;
	if (cl <= mip->clen) {
	    MSGHDR	*mp = &mip->msg ;
	    mip->vecs[0].iov_len = (dl >= 0) ? (mip->ml = dl) : mip->ml ;
	    mp->msg_controllen = cl ;
	    rs = u_sendmsg(fd,mp,0) ;
	} else {
	    rs = SR_TOOBIG ;
	}
	return rs ;
}
/* end subroutine (msgdata_send) */

/* receive or reject a passed FD (f=1 -> receive, f=0 -> reject) */
int msgdata_conpass(msgdata *mip,int f_passfd) noex {
	MSGHDR		*mp = &mip->msg ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	mip->ns = -1 ;
	if (mp->msg_controllen > 0) {
	    CONMSGHDR	*cmp = CMSG_FIRSTHDR(mp) ;
	    int		fd ;
	    while (cmp != nullptr) {
		if ((fd = cmsghdr_passed(cmp)) >= 0) {
	            if ((mip->ns < 0) && f_passfd) {
	                mip->ns = fd ;
			f = true ;
	            } else {
	                rs1 = u_close(fd) ;
			if (rs >= 0) rs = rs1 ;
		    }
	        } /* end if (cmsghdr_pass) */
	        cmp = CMSG_NXTHDR(mp,cmp) ;
	    } /* end while */
	} /* end if (had a control-part) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (msgdata_conpass) */

int msgdata_getpassfd(msgdata *mip) noex {
	int		rs = mip->ns ;
	if (rs < 0) rs = SR_NOTOPEN ;
	return rs ;
}
/* end subroutine (msgdata_getpassfd) */

int msgdata_setaddr(msgdata *mip,cvoid *sap,int sal) noex {
	cint		flen = sizeof(SOCKADDRESS) ;
	int		rs ;
	if (sal <= flen) {
	    memcpy(&mip->from,sap,sal) ;
	    mip->msg.msg_namelen = sal ;
	} else {
	    rs = SR_TOOBIG ;
	}
	return rs ;
}
/* end subroutine (msgdata_setaddr) */

int msgdata_rmeol(msgdata *mip) noex {
	while (mip->ml > 0) {
	    cint	ch = MKCHAR(mip->mbuf[mip->ml-1]) ;
 	    if (! iseol(ch)) break ;
	    mip->ml -= 1 ;
	}
	return mip->ml ;
}
/* end subroutine (msgdata_rmeol) */


/* private subroutines */

static int msgdata_setrecv(msgdata *mip) noex {
	MSGHDR		*mp = &mip->msg ;
	mip->mbuf[0] = '\0' ;
	mip->vecs[0].iov_base = mip->mbuf ;
	mip->vecs[0].iov_len = mip->mlen ;
	mip->ml = 0 ;
	mp->msg_controllen = mip->clen ;
	return SR_OK ;
}
/* end subroutine (msgdata_setrecv) */



