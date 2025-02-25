/* mailfiles_parse SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* subroutine to parse MAILPATH */
/* version %I% last-modified %G% */


/* revision history:

	= 1988-02-01, David A­D­ Morano
	This code was originally written.

	= 1988-02-01, David A­D­ Morano
	This subroutine was modified to not write out anything to
	standard output if the access time of the associated terminal
	has not been changed in 10 minutes.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/************************************************************************

  	Name:
	mailfiles_parse

	Desfcription:
	This subroutine looks at the environment variables MAILPATH
	and parses it into its component mailfiles.  Each mailfile
	is added to the 'mailfiles' object.

	Synposis:
	int mailfiles_parse(mailfiles *mfp,cchar *variable) noex

	Arguments:
	mfp		object pointer
	variable	whatever

	Returns:
	>=0		OK
	<0		error (system-return)

*************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<time.h>
#include	<stdlib.h>
#include	<string.h>
#include	<netdb.h>
#include	<usystem.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"mailfiles.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailfiles_parse(mailfiles *lp,cchar *mailpath) noex {
	int		rs ;
	int		f_done = FALSE ;
	char		*cp, *cp1 ;

	if ((lp == NULL) || (mailpath == NULL))
	    return SR_FAULT ;

	cp = (char *) mailpath ;
	while ((cp1 = strpbrk(cp,":?")) != NULL) {

	    rs = mailfiles_add(lp,cp,(cp1 - cp)) ;

	    if (rs < 0)
	        return rs ;

	    if (*cp1 == '?')
	        f_done = TRUE ;

	    cp = cp1 + 1 ;

	} /* end while */

	if (! f_done)
	    rs = mailfiles_add(lp,cp,-1) ;

	return rs ;
}
/* end subroutine (mailfiles_parse) */



