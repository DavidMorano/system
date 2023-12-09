/* bdup */
/* lang=C++20 */

/* "Basic I/O" package */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

	= 1999-01-10, David A­D­ Morano
	I added the little extra code to allow for memory mapped
	I/O. It is all a waste because it is way slower than without
	it! This should teach me to leave old programs alone!!!

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Duplicate a BFILE stream.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<string.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int bdup(bfile *fp,bfile *fnewp) noex {
	int		rs = SR_FAULT ;
	if (fp && fnewp) {
	    rs = SR_NOTOPEN ;
	    memcpy(fnewp,fp,sizeof(bfile)) ;
	    if (fp->magic == BFILE_MAGIC) {
	        if ((rs = bfile_flush(fp)) >= 0) {
	            if ((rs = u_dup(fp->fd)) >= 0) {
	                fnewp->fd = rs ;
	                if (fp->bsize > 0) {
		            void	*vp = nullptr ;
	                    if ((rs = uc_malloc(fp->bsize,&vp)) >= 0) {
	                        fnewp->bdata = (char *) vp ;
	                        fnewp->bbp = (char *) vp ;
	                        fnewp->bp = (char *) vp ;
	                    }
	                }
		        if (rs < 0) {
		            uc_close(fnewp->fd) ;
		            fnewp->fd = -1 ;
		        }
	            } /* end if (u_dup) */
	            if (rs < 0) {
		        fnewp->magic = 0 ;
	            }
	        } /* end if (bfile_flush) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bdup) */


