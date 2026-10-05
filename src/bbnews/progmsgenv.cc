/* progmsgenv */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* create an environment data */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable */
#define	CF_DEBUG	0		/* run-time debug print-outs */

/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We figure ot the user-newsrc file here.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctype.h>

#include	<usystem.h>
#include	<bfile.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern int	snsds(char *,int,cchar *,cchar *) ;
extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	matostr(cchar **,int,cchar *,int) ;
extern int	matocasestr(cchar **,int,cchar *,int) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) ;
extern int	debugprinthex(cchar *,int,cchar *,int) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* external variables */


/* external variables */


/* local structures */


/* forward references */

local int	progmsgenv_beginer(PROGINFO *pip) ;


/* local variables */


/* exported subroutines */


int progmsgenv_begin(PROGINFO *pip)
{
	int	rs = SR_OK ;

	if (pip == NULL) return SR_FAULT ;

	return rs ;
}
/* end subroutine (progmsgenv_begin) */


int progmsgenv_end(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip->open.envdate) {
	    pip->open.envdate = FALSE ;
	    rs1 = dater_finish(&pip->envdate) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (progmsgenv_end) */


int progmsgenv_envstr(PROGINFO *pip,char mbuf[],int mlen)
{
	int		rs ;

	if ((rs = progmsgenv_beginer(pip)) >= 0) {
	    rs = dater_mkstd(&pip->envdate,mbuf,mlen) ;
	}

	return rs ;
}
/* end subroutine (progmsgenv_envstr) */


/* local subroutines */


local int progmsgenv_beginer(PROGINFO *pip)
{
	int	rs = SR_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	debugprintf("progmsgenv_beginer: zn=%s\n",pip->zname) ;
#endif

	if (! pip->open.envdate) {
	    struct timeb	*nowp = &pip->now ;
	    DATER		*dp = &pip->envdate ;
	    cchar		*zn = pip->zname ;
	    if ((rs = dater_start(dp,nowp,zn,-1)) >= 0) {
		time_t		t = pip->daytime ;
		cint	isdst = nowp->dstflag ;
		cint	zoff = nowp->timezone ;
	        pip->open.envdate = TRUE ;
		rs = dater_settimezon(dp,t,zoff,zn,isdst) ;
	    }
	}

	return rs ;
}
/* end subroutine (progmsgenv_beginer) */


