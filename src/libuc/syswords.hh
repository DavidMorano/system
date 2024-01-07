/* syswords HEADER */
/* lang=C++20 */

/* this is a database of commonly used system words */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object contains various commonly used system-related
	words.

*******************************************************************************/

#ifndef	SYSWORDS_INCLUDE
#define	SYSWORDS_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>


struct syswords {
	static const char w_export[] ;
	static const char w_path[] ;
	static const char w_fpath[] ;
	static const char w_cdpath[] ;
	static const char w_libpath[] ;
	static const char w_manpath[] ;
	static const char w_incpath[] ;
	static const char w_infopath[] ;
} ; /* end struct (syswords) */


extern const syswords	sysword ;


#endif	/* __cplusplus */
#endif /* SYSWORDS_INCLUDE */


