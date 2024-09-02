/* piaudit SUPPORT */
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
	int piaudit(PROGINFO *pip,ARGINFO *aip,cchar *t,cc *dbn,char ofn) noex

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

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static constexpr cchar	aterms[] = {
	0x00, 0x3E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int piaudit(PROGINFO *pip,ARGINFO *aip,cchar *terms,cc *dbn,char *ofname) noex {
	int		rs ;
	(void) aip ;
	(void) terms ;
	(void) ofname ;
	rs = auditdb(pip,dbn) ;
	return rs ;
}
/* end subroutine (piaudit) */


