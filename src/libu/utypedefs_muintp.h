/* utypedefs_muintp HEADER (Integer-UnSigned-Mutable) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	UTYPEDEFSMUINTP_INCLUDE
#define	UTYPEDEFSMUINTP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_UCHARP
#define	TYPEDEF_UCHARP
typedef unsigned char *		ucharp ;
#endif /* TYPEDEF_UCHARP */

#ifndef	TYPEDEF_USHORTP
#define	TYPEDEF_USHORTP
typedef unsigned short *	ushortp ;
#endif /* TYPEDEF_USHORTP */

#ifndef	TYPEDEF_UINTP
#define	TYPEDEF_UINTP
typedef unsigned int *		uintp ;
#endif /* TYPEDEF_UINTP */

#ifndef	TYPEDEF_ULONGP
#define	TYPEDEF_ULONGP
typedef unsigned long *		ulongp ;
#endif /* TYPEDEF_ULONGP */


#endif /* UTYPEDEFSMUINTP_INCLUDE */


