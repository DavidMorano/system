/* ldebug SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* local debugging stubs */
/* version %I% last-modified %G% */


/* revision history:

	- 1997-11-21, David A­D­ Morano
	This program was started by copying from the RSLOW program.

*/

/* Copyright © 1997 David A­D­ Morano.  All rights reserved. */

/**************************************************************************

  	Name:
	ldebug

	Description:
	This modeule provides debugging support for the REXEC
	program.

**************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"



/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

/* convert file mode bits to ASCII string */
cchar *d_procmode(int mode,hcar *dbuf,int dlen) noex {
    	char	*rp = nullptr ;
	(void) dbuf ;
	(void) dlen ;
	switch (mode) {
	case IM_FILE:
	    rp = "FILE" ;
	    break ;
	case IM_PIPE:
	    rp = "PIPE" ;
	    break ;
	case IM_SEEK:
	    rp = "SEEK" ;
	    break ;
	} /* end switch */
	return rp ;
}
/* end subroutine (d_procmode) */


