/* varsub_addvec SUPPORT */
/* lang=C++20 */

/* variable manipulation (like for configuration files!) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-12, David A­D­ Morano
	This subroutine was written to replace some older variable
	substitution stuff from the old Automation Rsearch days
	with the old SAT tool stuff.

	= 2001-09-11, David A­D­ Morano
	This junk is not aging well!  And it was really rewritten
	from essentially scratch in 1997 (only a few years ago)!
	This code has many qualities that are common with crap code.
	When a new variable was "exported" from a configuration
	file and it did not have any value part specified, and when
	the same variable was not in the existing environment (like
	if a daemon is executed directly from |init(1m)| or something
	similar), then the variable name would end up in the exported
	environment variables list as just a variable key with no
	value!  I fixed this by not putting anything into the exported
	environment if it does not have a value (at least a value
	of zero length).  I *think* that a nullptr value and and a
	zero-length value distinquish the case of the variable
	having an empty value string from one where it did not have
	any value specified at all.

*/

/* Copyright © 1998,2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These routines operate on 'defined' and 'export'ed variables
	from a configuration file.  What we want to do is to perform
	a substitution on these variables from existing variables
	in the database or from the environment otherwise.  Variables
	that are already in the database get merged like in a path
	string with colon characters (':') separating the original
	values of the variables.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<estrings.h>
#include	<vecstr.h>
#include	<strwcpy.h>
#include	<strn.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"varsub.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int varsub_addvec(varsub *op,vecstr *vsp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*sp ;
	for (int i = 0 ; vecstr_get(vsp,i,&sp) >= 0 ; i += 1) {
	    if (sp) {
		cchar	*vp ;
	        cchar	*kp = sp ;
	        if (cchar *tp ; (tp = strchr(sp,'=')) != nullptr) {
		    int		ch = mkchar(kp[0]) ;
		    bool	f ;
		    f = isprintlatin(ch) ;
	            vp = (tp + 1) ;
	            if (f) {
		        ch = mkchar(vp[0]) ;
		        f = ((ch == '\0') || isprintlatin(ch)) ;
	            }
	            if (f) {
	                rs = varsub_add(op,kp,(tp - kp),vp,-1) ;
		        if (rs < INT_MAX) c += 1 ;
	            } /* end if */
	        } /* end if */
	    } /* end if (non-null) */
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (varsub_addvec) */


