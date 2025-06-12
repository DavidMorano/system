/* getpassword SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* read a password from the user's terminal */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-17, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getpassword

	Description:
	This is a replacement for some UNIX subroutine of the same
	sort of function ('getpass(3c)').  I forget why this one
	is better but most of the UNIX system libraries calls are
	crap so I really do not need to look for the reason.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<termios.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<ischarx.h>
#include	<usystem.h>

#include	"getpassword.h"

import libutil ;

/* local defines */

#ifndef	PASSWDLEN
#define	PASSWDLEN	32
#endif

#ifndef	TTYFNAME
#define	TTYFNAME	"/dev/tty"
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getpassword(cchar *prompt,char *passbuf,int passlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rlen = 0 ;
	if (prompt && passbuf) {
	    if ((rs = u_open(TTYFNAME,O_RDWR,0666)) >= 0) {
	        cint	fd = rs ;
	        if (TERMIOS oterm ; (rs = uc_tcattrget(fd,&oterm)) >= 0) {
	            TERMIOS	nterm = oterm ;
		    nterm.c_lflag &= (~ ECHO) ;
	            if ((rs = uc_tcattrset(fd,TCSADRAIN,&nterm)) >= 0) {
	                cint	pl = xstrlen(prompt) ;
	                if ((rs = uc_write(fd,prompt,pl)) >= 0) {
	                    passbuf[0] = '\0' ;
	                    rs = u_read(fd,passbuf,passlen) ;
	                    rlen = rs ;
	                }
	    	        uc_tcattrset(fd,TCSADRAIN,&oterm) ; /* try! */
	            } /* end if (terminal attributes) */
	        } /* end if (get-term-attributes) */
	        rs1 = u_close(fd) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (open) */
	    if ((rs >= 0) && (rlen > 0)) {
	        if (iseol(passbuf[rlen - 1])) {
	            rlen -= 1 ;
	        }
	        passbuf[rlen] = '\0' ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (getpassword) */


