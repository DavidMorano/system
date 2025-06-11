/* progmsgfromaddr */

/* create a mail-msg FROM-address */


#define	CF_DEBUGS	0		/* not-switchable debug print-outs */
#define	CF_DEBUG	0		/* run-time */


/* revision history:

	= 1998-05-01, David A�D� Morano
	This subroutine is originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/******************************************************************************

	This subroutine creates a default FROM-address.

	Synopsis:

	int progmsgfromaddr(pip)
	struct proginfo	*pip ;

	Arguments:

	pip		pointer to program information

	Returns:

	>=0		OK
	<0		error


******************************************************************************/


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>

#include	<usystem.h>
#include	<buffer.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	MABUFLEN
#define	MABUFLEN	((3 * MAXHOSTNAMELEN) + 1024)
#endif

#ifndef	REALNAMELEN
#define	REALNAMELEN	100
#endif

#ifndef	VARMAILFROM
#define	VARMAILFROM	"MAILFROM"
#endif


/* external subroutines */

extern int	progmsgfromname(struct proginfo *) ;


/* local structures */


/* forward references */

static int	procfrom(struct proginfo *) ;


/* local variables */


/* exported subroutines */


int progmsgfromaddr(pip)
struct proginfo	*pip ;
{
	int	rs = SR_OK ;

	if (pip->msgfromaddr == NULL) {
	    const char	*cp = NULL ;

	    if (pip->msgfromaddr == NULL) {
	        if ((cp = getenv(VARPROGMAILFROM)) != NULL) {
		    if (cp[0] != '\0') {
		        pip->msgfromaddr = cp ;
		        rs = strlen(cp) ;
		    }
	        }
	    }

	    if (pip->msgfromaddr == NULL) {
	        if ((cp = getenv(VARMAILFROM)) != NULL) {
		    if (cp[0] != '\0') {
		        pip->msgfromaddr = cp ;
		        rs = strlen(cp) ;
		    }
	        }
	    }

	    if (pip->msgfromaddr == NULL)
		rs = procfrom(pip) ;

	} else {
	    rs = strlen(pip->msgfromaddr) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progmsgfromaddr: ret rs=%d from=>%s<\n",
	        rs,pip->msgfromaddr) ;
#endif

	return rs ;
}
/* end subroutine (progmsgfromaddr) */


/* local subroutines */


static int procfrom(pip)
struct proginfo	*pip ;
{
	BUFFER	b ;

	int	rs ;
	int	bl = 0 ;


	if ((rs = buffer_start(&b,MABUFLEN)) >= 0) {
	    const char	*bp ;

	    buffer_strw(&b,pip->username,-1) ;
	    buffer_chr(&b,'@') ;
	    buffer_strw(&b,pip->domainname,-1) ;

/* add a name if we can find one */

	    if (pip->msgfromname == NULL)
	        rs = progmsgfromname(pip) ;

	    if ((rs >= 0) && (pip->msgfromname != NULL)) {
	        buffer_chr(&b,' ') ;
	        buffer_chr(&b,CH_LPAREN) ;
	        buffer_strw(&b,pip->msgfromname,-1) ;
	        buffer_chr(&b,CH_RPAREN) ;
	    } /* end if (adding name) */

	    if (rs >= 0) {
	        if ((rs = buffer_get(&b,&bp)) >= 0) {
	            bl = rs ;
	            rs = proginfo_setentry(pip,&pip->msgfromaddr,bp,bl) ;
		}
	    }

	    buffer_finish(&b) ;
	} /* end if */

	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (procfrom) */



