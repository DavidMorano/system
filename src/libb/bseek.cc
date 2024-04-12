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
	int bseek(bfile *fp,off_t wo,int whence) noex

	Arguments:
	- fp		file pointer
	- wo		new offset relative to "whence"
	- whence
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


/* external subroutines */


/* external variables */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int bseek(bfile *fp,off_t wo,int whence) noex {
	off_t	co ;
	off_t	soff, ao ;
	int		rs = SR_OK ;

	if (fp == NULL) return SR_FAULT ;

	if (fp->magic != BFILE_MAGIC) return SR_NOTOPEN ;

	if (fp->f.nullfile) goto ret0 ;

/* continue */

	if (fp->f.notseek)
	    return SR_NOTSEEK ;

#if	CF_DEBUGS
	{
	    int	i ;
	    for (i = 0 ; t[i].name != NULL ; i += 1) {
	        if (whence == t[i].w)
	            break ;
	    }
	    debugprintf("bseek: offset=%lu whence=%s(%u)\n",
	        wo,
	        ((t[i].name == NULL) ? "unk" : t[i].name),
	        whence) ;
	}
#endif /* CF_DEBUGS */

/* check for an easy way out */

	if (! (fp->oflags & O_APPEND)) {

	    if (((whence == SEEK_CUR) && (wo == 0)) || 
	        ((whence == SEEK_SET) && (wo == fp->offset)))
	        goto done ;

	}

/* we have to do some real work! */

	ao = 0 ;
	if (fp->f.write) {
	    if (fp->len > 0) {
	        rs = bfile_flush(fp) ;
	    }
	} else {
	    if (whence == SEEK_CUR) {
	        ao = (- fp->len) ;
	    }
	} /* end if */

	if (rs >= 0) {

	    fp->bp = fp->bdata ;
	    fp->len = 0 ;

	    co = (wo + ao) ;
	    rs = u_seeko(fp->fd,co,whence,&soff) ;
	    fp->offset = soff ;

	} /* end if */

done:
ret0:
	return rs ;
}
/* end subroutine (bseek) */


