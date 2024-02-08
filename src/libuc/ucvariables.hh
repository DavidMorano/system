/* ucvariables HEADER */
/* lang=C++20 */

/* this is a database of commonly used environment variable names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object contains various commonly used environment variable
	names.

*******************************************************************************/

#ifndef	UCVARIABLES_INCLUDE
#define	UCVARIABLES_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<varnames.hh>
#include	<syswords.hh>
#include	<valuelimits.hh>
#include	<digbufsizes.hh>


extern const varnames		varname ;
extern const valuelimits	valuelimit ;
extern const digbufsizes	digbufsize ;


#endif	/* __cplusplus */
#endif /* UCVARIABLES_INCLUDE */


