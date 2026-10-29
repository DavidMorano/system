/* piaudit SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* audito an index database */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	piaudit

	Description:
	This subroutine processes a single file.

	Synopsis:
	int piaudit(PI *pip,ARGINFO *aip,cchar *t,cc *dbn,char ofn) noex

	Arguments:
	pip		program information pointer
	aip		argument information pointer
	t		terms (character)
	dbn		database name
	ofn		out-file-name

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"config.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr char		aterms[] = {
	0x00, 0x3E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array */


/* exported variables */


/* exported subroutines */

int piaudit(PI *pip,ARGINFO *aip,cchar *terms,cc *dbn,char *ofname) noex {
	int		rs ;
	(void) aip ;
	(void) terms ;
	(void) ofname ;
	rs = auditdb(pip,dbn) ;
	return rs ;
} /* end subroutine (piaudit) */


