/* getmaxpid SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the maximum PID the OS will create */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Copied from |gethz(3dam)|.

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
	<0		error
	==0		?
	>0		MAXPID value

	Notes:
	+ We try to get a dynamic value if we can, otherwise we let
	the system itself guess (w/ PID_MAX), thereafter we guess
	with our own value.  There is no dynamic value (that we know
	of) for the maximum "system" PID.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	PID_MAX
#define	PID_MAX		999999
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct getmaxpid {
	pid_t		pid ;		/* assume this is atomic */
} ;


/* forward references */


/* local variables */

static struct getmaxpid		getmaxpid_data ; /* zero-initialized */


/* exported variables */


/* exported subroutines */

int getmaxpid(int w) noex {
	int		rs ;
	switch (w) {
	case 0:
	    {
#ifdef	_SC_MAXPID
		{
		    struct getmaxpid	*op = &getmaxpid_data ;
		    if (op->pid == 0) {
	    	        const int	cmd = _SC_MAXPID ;
	                rs = uc_sysconf(cmd,NULL) ;
		        op->pid = rs ;
	            } else {
		        rs = op->pid ;
		    }
		} /* end block */
#else
	        rs = PID_MAX ;
#endif /* _SC_MAXPID */
	    }
	    break ;
	case 1:
#ifdef	SYSPID_MAX
	    rs = SYSPID_MAX ;
#else
	    rs = SR_NOSYS ;
#endif /* SYSPID_MAX */
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */

	return rs ;
}
/* end subroutine (getmaxpid) */


