/* ucaio SUPPORT */
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
	uc_aiocancel
	uc_aiofsync
	uc_aioreturn
	uc_aiolist
	uc_suspend

	Description:
	Get some amount of data and time it also so that we can
	abort if it times out.

	Synopsis:
	int uc_aioread(AIOCB *aiocbp) noex
	int uc_aiowrite(AIOCB *aiocbp) noex
	int uc_aiocancel(AIOCB *aiocbp,int fd) noex
	int uc_aiofsync(AIUCB *aiocbp,int x) noex
	int uc_aioreturn(AIOCB *aiocbp) noex
	int uc_aiolist(int mode,AIOCB *const *aiocbpp,int n,SIGEVENT *sep) noex
	int uc_aiosuspend(CAIOCB *const *aiocbpp,int n,CTIMESPEC *tsp) noex

	Arguments:
	aiocbp		AIO block pointer
	fd		file-descriptor

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
#include	<aio.h>
#include	<cerrno>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucaio ;
    typedef int (ucaio::*mem_f)(AIOCB *) noex ;
    struct ucaio {
	mem_f		m ;
	CTIMESPEC	*tsp ;
	int		fd ;
	int		ne ;
	ucaio() noex { } ;
	ucaio(int f) noex : fd(f) { } ;
	ucaio(int n,CTIMESPEC *t) noex : ne(n), tsp(t) { } ;
	int read(AIOCB *) noex ;
	int write(AIOCB *) noex ;
	int cancel(AIOCB *) noex ;
	int fsync(AIOCB *) noex ;
	int suspend(AIOCB *) noex ;
	int operator () (AIOCB *) noex ;
    } ; /* end struct (ucaio) */
}


/* forward references */


/* exported subroutines */

int uc_aioread(AIOCB *aiocbp) noex {
	ucaio		ao ;
	ao.m = &ucaio::read ;
	return ao(aiocbp) ;
}

int uc_aiowrite(AIOCB *aiocbp) noex {
	ucaio		ao ;
	ao.m = &ucaio::write ;
	return ao(aiocbp) ;
}

int uc_aiocancel(AIOCB *aiocbp,int fd) noex {
	ucaio		ao(fd) ;
	ao.m = &ucaio::cancel ;
	return ao(aiocbp) ;
}

int uc_aiofsync(AIOCB *aiocbp,int cmd) noex {
	ucaio		ao(cmd) ;
	ao.m = &ucaio::fsync ;
	return ao(aiocbp) ;
}

int uc_aioreturn(AIOCB *aiocbp) noex {
	int	rs SR_FAULT ;
	if (aiocbp) {
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

int uc_aiolist(int mode,AIOCB *const *aiocbpp,int n,SIGEVENT *sep) noex {
	int		rs = SR_FAULT ;
	if (aiocbpp) {
	    errno = 0 ;
	    if ((rs = lio_listio(mode,aiocbpp,n,sep)) == -1) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_aiolist) */

int uc_aiosuspend(const AIOCB *const *aiocbpp,int n,CTIMESPEC *tsp) noex {
	AIOCB		*aiocbp = (AIOCB *) aiocbpp ;
	int		rs = SR_FAULT ;
	if (aiocbpp && tsp) {
	    ucaio	ao(n,tsp) ;
	    ao.m = &ucaio::suspend;
	    rs = ao(aiocbp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_aiosuspend) */


/* local subroutines */

int ucaio::operator () (AIOCB *aiocbp) noex {
	int		rs = SR_FAULT ;
	if (aiocbp) {
	    int		to_nomem = utimeout[uto_nomem] ;
	    int		to_again = utimeout[uto_again] ;
	    bool	f_exit = false ;
	    repeat {
	        errno = 0 ;
	        if ((rs = (this->*m)(aiocbp)) < 0) {
	            switch (rs) {
	            case SR_NOMEM:
	                if (to_nomem-- > 0) {
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

int ucaio::read(AIOCB *aiocbp) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = aio_read(aiocbp)) == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaio::read) */

int ucaio::write(AIOCB *aiocbp) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = aio_write(aiocbp)) == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaio::write) */

int ucaio::cancel(AIOCB *aiocbp) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = aio_cancel(fd,aiocbp)) == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaio::cancel) */

int ucaio::fsync(AIOCB *aiocbp) noex {
	int		rs ;
	errno = 0 ;
	if ((rs = aio_fsync(fd,aiocbp)) == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ucaio::fsync) */

int ucaio::suspend(AIOCB *aiocbp) noex {
	const AIOCB *const *aiocbpp = (AIOCB *const *) aiocbp ;
	int		rs ;
	errno = 0 ;
	if ((rs = aio_suspend(aiocbpp,ne,tsp)) == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (ucaio::suspend) */


