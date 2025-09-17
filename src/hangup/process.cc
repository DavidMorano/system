/* process */

/* process this device */


#define	CF_DEBUG 	0


/* revision history:

	= 1996-03-01, David A­D­ Morano

	This code was originally written.


*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/***********************************************************************

	Arguments:
	- pip		program information pointer
	- basedir	directory at top of tree
	- name		device name to pop off !



***********************************************************************/


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<termios.h>
#include	<cstdlib>
#include	<cstring>

#include	<bfile.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX((MAXPATHLEN + 10),2048)
#endif


/* external subroutines */


/* external variables */


/* exported subroutines */


int process(pip,ofp,basedir,name)
struct proginfo	*pip ;
bfile		*ofp ;
char		basedir[] ;
char		name[] ;
{
	ustat	sb ;

	int	rs, len ;

	char	dfname[MAXPATHLEN + 1] ;
	char	*bdp ;


	if (name == NULL) 
		return SR_FAULT ;

	bdp = name ;
	if (((rs = perm(name,-1,-1,NULL,R_OK | W_OK)) < 0) &&
	    (name[0] != '/')) {

	    if ((basedir == NULL) || (basedir[0] == '\0'))
	        basedir = DEVBASE ;

	    bdp = dfname ;
	    bufprintf(dfname,MAXPATHLEN,"%s/%s",basedir,name) ;

	    if ((rs = perm(bdp,-1,-1,NULL,R_OK | W_OK)) < 0) {

	        bprintf(pip->efp,"%s: could not access \"%s\" rs=%d\n",
	            pip->progname,bdp,rs) ;

	        return rs ;
	    }

	} /* end if */

	if (pip->fl.print || pip->fl.verbose)
	    bprintf(ofp,"device=%s\n",bdp) ;

	if (pip->fl.print || pip->fl.fakeit) 
		return SR_OK ;


	if ((rs = u_open(bdp,O_RDWR | O_NONBLOCK | O_NOCTTY,0666)) >= 0) {

	    struct termios	settings ;

	    int	tfd = rs ;


	    if ((rs = uc_tcgetattr(tfd,&settings)) >= 0) {

	        cfsetospeed(&settings,B0) ;

/* hang it up, baby ! */

	        if (! pip->fl.fakeit)
	            rs = uc_tcsetattr(tfd,TCSANOW,&settings) ;

	    }

	    u_close(tfd) ;

	} else
	    bprintf(pip->efp,"%s: could not open \"%s\" rs=%d\n",
	        pip->progname,bdp,rs) ;

	return rs ;
}
/* end subroutine (process) */



