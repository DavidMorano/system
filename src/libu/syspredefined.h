/* syspredfined HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* predefined variables in the various operating systems */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSPREDEFINED_INCLUDE
#define	SYSPREDEFINED_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extened integer types */


/* PREDEFINED start */
/* determine if some unsigned-related typedefs have already been made */
/* the following unsigned typedefs are supposedly "System V" compatibility */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)

#ifndef	TYPEDEF_USHORT
#define	TYPEDEF_USHORT
/* already typedef'ed on Darwin */
#endif

#ifndef	TYPEDEF_UINT
#define	TYPEDEF_UINT
/* already typedef'ed on Darwin */
#endif

#endif /* !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE) */
#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* PREDEFINED end */

/* PREDEFINED start */
/* determine if some unsigned-related typedefs have already been made */
/* the following unsigned typedefs are supposedly "System V" compatibility */
#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
#if !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__)

#ifndef	TYPEDEF_USHORT
#define	TYPEDEF_USHORT
/* already typedef'ed on Solaris® */
#endif

#ifndef	TYPEDEF_UINT
#define	TYPEDEF_UINT
/* already typedef'ed on Solaris® */
#endif

#ifndef	TYPEDEF_ULONG
#define	TYPEDEF_ULONG
/* already typedef'ed on Solaris® */
#endif

#endif /* !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__) */
#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */
/* PREDEFINED end */


#endif /* SYSPREDEFINED_INCLUDE */


