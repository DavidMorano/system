/* sysdbfiles SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* standard file-names */
/* version %I% last-modified %G% */


/* revision history:

= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written for the Ethernet-Controller (ETHCON)
	development work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This modeule provides that c-strings for the standard file
	names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"sysdbfiles.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sysdbfiler {
	sysdbfiler() noex ;
    } ; /* end struct (sysdbfiler) */
} /* end namespace */


/* forward references */


/* local variables */

const sysdbfiler	sysdbfile_data ;


/* exported variables */

ccharp		sysdbfile[sysdbfile_overlast + 1] ;


/* exported subroutines */


/* local subroutines */

sysdbfiler::sysdbfiler() noex {
    	ccharp *n = sysdbfile ;
	n[sysdbfile_pw] 	= "passwd" ;
	n[sysdbfile_sp] 	= "shadow" ;
	n[sysdbfile_ua] 	= "userattr" ;
	n[sysdbfile_gr] 	= "group" ;
	n[sysdbfile_pj] 	= "project" ;
	n[sysdbfile_pr] 	= "protocols" ;
	n[sysdbfile_nw] 	= "networks" ;
	n[sysdbfile_nm] 	= "netmasks" ;
	n[sysdbfile_ho] 	= "hosts" ;
	n[sysdbfile_sv] 	= "services" ;
	n[sysdbfile_sh]		= "shells" ;
	n[sysdbfile_overlast]	= nullptr ;
} /* end ctor (nr) */


