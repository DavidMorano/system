/* printout SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* send the article to the PCS printer */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-02-01, David A�D� Morano
	I wrote this from scratch when I took over the code.  The
	previous code was a mess (still is in many places!).

	= 1998-11-22, David A�D� Morano
        I did some clean-up.

*/

/* Copyright � 1994,1998 David A�D� Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getfiledirs.h>
#include	<bfile.h>
#include	<localmisc.h>

#include	"artlist.h"
#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	CMDBUFLEN
#define	CMDBUFLEN	(2 * MAXPATHLEN)
#endif


/* external subroutines */

extern int	bufprintf(char *,int,cchar *,...) ;


/* external variables */


/* exported subroutines */


int cmd_printout(pip,ap,ngdir,afname,options)
struct proginfo	*pip ;
ARTLIST_ENT	*ap ;
const char	ngdir[] ;
const char	afname[] ;
const char	options[] ;
{
	int		rs = SR_OK ;
	const char	*cp ;
	char		cmdbuf[CMDBUFLEN + 1] ;

	if ((cp = getenv("BBPRTCMD")) == NULL)
	    cp = pip->prog_print ;

	if (getfiledirs(NULL,cp,"x",NULL) <= 0) {

	    bprintf(pip->efp,
	        "%s: couldn't find the print program \"%s\"\n",
	        pip->progname,cp) ;

	    goto ret0 ;
	}

	rs = bufprintf(cmdbuf,CMDBUFLEN,"%s < %s %s",
	    cp,afname,options) ;

	if (rs >= 0)
	rs = system(cmdbuf) ;

ret0:
	return rs ;
}
/* end subroutine (cmd_printout) */



