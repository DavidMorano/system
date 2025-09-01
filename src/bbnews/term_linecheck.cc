/* proglinecheck */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check the number of lines on the terminal */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	1		/* run-time debugging */

/* revision history:

	= 1994-11-01, David A­D­ Morano
	- added a mode to intercept for mailbox use

	= 1994-12-01, David A­D­ Morano
        - modified to only print out header fields that a user is normally
        interested in

	= 1995-07-01, David A­D­ Morano
	- extensively modified to add:
		article follow-up capability
		article previous
		article printing
		article piping & redirecting

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1994,1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:
	int proglinecheck(proginfo *pip) noex

	Arguments:
	pip		program information pointer

	Returns:
	<0		error
	>=0		flag

*******************************************************************************/

#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/param.h>
#include	<termios.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<strings.h>
#include	<cstring>

#include	<usystem.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern int	snwcpy(char *,int,const char *,int) ;
extern int	sncpy1w(char *,int,const char *,int) ;
extern int	sncpy1(char *,int,const char *) ;
extern int	mkpath1w(char *,const char *,int) ;
extern int	mkpath1(char *,const char *) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	mkpath3(char *,const char *,const char *,const char *) ;
extern int	sfshrink(const char *,int,const char **) ;
extern int	nextfield(const char *,int,const char **) ;
extern int	getfiledirs(const char *,const char *,const char *,vecstr *) ;
extern int	bufprintf(char *,int,const char *,...) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(const char *,...) ;
extern int	debugprinthex(const char *,int,const char *,int) ;
extern int	strlinelen(const char *,int,int) ;
#endif

extern char	*strshrink(char *) ;


/* external variables */

extern int	if_winchange ;


/* forward references */


/* local variables */


/* exported subroutines */


int proglinecheck(RPOGINFO *pip)
{
	int		rs = SR_OK ;
	int		f = FALSE ;

	if (pip->fl.terminal) {
	struct winsize	ws ;

	f = if_winchange ;
	if (if_winchange) {
	    if_winchange = FALSE ;
	    if (u_ioctl(FD_STDOUT, TIOCGWINSZ, &ws) >= 0) {
	        if (ws.ws_row > 0) {
	            pip->termlines = ws.ws_row ;
		}
	    }
	}

	} /* end if (needed) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (progtermlinecheck) */


