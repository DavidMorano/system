/* sysdbfnames MODULE */
/* charset=ISO8859-1 */
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
	sysdbfnames

	Description:
	This module contains a structure (UNIXFNAMES) that facilitates
	retrieving various UNIX® database file names.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

module sysdbfnames ;

const char sysdbfnames::passwd[] 	= "passwd" ;
const char sysdbfnames::shadow[]	= "shadow" ;
const char sysdbfnames::userattr[]	= "userattr" ;
const char sysdbfnames::group[]		= "group" ;
const char sysdbfnames::project[]	= "project" ;
const char sysdbfnames::shells[]	= "shells" ;
const char sysdbfnames::protocols[]	= "protocols" ;
const char sysdbfnames::networks[]	= "networks" ;
const char sysdbfnames::netmasks[]	= "netmasks" ;
const char sysdbfnames::hosts[]		= "hosts" ;
const char sysdbfnames::services[]	= "services" ;

constexpr sysdbfnames	sysdbfname ;


