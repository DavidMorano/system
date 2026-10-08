/* varnames HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this is a database of commonly used environment variable names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	varnames

	Description:
	This object contains various commonly used environment variable
	names.

*******************************************************************************/

#ifndef	VARNAMES_INCLUDE
#define	VARNAMES_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


struct varnames {
	static cchar logid[] ;
	static cchar logname[] ;
	static cchar logline[] ;
	static cchar loghost[] ;
	static cchar utmpid[] ;
	static cchar utmpname[] ;
	static cchar utmpline[] ;
	static cchar utmphost[] ;
	static cchar username[] ;
	static cchar groupname[] ;
	static cchar projname[] ;
	static cchar user[] ;
	static cchar group[] ;
	static cchar cdpath[] ;
	static cchar path[] ;
	static cchar fpath[] ;
	static cchar binpath[] ;
	static cchar libpath[] ;
	static cchar incpath[] ;
	static cchar manpath[] ;
	static cchar manxpath[] ;
	static cchar infopath[] ;
	static cchar architecture[] ;
	static cchar sysname[] ;
	static cchar release[] ;
	static cchar version[] ;
	static cchar machine[] ;
	static cchar osname[] ;
	static cchar ostype[] ;
	static cchar osrelease[] ;
	static cchar osnum[] ;
	static cchar osrel[] ;
	static cchar osvers[] ;
	static cchar mail[] ;
	static cchar node[] ;
	static cchar cluster[] ;
	static cchar domain[] ;
	static cchar localdomain[] ;
	static cchar lang[] ;
	static cchar shell[] ;
	static cchar shlvl[] ;
	static cchar home[] ;
	static cchar pwd[] ;
	static cchar tmpdir[] ;
	static cchar maildir[] ;
	static cchar maildirs[] ;
	static cchar mailhost[] ;
	static cchar uucppublic[] ;
	static cchar lines[] ;
	static cchar columns[] ;
	static cchar display[] ;
	static cchar term[] ;
	static cchar termprogram[] ;
	static cchar termdev[] ;
	static cchar tz[] ;
	static cchar printer[] ;
	static cchar printerbin[] ;
	static cchar pager[] ;
	static cchar organization[] ;
	static cchar orgloc[] ;
	static cchar orgcode[] ;
	static cchar office[] ;
	static cchar name[] ;
	static cchar fullname[] ;
	static cchar mailname[] ;
	static cchar tmout[] ;
	static cchar editor[] ;
	static cchar visual[] ;
	static cchar random[] ;
	static cchar seconds[] ;
	static cchar hz[] ;
	static cchar ncpu[] ;
	static cchar nisdomain[] ;
	static cchar systat[] ;
	static cchar netload[] ;
	static cchar provider[] ;
	static cchar folder[] ;
} ; /* end struct (varnames) */


#endif /* __cplusplus (C++ only) */
#endif /* VARNAMES_INCLUDE */


