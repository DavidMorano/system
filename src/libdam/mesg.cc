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
	flag	switch:
			!= 0	allow messages
			0	forbid messages

	Returns:
	1  	if messages were previously ON
	0	if messages were previously OFF
	<0	error (system-return)
 
****************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"mesg.h"


/* local defines */


/* local namespaces */

using libuc::libmem ;			/* variable */


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
	int		rs1 ;
	int		rc = 0 ; /* return-value */
	if (char *tbuf ; (rs = libmem.mp(&tbuf)) >= 0) {
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
	                    fm &= (compl S_IWGRP) ;
	                    rs = u_chmod(tbuf,fm) ;
	                }
	            } /* end if */
	        } /* end if (u_stat) */
	    } /* end if (OK) */
	    rs1 = libmem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rc : rs ;
} /* end subroutine (mesg) */


