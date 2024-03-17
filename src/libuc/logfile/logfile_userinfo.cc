/* logfile_userinfo SUPPORT */
/* lang=C++20 */

/* log user information */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-01-20, David A­D­ Morano
	This was originally written to collect some common PCS code
	into one subroutines.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutines is generally used to make the first log-file
	entry at the start of a program involcation. The subroutine
	requires that both a LOGFILE object have already been opened
	and that the caller has retrieved the user information using
	the userinfo facility (and passed a pointer to that down
	to this subroutine).

	Synopsis:
	int logfile_userinfo(logfile *op,userinfo *uip,time_t dt,
		cc *pn,cc *vn) noex

	Arguments:
	op		pointer to LOGFILE object
	uip		pointer to userinfo object
	dt		current UNIX® time of day
	pn		program name
	vn		program-version name

	Returns:
	>=0	number of bytes written to log
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<logfile.h>
#include	<userinfo.h>
#include	<localmisc.h>


/* local defines */

#ifndef	VARARCHITECTURE
#define VARARCHITECTURE	"ARCHITECTURE"
#endif

#ifndef	BANGBUFLEN
#define	BANGBUFLEN	(NODENAMELEN+1+USERNAMELEN)
#endif

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif


/* external subroutines */

extern "C" {
    extern int	mkuibang(char *,int,userinfo *) noex ;
    extern int	mkuiname(char *,int,userinfo *) noex ;
}

extern "C" {
    extern char	*timestr_logz(time_t,char *) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int logfile_userinfo(LOGFILE *op,userinfo *uip,time_t dt,cc *pn,cc *vn) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar	*a = getenv(VARARCHITECTURE) ;
	cchar	*fmt ;
	char		timebuf[TIMEBUFLEN + 1] ;

	if ((op == NULL) || (uip == NULL))
	    return SR_FAULT ;

	if (pn == NULL)
	    pn = "" ;

	if (vn == NULL)
	    vn = "" ;

	if (dt <= 0)
	    dt = time(NULL) ;

/* first line (w/ possible additional information) */

	fmt = "%s starting" ;
	if ((pn[0] != '\0') || (vn[0] != '\0'))
	    fmt = "%s %s %s/%s" ;

	{
	    cchar	*ts = timestr_logz(dt,timebuf) ;
	    cchar	*st = (uip->f.sysv_ct) ? "SYSV" : "BSD" ;
	    rs = logfile_printf(op,fmt,ts,pn,vn,st) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    cchar	*sn = uip->sysname ;
	    cchar	*rn = uip->release ;
	    cchar	*dn = uip->domainname ;
	    if (a != NULL) {
	        rs = logfile_printf(op,"a=%s os=%s(%s) d=%s",a,sn,rn,dn) ;
		wlen += rs ;
	    } else {
	        rs = logfile_printf(op,"os=%s(%s) d=%s",sn,rn,dn) ;
		wlen += rs ;
	    }
	}

	if (rs >= 0) {
	    char	namebuf[BANGBUFLEN + 1] ;
	    if ((rs1 = mkuibang(namebuf,BANGBUFLEN,uip)) >= 0) {
	        rs = logfile_print(op,namebuf,rs1) ;
	        wlen += rs ;
	    }
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (logfile_userinfo) */


