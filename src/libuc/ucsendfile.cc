/* ucsendfile */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* UNIX® write system call subroutine */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-02-21, David A­D­ Morano
	Originally written when it became available.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® for |sendfile(3net)| */
#include	<sys/socket.h>		/* POSIX® for |sendfile(3net)| */
#include	<sys/uio.h>		/* POSIX® for |sendfile(3net)| */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_sendfile(int ofd,int ifd,off_t *offp,int wlen) noex {
	off_t		fo = 0z ;
	int		rs = SR_OK ;
	int		len = 0 ;

	if (offp == nullptr) return SR_FAULT ;

#ifdef	COMMENT
	repeat {
	    ssize_t	ss ;
	    fo = (off_t) (*offp & INT_MAX) ;
	    ss = sendfile(ofd,ifd,&fo,wlen) ;
	    if (ss == -1) rs = (neg errno) ;
	    len = (ss & INT_MAX) ;
	} until (rs != SR_INTR) ;
#else /* COMMENT */
	(void) ifd ;
	(void) ofd ;
	(void) wlen ;
	rs = SR_NOSYS ;
#endif /* COMENT */

	*offp = (off_t) fo ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (uc_sendfile) */

int uc_sendbuf(int ofd,void *wbuf,int wlen) noex {
	int		rs = SR_OK ;
	int		len = 0 ;

	if (wbuf == nullptr) return SR_FAULT ;

#ifdef	COMMENT
	repeat {
	    ssize_t	ss ;
	    ss = sendfile(ofd,SFV_FD_SELF,wbuf,wlen) ;
	    if (ss == -1) rs = (neg errno) ;
	    len = (ss & INT_MAX) ;
	} until (rs != SR_INTR) ;
#else /* COMMENT */
	(void) ofd ;
	(void) wlen ;
	rs = SR_NOSYS ;
#endif /* COMENT */

	return (rs >= 0) ? len : rs ;
} /* end subroutine (uc_sendbuf) */


