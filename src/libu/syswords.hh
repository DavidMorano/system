/* syswords HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


struct syswords {
	static cchar w_export[] ;
	static cchar w_path[] ;
	static cchar w_fpath[] ;
	static cchar w_cdpath[] ;
	static cchar w_libpath[] ;
	static cchar w_manpath[] ;
	static cchar w_incpath[] ;
	static cchar w_infopath[] ;
	static cchar w_etc[] ;
	static cchar w_lib[] ;
	static cchar w_man[] ;
	static cchar w_var[] ;
	static cchar w_share[] ;
	static cchar w_info[] ;
	static cchar w_help[] ;
	static cchar w_users[] ;
	static cchar w_bindir[] ;
	static cchar w_sbindir[] ;
	static cchar w_usrdir[] ;
	static cchar w_etcdir[] ;
	static cchar w_tmpdir[] ;
	static cchar w_devdir[] ;
	static cchar w_vardir[] ;
	static cchar w_procdir[] ;
	static cchar w_sysdbdir[] ;
	static cchar w_devstdin[] ;
	static cchar w_devstdout[] ;
	static cchar w_devstderr[] ;
	static cchar w_devstdlog[] ;
	static cchar w_devnull[] ;
	static cchar w_devzero[] ;
	static cchar w_devrandom[] ;
	static cchar w_maildir[] ;
	static cchar w_vartmpdir[] ;
	static cchar w_usrlocaldir[] ;
	static cchar w_usrextradir[] ;
	static cchar w_blanks[] ;
	static cchar w_defprovider[] ;
	static cchar w_localhost[] ;
	static cchar w_mailhost[]	;
	static cchar w_loghost[] ;
} ; /* end struct (syswords) */


#endif /* __cplusplus (C++ only) */
#endif /* SYSWORDS_INCLUDE */


