/* mfs-debug */

/* MFS-debug */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* switchable at invocation */


/* revision history:

	= 2011-01-25, David A�D� Morano
        I had to separate this code due to AST-code conflicts over the system
        socket structure definitions.

	= 2017-08-10, David A�D� Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright � 2011,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is MFS used for debugging.


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>

#include	<usystem.h>
#include	<localmisc.h>

#include	"mfsmain.h"
#include	"mfsconfig.h"
#include	"mfslocinfo.h"
#include	"mfslog.h"
#include	"defs.h"


/* local typedefs */


/* local defines */

#ifndef	POLLINTMULT
#define	POLLINTMULT	1000
#endif

#ifndef	PBUFLEN
#define	PBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif

#define	NDF		"/tmp/mfs.deb"


/* external subroutines */

extern int	snsd(char *,int,const char *,uint) ;
extern int	snsds(char *,int,const char *,const char *) ;
extern int	sncpy1(char *,int,const char *) ;
extern int	sncpy2(char *,int,const char *,const char *) ;
extern int	sncpy3(char *,int,const char *,const char *,const char *) ;
extern int	mkpath1w(char *,const char *,int) ;
extern int	mkpath1(char *,const char *) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	mkpath3(char *,const char *,const char *,const char *) ;
extern int	matstr(const char **,const char *,int) ;
extern int	matostr(const char **,int,const char *,int) ;
extern int	sfdirname(const char *,int,const char **) ;
extern int	cfdeci(const char *,int,int *) ;
extern int	cfdecui(const char *,int,uint *) ;
extern int	cfdecti(const char *,int,int *) ;
extern int	cfdecmfi(const char *,int,int *) ;
extern int	ctdeci(char *,int,int) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(const char *,...) ;
extern int	strllen(const char *,int,int) ;
#endif

extern char	*strwcpy(char *,const char *,int) ;
extern char	*timestr_log(time_t,char *) ;
extern char	*timestr_logz(time_t,char *) ;
extern char	*timestr_elapsed(time_t,char *) ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


int mfsdebug_lockprint(PROGINFO *pip,cchar *place)
{
	int		rs = SR_OK ;

	if (pip == NULL) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5)) {
	    LOCINFO	*lip = pip->lip ;
	    bfile	lf ;
	    int		rs1 ;
	    const char	*lockfname = lip->pidfname ;
	    if (place != NULL)
	        debugprintf("mfsdebug_lockprint: place=%s\n",place) ;
	    debugprintf("mfsdebug_lockprint: lockfname=%s\n",lockfname) ;
	    if ((rs1 = bopen(&lf,lockfname,"r",0666)) >= 0) {
	        const int	llen = LINEBUFLEN ;
	        char		lbuf[LINEBUFLEN+1] ;
	        while ((rs1 = breadln(&lf,lbuf,llen)) > 0) {
	            int	ll = strllen(lbuf,rs1,60) ;
	            debugprintf("mfsdebug_lockprint: >%t<\n",lbuf,ll) ;
	        }
	        bclose(&lf) ;
	    }
	    debugprintf("mfsdebug_lockprint: end rs=%d\n",rs1) ;
	}
#endif /* CF_DEBUG */

	return rs ;
}
/* end subroutine (mfsdebug_lockprint) */


