/* utmpaccenum SUPPORT */
/* lang=C++20 */

/* UTMPACC enumeration feature */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines perform enumeration on UTMPACC entries.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstring>
#include	<usystem.h>
#include	<storeitem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"utmpacc.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef utmpacc_cur	cur ;
typedef utmpacc_ent	ent ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int utmpacc_curbegin(cur *curp) noex {
	int		rs = SR_OK ;
	(void) curp ;
	return rs ;
}

int utmpacc_curend(cur *curp) noex {
	int		rs = SR_OK ;
	(void) curp ;
	return rs ;
}

int utmpacc_curenum(cur *curp,ent *ep,char *ebuf,int elen) noex {
	int		rs = SR_OK ;
	(void) curp ;
	(void) ep ;
	(void) ebuf ;
	(void) elen ;
	return rs ;
}


/* local subroutines */


