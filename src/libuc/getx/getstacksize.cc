/* getstacksize SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* retrieve a specified stack size from the system */
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
	Retrieve a stack size value from the system.  Various
	stacks are identified by a stack types (see |stacktypes|).

	Synopsis:
	int getstacksize(stacktypes w) noex

	Arguments:
	w		which value to retrieve:
				stacktype_thread	-> minimum stack
				stacktype_default	-> default stack

	Returns:
	>0		returned value
	==0		*na*
	<0		error (system-return)

	Notes:
	Of course, both minimum and default stack sizes are highly
	OS dependent.  Although there seems to be a way to find out
	what the minimum stack size (for a given OS) is, there does
	not seem to be a way to retrieve the default stack (see
	DEFSTACKSIZE define below) size for anyone (any OS).  So,
	bascially, this code needs to be updated and recompiled for
	any given OS.  Some better solution for this situation should
	be found.  

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<csignal>		/* CSTD |sig_atomic_t| */
#include	<climits>		/* CSTD |PTHREAD_STACK_MIN| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucsysconf.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU |_SC_THREAD_STACK_MIN| */
#include	<localmisc.h>		/* LIBU */
#include	<stacktypes.h>		/* LIBU */

#include	"getstacksize.h"


/* local defines */

#define	GETSTACKSIZE	getstacksize_head


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct getstacksize_head {
	sig_atomic_t	ss[stacktype_overlast] ;
} ; /* end struct */


/* forward references */

local int	getval(stacktypes) noex ;


/* local variables */

static GETSTACKSIZE	getstacksize_data ;


/* exported variables */


/* exported subroutines */

int getstacksize(stacktypes w) noex {
	GETSTACKSIZE	*op = &getstacksize_data ;
	int		rs = SR_INVALID ;
	if ((w >= 0) && (w < stacktype_overlast)) ylikely {
	    if (op->ss[w] == 0) {
	        if ((rs = getval(w)) >= 0) {
	            op->ss[w] = rs ;
	        }
	    } else {
	        rs = op->ss[w] ;
	    }
	} /* end if (valid) */
	return rs ;
} /* end subroutine (getstacksize) */


/* local subroutines */

local int getval(stacktypes w) noex {
	int		rs ;
	switch (w) {
	case stacktype_thread:
	    {
		cint	cmd = _SC_THREAD_STACK_MIN ;
	        rs = uc_sysconfval(cmd,nullptr) ;
	    }
	    break ;
	case stacktype_default:
	    rs = PTHREAD_STACK_MIN ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return rs ;
} /* end subroutine (getval) */


