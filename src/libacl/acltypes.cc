/* acltypes SUPPORT */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* ACL types (types of ACLs) */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-02-24, David A­D­ Morano
	This code was adopted from the SHCAT program, which in turn
	had been adopted from programs with a lineage dating back
	(from the previous notes in this space) from 1989!  I deleted
	the long list of notes here to clean this up.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	acltypes

	Description:
	This subroutine performs ACL handling.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* |MAX_ACL_ENTRIES| */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<matxstr.h>
#include	<localmisc.h>

#include	"acltypes.h"


/* local defines */

#ifdef	MAX_ACL_ENTRIES
#define	MAXACLS		MAX_ACL_ENTRIES
#else
#define	MAXACLS		1024
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cpcchar	acltypes[] = {
	"user",
	"group",
	"other",
	"mask",
	"duser",
	"dgroup",
	"dother",
	"dmask",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int getacltype(cchar *tp,int tl) noex {
	return matostr(acltypes,1,tp,tl) ;
}
/* end subroutine (getacltype) */


