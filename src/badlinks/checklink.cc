/* checklink */

/* check a directory entry for a dangling link */


#define	CF_DEBUG 	0


/* revision history:

	= 96/03/01, David A­D­ Morano

	This code was originally written.


*/


/***********************************************************************

	Arguments:
	- name		directory entry
	- sbp		'stat' block pointer


	Returns:
	<0		exit directory walk altogether
	0		continue with directory walk as usual
	>0		skip this directory entry in directory walk


***********************************************************************/



#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<dirent.h>
#include	<climits>

#include	<usystem.h>
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


int checklink(pip,name,sbp)
struct proginfo	*pip ;
const char	name[] ;
ustat	*sbp ;
{
	ustat	sb2 ;

	int	rs ;


	if (! S_ISLNK(sbp->st_mode)) 
		return 0 ;

	if ((rs = u_stat(name,&sb2)) >= 0) 
		return 0 ;

#if	CF_DEBUG
	if (pip->debuglevel > 0)
	bprintf(pip->efp,"%s: stat rs=%d\n",
	    pip->progname,rs) ;
#endif

	if (pip->debuglevel > 0)
	    bprintf(pip->efp,"%s: rs=%d badlink=%s\n",
	        pip->progname,rs,name) ;

	if (rs != SR_NOENT)
		return 1 ;


	if (! pip->fl.no) {

#if	CF_DEBUG
	if (pip->debuglevel > 0)
	    bprintf(pip->efp,"%s: unlinked=%s\n",
	        pip->progname,name) ;
#else
	    u_unlink(name) ;
#endif

	} /* end if (removing) */

	if (pip->fl.print || (pip->verboselevel > 0))
	    bprintf(pip->ofp,"%s\n", name) ;

	return 1 ;
}
/* end subroutine (checklink) */



