/* unixfnames MODULE */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* retrieve various UNIX® database file names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	unixfnames

	Description:
	This module contains a structure (UNIXFNAMES) that facilitates
	retrieving various UNIX® database file names.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

module unixfnames ;

const char unixfnames::passwd[] 	= "passwd" ;
const char unixfnames::shadow[]		= "shadow" ;
const char unixfnames::userattr[]	= "user_attr" ;
const char unixfnames::group[]		= "group" ;
const char unixfnames::project[]	= "project" ;
const char unixfnames::shells[]		= "shells" ;
const char unixfnames::protocols[]	= "protocols" ;
const char unixfnames::networks[]	= "networks" ;
const char unixfnames::netmasks[]	= "netmasks" ;
const char unixfnames::hosts[]		= "hosts" ;
const char unixfnames::services[]	= "services" ;

constexpr unixfnames	unixfname ;


