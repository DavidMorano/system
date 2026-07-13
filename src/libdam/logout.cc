/* logout SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* log out of the system (out of the UTMP database) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	= 1999-03-01, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine logs the caller out of the system. What this really does
        is to log the caller out of the UTMP database.

	Synopsis:
	int logout(pid_t sid) noex

	Arguments:
	sid		session ID to log-out

	Returns:
	0		OK
	SR_PERM		no permission to perform function
	SR_NOENT	session was not logged in

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<tmpx.h>		/* LIBUC */
#include	<utmptypes.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"logout.h"


/* local defines */

#ifndef	WTMPFNAME
#define	WTMPFNAME	"/var/adm/wtmpx"
#endif

#ifndef	UTMPFNAME
#define	UTMPFNAME	"/var/adm/utmpx"
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif

#define	F_UTXEXIT	(F_LINUX || F_SUNOS)


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cint	utx_user	= utmptype.procuser ;
cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int logout(pid_t sid) noex {
	utmpx		*up = nullptr ;
	int		rs = SR_OK ;
	int		si = 0 ; /* return-value */
	DEBUGPRINTF("ent sid=%d\n",sid) ;
	if (sid <= 0) sid = getsid(0) ;	/* not supposed to fail! */
	setutxent() ;
	while ((up = getutxent()) != nullptr) {
	    if ((up->ut_pid == sid) && (up->ut_type == utx_user)) break ;
	    si += 1 ;
	} /* end while (positioning within the UTMPX file) */
	if (up) {
	    utmpx uc = *up ;		/* copy the record found */
#if	F_UTXEXIT
	        uc.ut_exit.e_termination = 0 ;
	        uc.ut_exit.e_exit = 0 ;
#endif /* F_UTXEXIT */
	    uc.ut_type = utmptype.procdead ;
	    up = pututxline(&uc) ;
	    if (up == nullptr) rs = SR_PERM ;
	} else {
	    rs = SR_NOENT ;
	}
	endutxent() ;
	DEBUGPRINTF("ret rs=%d si=%d\n",rs,si) ;
	return (rs >= 0) ? si : rs ;
} /* end subroutine (logout) */


