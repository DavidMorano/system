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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

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
	sysdbfile[sysdbfile_pw] 	= "passwd" ;
	sysdbfile[sysdbfile_sp] 	= "shadow" ;
	sysdbfile[sysdbfile_ua] 	= "userattr" ;
	sysdbfile[sysdbfile_gr] 	= "group" ;
	sysdbfile[sysdbfile_pj] 	= "project" ;
	sysdbfile[sysdbfile_pr] 	= "protocols" ;
	sysdbfile[sysdbfile_nw] 	= "networks" ;
	sysdbfile[sysdbfile_nm] 	= "netmasks" ;
	sysdbfile[sysdbfile_ho] 	= "hosts" ;
	sysdbfile[sysdbfile_sv] 	= "services" ;
	sysdbfile[sysdbfile_sh]		= "shells" ;
	sysdbfile[sysdbfile_overlast]	= nullptr ;
} /* end ctor (sysdbfiler) */


