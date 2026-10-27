/* freader SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read a coded line from the STDIO stream */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-17, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	freader

	Description:
	This routine will only read at most 'len' number of bytes
	from the file.  Note that the semantics of this call are not
	the same as |fgets(3c)|.  This call will write a NUL character
	into the user buffer after the supplied length of the buffer
	is used up.  With |fgets(3c)|, it will never write more
	than the supplied length of bytes.

	Synopsis:
	int freader(FILE *fp,void *rbuf,int rlen) noex

	Arguments:
	fp		pointer to FILE object
	rbuf		result buffer pointer
	rlen		resuly buffer length

	Returns:
	>=0		length of data (in bytes) transferred
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<cerrno>		/* STD-C */
#include	<cstddef>		/* STD-C */
#include	<cstdlib>		/* STD-C */
#include	<cstdio>		/* STD-C */
#include	<cnumeric>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"libf.h"

import modutil ;
import modmisc ;			/* |mknpath(3u)| */


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int freader(FILE *fp,void *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (fp && rbuf) ylikely {
	    rs = SR_INVALID ;
	    if (rlen > 0) ylikely {
		csize rdsize = size_t(rlen + 1) ;
	        size_t res ;
	        res = fread(rbuf,rdsize,1uz,fp) ;
		if (feof(fp)) {
		    rs = SR_OK ;
		} else if (ferror(fp)) {
		    rs = (errno) ? (neg errno) : SR_IO ;
	            clearerr(fp) ;
		} else {
		    len = saturate<int>(res) ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (freader) */

int fread(FILE *fp,void *rbuf,int rlen) noex {
    	return freader(fp,rbuf,rlen) ;
} /* end subroutine (fread) */


