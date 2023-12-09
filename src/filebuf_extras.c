/* filebuf_extras */
/* lang=C20 */

/* extra methods for the FILEBUF object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	We provide some extra small function for special circumstances.

	Synopsis:
	int filebuf_writefill(filebuf *bp,cchar *sp,int sl) noex

	Arguments:
	bp		FILEBUF object pointer
	sp		source buffer
	sl		source buffer length

	Returns:
	<0		error
	>=0		number of bytes written


*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<filebuf.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern int	filebuf_writealign(FILEBUF *,int) ;


/* external variables */


/* exported variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


int filebuf_writefill(FILEBUF *bp,const char *sp,int sl)
{
	const int	asize = sizeof(int) ;
	int		rs ;
	int		len = 0 ;

	if (sl < 0) sl = (strlen(sp) + 1) ;

	if ((rs = filebuf_write(bp,sp,sl)) >= 0) {
	    len = rs ;
	    rs = filebuf_writealign(bp,asize) ;
	    len += rs ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (filebuf_writefill) */


