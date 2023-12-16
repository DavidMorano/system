/* varnames INCLUDE */
/* lang=C++20 */

/* this is a database of commonly used environment variable names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 2001 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This object contains various commonly used environment variable
	names.

*******************************************************************************/

#ifndef	VARNAMES_INCLUDE
#define	VARNAMES_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>


struct varnames {
	static const char logname[] ;
	static const char username[] ;
	static const char groupname[] ;
	static const char user[] ;
	static const char group[] ;
	static const char cdpath[] ;
	static const char path[] ;
	static const char fpath[] ;
	static const char binpath[] ;
	static const char libpath[] ;
	static const char incpath[] ;
	static const char manpath[] ;
	static const char infopath[] ;
	static const char tz[] ;
	static const char architecture[] ;
	static const char version[] ;
	static const char release[] ;
	static const char sysname[] ;
	static const char osname[] ;
	static const char osnum[] ;
	static const char osrel[] ;
	static const char ostype[] ;
	static const char osvers[] ;
	static const char mail[] ;
	static const char fpath[] ;
	static const char node[] ;
	static const char domain[] ;
	static const char editor[] ;
	static const char visual[] ;
	static const char lang[] ;
	static const char shell[] ;
	static const char shlvl[] ;
	static const char home[] ;
	static const char pwd[] ;
	static const char display[] ;
	static const char term[] ;
	static const char tmout[] ;
	static const char tmpdir[] ;
	static const char uucppublic[] ;
	static const char lines[] ;
	static const char columns[] ;
	static const char digtab[] ;
} ; /* end struct (varnames) */


extern const varnames	varname ;


#endif	/* __cplusplus */
#endif /* VARNAMES_INCLUDE */


