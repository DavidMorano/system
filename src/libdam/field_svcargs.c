/* field_svcargs */
/* lang=C20 */

/* parse a field object into service envelope items */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	field_svcargs

	Description:
	This subroutine will take a 'field' object and creates a
	string-list of service arguments from it.

	Synopsis:
	int field_svcargs(field *fbp,vecstr *sap) noex

	Arguments:
	fbp		pointer to field object
	sap		pointer to VECSTR of arguments

	Returns:
	>=0		OK
	<0		bad

	Notes:
	I note that "service" arguments (at least as we see it at the
	present) are the same as "shell" arguments.  So we call
	|field_sharg()| to get the next "service" argument.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"field.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

int field_svcargs(field *fbp,vecstr *sap) noex {
	int		rs = SR_FAULT;
	int		rs1 ;
	int		c = 0 ;
	if (fbp && sap) {
	    nullptr_t	n = 0 ;
	    if ((rs = field_remaining(fbp,n)) >= 0) {
	        cint	alen = rs ;
	        char	*abuf ;
	        if ((rs = uc_malloc((alen+1),&abuf)) >= 0) {
		    int	al ;
	            while ((al = field_sharg(fbp,n,abuf,alen)) >= 0) {
	                c += 1 ;
	                rs = vecstr_add(sap,abuf,al) ;
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = uc_free(abuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a) */
	    } /* end if (field_remaining) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (field_svcargs) */


