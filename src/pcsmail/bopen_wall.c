/* bopen_wall */

/* open but so that all can write */


#define	CF_DEBUG	0


/************************************************************************
 *									
	= David A.D. Morano	94/01/06
	This subroutine was originally written.
*
*
************************************************************************/



#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<sys/stat.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<errno.h>
#include	<unistd.h>
#include	<string.h>
#include	<signal.h>
#include	<time.h>
#include	<pwd.h>
#include	<grp.h>
#include	<stdio.h>

#include	<usystem.h>
#include	<baops.h>
#include	<logfile.h>
#include	<bfile.h>

#include	"localmisc.h"



/* external subroutines */


/* external data */


/* global data */


/* forward references */




int bopen_wall(fp,fname)
bfile	*fp ;
char	fname[] ;
{
	int	rs, fd ;


	if ((rs = bopen(fp,fname,"wa",0666)) < 0) {

	    if ((rs = bopen(fp,fname,"wca",0666)) < 0) {

	        unlink(fname) ;

	        rs = bopen(fp,fname,"wca",0666) ;

	    }

	    if (rs >= 0)
	        bcontrol(fp,BC_CHMOD,0666) ;

	}

	return rs ;
}



