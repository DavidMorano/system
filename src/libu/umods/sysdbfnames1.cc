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

cchar sysdbfnames::passwd[] 	= "passwd" ;
cchar sysdbfnames::shadow[]	= "shadow" ;
cchar sysdbfnames::userattr[]	= "userattr" ;
cchar sysdbfnames::group[]		= "group" ;
cchar sysdbfnames::project[]	= "project" ;
cchar sysdbfnames::shells[]	= "shells" ;
cchar sysdbfnames::protocols[]	= "protocols" ;
cchar sysdbfnames::networks[]	= "networks" ;
cchar sysdbfnames::netmasks[]	= "netmasks" ;
cchar sysdbfnames::hosts[]		= "hosts" ;
cchar sysdbfnames::services[]	= "services" ;

constexpr sysdbfnames	sysdbfname ;


