/* pocess */

/* print out the status of a file */

#define	CF_DEBUG		1

/* revision history:

	= 1987-09-01, David A­D­ Morano
	This code was originally written.

*/

/*******************************************************************************

	This program will return the various time statusi of a file.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<tzfile.h>		/* |TM_YEAR_BASE| */
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>
#include	<paramopt.h>
#include	<bfile.h>
#include	<localmisc.h>		/* LIBU */

#include	"misc.h"
#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* exported variables */


/* exported subroutines */

int process(pip,otype,ofp,filename,pp)
struct proginfo	*pip ;
int		otype ;
bfile		*ofp ;
char		filename[] ;
PARAMOPT	*pp ;
{
	ustat	ss, *sp = &ss ;
	TM	*ftsp ;
	int	rs ;
	int	i ;
	char	dbuf[MAXPATHLEN + 1], *bp ;
	char	tbuf[40] ;
	char	*cbuf ;
	char	*cp ;

	/* do it */
	if ((rs = u_stat(filename,sp)) < 0)
	    goto badstat ;


	if (pip->verboselevel > 0)
	    bprintf(ofp,"\\o%08o\n",sp->st_mode) ;


#if	CF_DEBUG
	if (pip->debuglevel >= 3)
	eprintf("process: about to get time\n") ;
#endif

	ftsp = localtime(&sp->st_mtime) ;


	switch (otype) {

	case OTYPE_INT:
	    rs = bprintf(ofp,"%ld\n",sp->st_mtime) ;

	    break ;

	case OTYPE_TOUCH:
	    rs = bprintf(ofp,"%02.2d%02.2d%02.2d%02.2d%02d\n",
	        ftsp->tm_mon + 1,
	        ftsp->tm_mday,
	        ftsp->tm_hour,
	        ftsp->tm_min,
	        ftsp->tm_year) ;

	    break ;

	case OTYPE_TOUCHT:
	case OTYPE_TTOUCH:
	    rs = bprintf(ofp,"%04d%02.2d%02.2d%02.2d%02.2d.%02d\n",
	        ftsp->tm_year + TM_YEAR_BASE,
	        ftsp->tm_mon + 1,
	        ftsp->tm_mday,
	        ftsp->tm_hour,
	        ftsp->tm_min,
	        ftsp->tm_sec) ;

	    break ;

	case OTYPE_INFO:
	    bp = dbuf ;

	    cbuf = ctime(&sp->st_atime) ;
	    cbuf[24] = '\0' ;

	    bp += sprintf(bp," accessed %s\n",
	        cbuf) ;


	    cbuf = ctime(&sp->st_mtime) ;

	    cbuf[24] = '\0' ;

	    bp += sprintf(bp," modified %s\n",
	        cbuf) ;


	    cbuf = ctime(&sp->st_ctime) ;
	    cbuf[24] = '\0' ;

	    bp += sprintf(bp," changed  %s\n",
	        cbuf) ;

	    rs = bwrite(ofp,dbuf,(bp - dbuf)) ;

	    break ;

	case OTYPE_ACCESS:
	    cbuf = ctime(&sp->st_atime) ;
	    cbuf[24] = '\0' ;

		rs = bprintf(ofp,"%s\n",cbuf,24) ;

		break ;

	default:
		rs = SR_NOTSUP ;

	} /* end switch */


badstat:

#if	CF_DEBUG
	if (pip->debuglevel >= 3)
	eprintf("process: exiting rs=%d\n",rs) ;
#endif


	return rs ;
} /* end subroutine (process) */


