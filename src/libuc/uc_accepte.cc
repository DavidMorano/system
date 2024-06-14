/* uc_accepte SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* extended read */


/* revision history:

	= 1998-03-26, David A­D­ Morano
        This was first written to give a little bit to UNIX® what we have in our
        own circuit pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:

	Description:
        Accept a connection on a socket and time it also so that we can abort if
        it times out.

	Synopsis:
	int uc_accepte(int fd,SOCKADDR *sap,int *salp,int to) noex

	Arguments:
	fd		file descriptor
	sap		address to buffer to receive the "from" address
	salp		pointer to the length of the "from" buffer
	to		time in seconds to wait

	Returns:
	>=0		socket of new connection
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<usystem.h>
#include	<bufprintf.h>
#include	<localmisc.h>


/* local defines */

#define	POLL_INTMULT	1000		/* poll-time multiplier */
#define	EBUFLEN		100


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_accepte(int fd,SOCKADDR *sap,int *salp,int to) noex {
	int		rs = SR_FAULT ;
	int		s = -1 ;
	if (sap && salp) {
	    rs = SR_BADFD ;
	    if (fd >= 0) {
	        if (to < 0) to = INT_MAX ;
	        if (to >= 0) {
	            POLLFD	fds[1] = {} ;
	            fds[0].fd = fd ;
	            fds[0].events = POLLIN ;
	            while (rs >= 0) {
	                if ((rs = u_poll(fds,1,POLL_INTMULT)) >= 0) {
	                    if (rs > 0) {
	                        cint	re = fds[0].revents ;
	                        if (re & POLLIN) {
	                            rs = u_accept(fd,sap,salp) ;
	                            s = rs ;
	                        } else if (re & POLLHUP) {
	                            rs = SR_HANGUP ;
	                        } else if (re & POLLERR) {
	                            rs = SR_POLLERR ;
	                        } else if (re & POLLNVAL) {
	                            rs = SR_NOTOPEN ;
	                        }
	                    } else {
	                        to -= 1 ;
        		    }
	                    if (to <= 0) break ;
	                    if (s >= 0) break ;
	                } else if (rs == SR_INTR) rs = SR_OK ;
	            } /* end while */
	            if ((rs == 0) && (s < 0) && (to <= 0)) {
	                rs = SR_TIMEDOUT ;
	            }
	        } else {
	            rs = u_accept(fd,sap,salp) ;
	            s = rs ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? s : rs ;
}
/* end subroutine (uc_accepte) */


