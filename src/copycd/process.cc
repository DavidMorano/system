/* process SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process a name */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that
	did similar types of functions.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/******************************************************************************

  	Description:
	This module just provides optional expansion of directories.
	The real work is done by the 'checkname' module.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<termios.h>
#include	<unistd.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<baops.h>
#include	<field.h>
#include	<paramopt.h>
#include	<wdt.h>
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

int process(proginfo *pip,cchar *name,paramopt *pp) noex {
	ustat		sb ;
	checkparams	ck ;
	int		rs ;
	if (name == NULL) return BAD ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
		debugprintf("process: entered name=\"%s\"\n",name) ;
#endif

	if (u_stat(name,&sb) < 0) 
		return BAD ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
		debugprintf("process: name=\"%s\" mode=%0o\n",
			name,sb.st_mode) ;
#endif

	ck.pip = pip ;
	ck.pp = pp ;

	if (S_ISDIR(sb.st_mode)) {

#if	CF_DEBUG
	if (pip->debuglevel > 1)
		debugprintf("process: calling wdt\n") ;
#endif

		rs = wdt(name,WDTM_FOLLOW,checkname,&ck) ;

	} else {
		rs = checkname(name,&sb,&ck) ;
	}

#if	CF_DEBUG
	if (pip->debuglevel > 1)
		debugprintf("process: rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (process) */


