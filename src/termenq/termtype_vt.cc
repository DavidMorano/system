/* termtype_ct SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* discriminate among VT1xx models */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

/* revision history:

	= 2000-07-19, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

	= 2017-12-16, David A­D­ Morano
	Updated.

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:
	Discrminate among VTxx0 models.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sncpyx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"termtype.h"


/* local defines */

#define	U	SHORT_MIN


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr termtype	types[] = {
	{ "vt100",	{ 1, 2, U, U },		{ -83, U, U, U } },
	{ "vt101",	{ 1, 0, U, U },		{ U, U, U, U } },
	{ "vt102",	{ 6, U, U, U },		{ U, U, U, U } },
	{ "vt220",	{ 62, -9, U, U },	{ U, U, U, U } },
	{ "vt220int",	{ 62, 9, U, U },	{ U, U, U, U } },
	{ "vt320",	{ 63, -9, U, U },	{ U, U, U, U } },
	{ "vt320int",	{ 63, 9, U, U },	{ U, U, U, U } },
	{ "vt420",	{ 64, U, U, U },	{ U, U, U, U } },
	{ "vt520",	{ 65, -22, U, U },	{ 64, 0, U, U } },
	{ "vt520pc",	{ 65, -22, U, U },	{ 64, 1, U, U } },
	{ "vt525",	{ 65, 22, U, U },	{ 65, 0, U, U } },
	{ "vt525pc",	{ 65, 22, U, U },	{ 65, 1, U, U } },
	{ "screen",	{ 1, 2, U, U },		{ 83, 9999, 0, U } },
	{ nullptr,		{ 0, 0, 0, 0 },		{ 0, 0, 0, 0 } }
} ; /* end array */


/* exported variables */


/* exported subroutines */

int termtype_vt(char *rbuf,int rlen,cshort *pvp,cshort *svp) noex {
	int		rs = SR_OK ;
	int		si ;
	if (rbuf == nullptr) return SR_FAULT ;
	if (pvp == nullptr) return SR_FAULT ;
	if (svp == nullptr) return SR_FAULT ;
	rbuf[0] = 0 ;
	if ((si = termtype(types,pvp,svp)) >= 0) {
	    rs = sncpy1(rbuf,rlen,types[si].name) ;
	}
	return rs ;
} /* end subroutine (termtype_vt) */


