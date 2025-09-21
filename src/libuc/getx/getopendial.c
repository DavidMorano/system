/* getopendial SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 ( <- MUST be plain C-language) */

/* get the OPENDIAL code for a dialer specification (string) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getopendial

	Description:
	Get the open-dial code (an integer value) for an open-dial
	name string.

	Synopsis:
	int getopendial(cchar *dialspec) noex

	Arguments:
	dialspec	the dialer specification string

	Returns:
	>=0		the open-dialer code for the specified dialer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<stddef.h>		/* |nullptr_t| */
#include	<stdlib.h>
#include	<string.h>		/* |strcmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<matstr.h>
#include	<localmisc.h>

#include	"getopendial.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cpcchar		dialers[] = {
	[opendial_unspec] 		= "unspec", 
	[opendial_udp] 			= "udp", 
	[opendial_tcp] 			= "tcp", 
	[opendial_tcpmux] 		= "tcpmux", 
	[opendial_tcpnls] 		= "tcpnls", 
	[opendial_uss] 			= "uss", 
	[opendial_ussmux] 		= "ussmux", 
	[opendial_ussnls] 		= "ussnls", 
	[opendial_ticotsord] 		= "ticotsord",
	[opendial_ticotsordnls] 	= "ticotsordnls", 
	[opendial_pass] 		= "pass", 
	[opendial_open] 		= "open", 
	[opendial_prog] 		= "prog", 
	[opendial_finger] 		= "finger", 
	[opendial_overlast] 		= nullptr
} ; /* end array (dialers) */


/* exported variables */


/* exported subroutines */

int getopendial(cchar *name) noex {
    	int		rs = SR_FAULT ;
	int		v = 0 ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
		rs = SR_NOTFOUND ;
		if ((v = matstr(dialers,name,-1)) >= 0) {
		    rs = SR_OK ;
		} else {
	            if (strcmp(name,"unix") == 0) {
			rs = SR_OK ;
		        v = opendial_uss ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (getopendial) */


