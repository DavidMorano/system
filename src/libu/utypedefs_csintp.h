/* utypedefs_csintp HEADER (Integers-Signed-Constant) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UTYPEDEFSCSINTP_INCLUDE
#define	UTYPEDEFSCSINTP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_CSCHARP
#define	TYPEDEF_CSCHARP
typedef const signed char *	cscharp ;
#endif /* TYPEDEF_CSCHARP */

#ifndef	TYPEDEF_CSSHORTP
#define	TYPEDEF_CSSHORTP
typedef const signed short *	csshortp ;
#endif /* TYPEDEF_CSSHORTP */

#ifndef	TYPEDEF_CSINTP
#define	TYPEDEF_CSINTP
typedef const signed int *	csintp ;
#endif /* TYPEDEF_CSINTP */

#ifndef	TYPEDEF_CSLONGP
#define	TYPEDEF_CSLONGP
typedef const signed long *	cslongp ;
#endif /* TYPEDEF_CSLONGP */


#endif /* UTYPEDEFSCSINTP_INCLUDE */


