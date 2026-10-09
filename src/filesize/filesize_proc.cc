/* filesize_process SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process a name */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/******************************************************************************

  	Description:
	This module just provides optional expansion of directories.
	The real work is done by the 'checkname' module.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<field.h>		/* LIBUC */
#include	<paramopt.h>		/* LIBUC */
#include	<wdt.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"filesize_config.h"


/* local defines */


/* external subroutines */

extern int	checkname() ;

extern char	*strbasename(char *) ;


/* external variables */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int process(PI *pip,cchar *name,paramopt *pp) noex {
	ustat		sb, sb2 ;
	checkparams	ck ;
	int	rs ;
	int	wopts = 0 ;

	if (name == nullptr) 
		return BAD ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
		eprintf("process: entered name=\"%s\"\n",name) ;
#endif

	if (u_lstat(name,&sb) < 0) 
		return BAD ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
		eprintf("process: name=\"%s\" mode=%0o\n",
			name,sb.st_mode) ;
#endif

	ck.pip = pip ;
	ck.pp = pp ;

	if (S_ISLNK(sb.st_mode)) {

	    if (pip->fl.follow &&
		(u_stat(name,&sb2) >= 0) && S_ISDIR(sb.st_mode)) {

#if	CF_DEBUG
	if (pip->debuglevel > 1)
		eprintf("process: calling wdt\n") ;
#endif

		wopts != (pip->fl.follow) ? WDT_MFOLLOW : 1 ;
		rs = wdt(name,WDTM_FOLLOW,checkname,&ck) ;

	    } else
		rs = checkname(name,&sb,&ck) ;

	} else
		rs = checkname(name,&sb,&ck) ;

#if	CF_DEBUG
	if (pip->debuglevel > 1)
		eprintf("process: rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (process) */


