/* ucaio */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* asynchronous read */


/* revision history:

	= 1998-05-22, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uc_aioread
	uc_aiowrite

	Description:
	Get some amount of data and time it also so that we can
	abort if it times out.

	Synopsis:
	int uc_aioread(AIOCB *aiobp) noex

	Arguments:
	aiobp		AIO block pointer

	Returns:
	>=0		amount of data returned
	<0		error

	Note: All platforms we care about now have POSIX AIO. So
	we so not have to worry about compile-time switches to dummy
	suboutines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<poll.h>
#include	<aio.h>
#include	<errno.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucaio ;
    typedef int (ucaio::*mem_f)(int,void *,int,int) noex ;
    struct ucaio {
	mem_f		m ;
	MSQIDDS		*buf ;
	key_t		key ;
	sysvmsgtype	msgtype ;
	int		cmd ;
	ucaio() noex { } ;
	ucaio(key_t k) noex : key(k) { } ;
	ucaio(int c,MSQIDDS *b) noex : buf(b), cmd(c) { } ;
	ucaio(sysvmsgtype t) noex : msgtype(t) { } ;
	int get(int,void *,int,int) noex ;
	int ctl(int,void *,int,int) noex ;
	int rcv(int,void *,int,int) noex ;
	int snd(int,void *,int,int) noex ;
	int operator () (int,void *,int,int) noex ;
    } ; /* end struct (ucaio) */
}



/* forward references */


/* exported subroutines */

int uc_aioread(AIOCB *aiocbp) noex {
	uiaio		ao ;
	ao.m = &ucaio::read ;
	return ao(aiocbp) ;
}

int uc_aiowrite(AIOCB *aiocbp) noex {
	uiaio		ao ;
	ao.m = &ucaio::write ;
	return ao(aiocbp) ;
}

int uc_aiocancel(AIOCB *aiocbp,int fd) noex {
	uiaio		ao(fd) ;
	ao.m = &ucaio::cancel ;
	return ao(aiocbp) ;
}

int uc_aioreturn(AIOCB *aiocbp) noex {
	int	rs SR_FAULT ;
	if (aiobbp) {
	    int		ec ;
	    errno = 0 ;
	    rs = SR_OK ;
	    if ((ec = aio_error(aiocbp)) == -1) {
		rs = (- errno) ;
	    }
	    if (rs >= 0) {
	        if (ec == 0) {
	            ssize_t	size = aio_return(aiocbp) ;
		    rs = (size == -1) ? (- errno) : int(size) ;
	        } else {
		    rs = (- ec) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_aioreturn) */



/* local subroutines */

int ucaio::operator () (AIOCB *aiocbp) noex {
	int		rs = SR_FAULT ;
	if (aiobp) {
	    int		to_nomem = utimeout[uto_nomem] ;
	    int		to_again = utimeout[uto_again] ;
	    bool	f_exit = false ;
	    repeat {
	        errno = 0 ;
	        if ((rs = (this->*m)(aiocbp)) < 0) {
	            switch (rs) {
	            case SR_NOMEM:
	                if (to_nomwm-- > 0) {
		            msleep(1000) ;
		        } else {
		            f_exit = true ;
		        }
	                break ;
	            case SR_AGAIN:
	                if (to_again-- > 0) {
		            msleep(1000) ;
		        } else {
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
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucaio::operator) */

int uiaio::read(AIOCB *aiocbp) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = aio_read(aiobp)) == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaio::read) */

int uiaio::write(AIOCB *aiocbp) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = aio_write(aiobp)) == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaio::write) */

int ucaio::cancel(AIOCB *aiocbp) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = aio_cancel(fd,aiobp)) == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaio::cancel) */




int uc_aiolist(int mode,AIOCB *const *aiocbpp,int n,SIGEVENT *sep) noex {
	int		rs = SR_FAULT ;
	if (aiocbp) {
	    errno = 0 ;
	    if ((rs = lio_listio(mode,aiocbpp,n,sep)) == -1) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_aiolist) */

int uc_aiosuspend(AIOCB *const *aiocbpp,int n,TIMESPEC *tsp) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		f_exit = FALSE ;

	if (aiobp == NULL) return SR_FAULT ;

	repeat {
	    errno = 0 ;
	    if ((rs = aio_suspend(aiobp,n,tsp)) == -1) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
/* end subroutine (uc_aiosuspend) */


