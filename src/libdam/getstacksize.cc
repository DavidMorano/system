/* getstacksize SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the machine HZ */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This is a spin off of various programs that needed to get system
	(machine) specific paramaters.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getstacksize

	Description:
	Retrieve a stack size value from the system.

	Synopsis:
	int getstacksize(int w) noex

	Arguments:
	w		which value to retrieve:
				0 -> minimum stack size
				1 -> default stack size

	Returns:
	>0		returned value
	==0		?
	<0		error (system-return)

	Notes:
	Of course, both minimum and default stack sizes are highly
	OS dependent.  Although there seems to be a way to find out
	what the minimum stack size (for a given OS) is, there does
	not seem to be a way to retrieve the default stack (see
	DEFSTACKSIZE define below) size for anyone (any OS).  So,
	bascially, this code needs to be updated and recompiled for
	any given OS. Some better solution for this situation should
	be found.  Our little caching scheme depends on the assumption
	that an 'int' is atomic. We can live with this in most all
	machine architectures we care about.

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

#if	_LP64
#define	DEFSTACKSIZE	(2*1024)
#else
#define	DEFSTACKSIZE	(1*1024)
#endif

#define	GETSTACKSIZE	struct getstacksize


/* local structures */

struct getstacksize {
	int		ss[2] ;
} ;


/* forward references */

static int	getval(int) ;


/* local variables */

static GETSTACKSIZE	getstacksize_data ; /* zero-initialized */


/* exported variables */


/* exported subroutines */

int getstacksize(int w) noex {
	GETSTACKSIZE	*op = &getstacksize_data ;
	int		rs ;
	if (op->ss[w] == 0) {
	    if ((rs = getval(w)) >= 0) {
	        op->ss[w] = rs ;
	    }
	} else {
	    rs = op->ss[w] ;
	}
	return rs ;
}
/* end subroutine (getstacksize) */


/* local subroutines */

static int getval(int w) noex {
	int		rs = SR_INVALID ;
	switch (w) {
	case 0:
	    {
		cint	cmd = _SC_THREAD_STACK_MIN ;
	        rs = uc_sysconf(cmd,NULL) ;
	    }
	    break ;
	case 1:
	    rs = DEFSTACKSIZE ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (getval) */


