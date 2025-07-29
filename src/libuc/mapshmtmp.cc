/* mapshmtmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* map some POSIX® SHM-type shared memory */
/* version %I% last-modified %G% */


/* revision history:

	= 2006-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2006 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	mapshmtmp

	Description:
	This subroutine creates and maps a POSIX® shared-memory
	temporary object. The POSIX® SHM-type name of the object
	created is returned to the caller.

	Synopsis:

	int mapshmtmp(char *rbuf,int rlen,mode_t operm,int shmlen,char *rpp)

	Arguments:
	rbuf		suppled buffer to receive the resulting SHM name
	rlen		length of suppled buffer; should be at least SHMNAMELEN
	operm		SHM object creation opermissions
	shmlen		desired length of resulting SHM object
	rpp		pointer to hold address of resulting SHM object

	Returns:
	>=0		file descriptor to program STDIN and STDOUT
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sysval.hh>
#include	<mallocxx.h>
#include	<localmisc.h>

#include	"mapshmtmp.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	openshmtmp(char *,int,int) noex ;
}


/* external variables */


/* local structures */


/* forward reference */

static int	shmalloc(int,int) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int mapshmtmp(char *rbuf,int rlen,mode_t operm,int shmlen,char **rpp) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (rpp) ylikely {
	    rs = SR_INVALID ;
	    if (shmlen > 0) ylikely {
		bool	f_bufalloc = false ;
		rs = SR_OK ;
                if (rbuf == nullptr) {
                    if ((rs = malloc_mn(&rbuf)) >= 0) {
                        rlen = rs ;
                        f_bufalloc = true ;
                    }
                } /* end if (not user-supplied buffer) */
                if (rs >= 0) ylikely {
                    if ((rs = openshmtmp(rbuf,rlen,operm)) >= 0) ylikely {
                        cint	fd = rs ;
                        if (f_bufalloc) {
                            uc_unlinkshm(rbuf) ;
                        }
                        if ((rs = shmalloc(fd,shmlen)) >= 0) ylikely {
                            csize	ms = shmlen ;
                            cint	mp = (PROT_READ | PROT_WRITE) ;
                            cint	mo = MAP_SHARED ;
                            rs = u_mmapbegin(np,ms,mp,mo,fd,0z,rpp) ;
                        } /* end if */
                        uc_close(fd) ;
                    } /* end if (openshmtmp) */
                    if (f_bufalloc && rbuf) {
                        uc_free(rbuf) ;
                    }
                } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapshmtmp) */


/* local subroutines */

static int shmalloc(int fd,int shmlen) noex {
	int		rs ;
	if ((rs = pagesize) >= 0) ylikely {
	    cint	ps = rs ;
	    off_t	off = 0 ;
	    cint	wlen = szof(int) ;
	    char	wbuf[szof(int) + 1] = {} ;
	    while ((rs >= 0) && (off < shmlen)) {
	        rs = u_pwrite(fd,wbuf,wlen,off) ;
	        off += ps ;
	    } /* end while */
	} /* end if (pagesize) */
	return rs ;
}
/* end subroutine (shmalloc) */


