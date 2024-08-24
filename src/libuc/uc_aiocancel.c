/* uc_aiocancel */

/* interface component for UNIX� library-3c */
/* asynchronous I/O cancel operation */


#define	CF_DEBUGS	0		/* non-switchable debug printo-outs */


/* revision history:

	= 1998-05-22, David A�D� Morano
	This was first written to give a little bit to UNIX� what
	we have in our own circuit pack OSes!

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

        Note: All platforms we care about now have POSIX AIO. So we don't have
        to worry about compile-time switches to dummy suboutines.

	Asynchronously cancel an outstanding I/O operation.

	Synopsis:

	int uc_aiocancel(fd,aiobp)
	int		fd ;
	struct aiocb	*aiobp ;

	Arguments:

	fd		file descriptor
	aiobp		AIO block pointer

	Returns:

	>=0		amount of data returned
	<0		error


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<unistd.h>
#include	<poll.h>
#include	<aio.h>
#include	<errno.h>

#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_AGAIN	10


/* external subroutines */


/* forward references */


/* exported subroutines */


int uc_aiocancel(int fd,struct aiocb *aiobp)
{
	int		rs ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	if (aiobp == NULL) return SR_FAULT ;

	repeat {
	    errno = 0 ;
	    if ((rs = aio_cancel(fd,aiobp)) == -1) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
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
/* end subroutine (uc_aiocancel) */


