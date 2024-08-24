/* freadln SUPPORT */
/* lang=C++20 */

/* read a coded line from the STDIO stream */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-17, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This routine will only read at most 'len' number of bytes
	from the file.  Note that the sematics of this call are not
	the same as |fgets(3c)|.  This call will write a nullptrCHAR
	into the user buffer after the supplied length of the buffer
	is used up.  With |fgets(3c)|, it will never write more
	than the supplied length of bytes.

	Notes:
	The IRIX operating system is messed up somehow.  An attempt
	to correct for this is below.

******************************************************************************/

#include	<envstandards.h>
#include	<cstdio>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"stdsio.h"


/* local defines */


/* exported subroutines */

int freadln(FILE *fp,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (fp && rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 1) {
	        char	*bp = fgets(rbuf,(rlen + 1),fp) ;
		rs = SR_OK ;
	        i = (bp != nullptr) ? strlen(bp) : 0 ;
	        if ((i == 0) && ferror(fp)) {
	           clearerr(fp) ;
	           rs = SR_IO ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (freadln) */

int fgetline(FILE *fp,char *rbuf,int rlen) noex {
	return freadln(fp,rbuf,rlen) ;
}
/* end subroutine (fgetline) */


