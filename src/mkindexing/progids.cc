/* progids */
/* lang=C20 */

/* manage process IDs */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-09-01, David A­D­ Morano
        This subroutine was borrowed and modified from some previous module.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Manage the general process logging.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"defs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int progids_begin(PROGINFO *pip) noex {
	int		rs = SR_OK ;
	if (! pip->open.ids) {
	    if ((rs = ids_load(&pip->id)) >= 0) {
		pip->open.ids = TRUE ;
	    }
	}
	return rs ;
}
/* end subroutine (progids_begin) */

int progids_end(PROGINFO *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->open.ids) {
	    pip->open.ids = FALSE ;
	    rs1 = ids_release(&pip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (progids_end) */

extern int progids_permid(PROGINFO *pip,USTAT *sbp,int am) noex {
	int		rs ;
	if (pip->open.ids) {
	   rs = permid(&pip->id,sbp,am) ;
	} else {
	    rs = SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (progids_permid) */


