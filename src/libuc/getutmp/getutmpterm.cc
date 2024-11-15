/* getutmpterm SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get the name of the controlling terminal for the current session */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-01-10, David A­D­ Morano
	This code was originally written.  It was prompted
	by the failure of other system subroutines from finding the
	proper controlling terminal (on Solaris®)!  Maybe they fixed
	their stuff -- and maybe they have not yet!

	= 2007-10-11, David A­D­ Morano
	I added a little comparison of the result with the device
	directory name.  Some other systems (no names mentioned
	here) sometimes put the line-terminal name into the UTMP
	entry with the device directory name prefixed to the
	line-terminal name.  The world is a complicated place!

	= 2010-02-22, David A­D­ Morano
	I added a crap-hack for Darwin (Apple MacOS kernel).  Darwin
	based systems -- and many other BSD (cough) systems -- are
	very screwed up in the way that they do things.

*/

/* Copyright © 1998,2007,2010 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getutmpterm

	Description:
	This subroutine will find and return the name of the
	controlling terminal for the given session ID.

	Synopsis:
	int getutmpterm(char *rbuf,int rlen,pid_t sid) noex

	Arguments:
	rbuf		user buffer to receive name of controlling terminal
	rlen		length of user supplied buffer
	sid		session ID to find controlling terminal for

	Returns:
	>=0		length of name of controlling terminal
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<uvariables.hh>
#include	<utmpent.h>
#include	<mknpathx.h>
#include	<localmisc.h>

#include	"getutmpent.h"
#include	"getutmpterm.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getutmpterm(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    if (utmpentx ute ; (rs = getutmpent(&ute,sid)) >= 0) {
		cchar	*devdir = sysword.w_devdir ;
                rs = mknpath(rbuf,rlen,devdir,ute.line) ;
	    } /* end if (getutmpent) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmpterm) */


/* local subroutines */


