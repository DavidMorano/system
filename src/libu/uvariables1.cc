/* uvariables1 SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* this is a database of commonly used global variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uvaraibles

	Description:
	This object contains various commonly used environment
	variable names.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	<varnames.hh>
#include	<syswords.hh>

import uvariables ;

constinit const varnames	varname ;
constinit const syswords	sysword ;
constinit const valuelims	valuelimit ;

const digbufsizes		digbufsize ;


