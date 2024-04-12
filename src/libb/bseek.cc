/* bseek SUPPORT */
/* lang=C++20 */

/* "Basic I/O" package (BFILE) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	Seek in the file.

	Synopsis:
	int bseek(bfile *op,off_t wo,int w) noex

	Arguments:
	- op		file pointer
	- wo		new offset relative to "w"
	- w
			SEEK_SET	0 = from beginning of file
			SEEK_CUR	1 = from current pointer of file
			SEEK_END	2 = from end of file

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	notappend(bfile *op,off_t wo,int w) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int bseek(bfile *op,off_t wo,int w) noex {
	int		rs ;
	if ((rs = bfile_magic(op)) > 0) {
	    rs = SR_NOTSEEK ;
	    if (! op->f.notseek) {
		if ((rs = notappend(op,wo,w)) > 0) {
		    off_t	ao = 0 ;
	            if (op->f.write) {
	                if (op->len > 0) {
	                    rs = bfile_flush(op) ;
	                }
	            } else {
	                if (w == SEEK_CUR) {
	                    ao = (- op->len) ;
	                }
	            } /* end if */
	            if (rs >= 0) {
		        off_t	soff{} ;
		        off_t	co ;
	                op->bp = op->bdata ;
	                op->len = 0 ;
	                co = (wo + ao) ;
	                rs = u_seeko(op->fd,co,w,&soff) ;
	                op->offset = soff ;
	            } /* end if */
		} /* end if (not-appending) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (bseek) */


/* local subroutines */

static int notappend(bfile *op,off_t wo,int w) noex {
	int		rs = 1 ;
	if (! (op->oflags & O_APPEND)) {
	    coff	foff = off_t(op->offset) ;
	    if (((w == SEEK_CUR) && (wo == 0)) || 
	        ((w == SEEK_SET) && (wo == foff))) {
	        rs = 0 ;
	    }
	}
	return rs ;
}
/* end subroutine (notappend) */


