/* getmaxpid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the maximum PID the OS will create */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Copied from |gethz(3uc)|.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getmaxpid

	Synopsis:
	int getmaxpid(int w) noex 

	Arguments:
	w		which maximum to report:
				0	all processes
				1	system processes (where available)

	Returns:
	>0		MAXPID value
	==0		?
	<0		error (system-return)

	Notes:
	+ I try to get a dynamic value if we can, otherwise we let
	the system itself guess (w/ PID_MAX), thereafter we guess
	with our own value.  There is no dynamic value (that we know
	of) for the maximum "system" PID.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® |pid_t| */
#include	<unistd.h>		/* POSIX® |sysconf(3c)| */
#include	<climits>		/* CSTD |PID_MAX| (if supplied) */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucsysconf.h>		/* LIBU */
#include	<isoneof.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getmaxpid.h"


/* local defines */

#ifndef	PID_MAX
#define	PID_MAX		999999		/* six '9's */
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct pidmgr {
	pid_t		pid ;		/* assume this is atomic */
} ; /* end struct */


/* forward references */

local int	getsys() noex ;
local bool	isnosys(int) noex ;


/* local variables */

static pidmgr		getmaxpid_data ;

constexpr int		rsnosys[] = {
    	SR_NOSYS,
	SR_INVALID,
	0
} ; /* end array */


/* exported variables */

libuc::maxpider		maxpid ;


/* exported subroutines */

int getmaxpid(int w) noex {
	int		rs = SR_INVALID ;
	switch (w) {
	case 0:
	    rs = getsys() ;
	    break ;
	case 1:
	    rs = PID_MAX ;
	    break ;
	} /* end switch */
	return rs ;
} /* end subroutine (getmaxpid) */


/* local subroutines */

local int getsys() noex {
	pidmgr		*op = &getmaxpid_data ;
	int		rs ;
	if ((rs = op->pid) == 0) {
            cint    cmd = _SC_MAXPID ;
            if ((rs = uc_sysconfval(cmd,nullptr)) >= 0) {
                op->pid = rs ;
            } else if (isnosys(rs)) {
                rs = PID_MAX ;
                op->pid = rs ;
            }
	} /* end if (test cache) */
	return rs ;
} /* end subroutine (getsys) */

local bool isnosys(int rs) noex {
    	return isOneOf(rsnosys,rs) ;
} /* end subroutine (isnosys) */


