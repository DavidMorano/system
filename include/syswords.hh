/* syswords HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* this is a database of commonly used system words */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	syswords

	Description:
	This object contains various commonly used system-related
	words.

*******************************************************************************/

#ifndef	SYSWORDS_INCLUDE
#define	SYSWORDS_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


struct syswords {
	static const char w_export[] ;
	static const char w_path[] ;
	static const char w_fpath[] ;
	static const char w_cdpath[] ;
	static const char w_libpath[] ;
	static const char w_manpath[] ;
	static const char w_incpath[] ;
	static const char w_infopath[] ;
	static const char w_users[] ;
	static const char w_help[] ;
	static const char w_lib[] ;
	static const char w_bindir[] ;
	static const char w_sbindir[] ;
	static const char w_usrdir[] ;
	static const char w_etcdir[] ;
	static const char w_tmpdir[] ;
	static const char w_devdir[] ;
	static const char w_vardir[] ;
	static const char w_procdir[] ;
	static const char w_sysdbdir[] ;
	static const char w_devstdin[] ;
	static const char w_devstdout[] ;
	static const char w_devstderr[] ;
	static const char w_devstdlog[] ;
	static const char w_devnull[] ;
	static const char w_devrandom[] ;
	static const char w_maildir[] ;
	static const char w_usrlocaldir[] ;
	static const char w_vartmpdir[] ;
	static const char w_digtab[] ;
	static const char w_blanks[] ;
	static const char w_defprovider[] ;
	static const char w_localhost[] ;
} ; /* end struct (syswords) */


extern const syswords	sysword ;


#endif	/* __cplusplus */
#endif /* SYSWORDS_INCLUDE */


