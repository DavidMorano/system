/* mesg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* enable or disable messaging on the user's terminal device */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-22, David A­D­ Morano
	This code was originally written.  Unfortunately, it depends
	on the controlling terminal still being (or being) available
	on one of the file descriptors between 0 and 2 inclusive.
	This may be a difficult thing to arrange for in certain
	circumstances!

	= 1999-01-10, David A­D­ Morano
	This subroutine was enhanced to use the POSIX |ttyname_r(3c)|
	subroutine.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/****************************************************************************

  	Name:
	mesg

	Description:
 	I set the current (controlling?) terminal to accept or
 	forbid write permission.  Tis corresponds to receiving
 	anonymous "messages" or not from other users.
 
	Synopsis:
 	int mesg(int flag) noex

	Arguments:
	!= 0	allow messages
	0	forbid messages

	Returns:
	1  	if messages were previously ON
	0	if messages were previously OFF
	<0	error (system-return)
 
****************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<localmisc.h>

#include	"mesg.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mesg(int flag) noex {
	int		rs ;
	int		rc = 0 ; /* return-value */
	if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	    cint	tlen = rs ;
	    for (int i = 0 ; i < 3 ; i += 1) {
	        rs = ttyname_r(i,tbuf,tlen) ;
	        if (rs >= 0) break ;
	    } /* end for */
	    if (rs >= 0) {
	        if (ustat sb ; (rs = u_stat(tbuf,&sb)) >= 0) {
		    mode_t	fm = sb.st_mode ;
	            rc = (sb.st_mode & S_IWGRP) ? 1 : 0 ;
	            if (flag) {
	                if (! rc) {
	                    fm |= S_IWGRP ;
	                    rs = u_chmod(tbuf,fm) ;
	                }
	            } else {
	                if (rc) {
	                    fm &= (~ S_IWGRP) ;
	                    rs = u_chmod(tbuf,fm) ;
	                }
	            } /* end if */
	        } /* end if (u_stat) */
	    } /* end if (OK) */
	    rs = rsfree(rs,tbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (mesg) */


