/* utypedefs_cintp HEADER (Integers-Constant) */
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

#ifndef	UTYPEDEFSCINTP_INCLUDE
#define	UTYPEDEFSCINTP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */

#include	<utypedefs_csintp.h>
#include	<utypedefs_cuintp.h>


#ifndef	TYPEDEF_CCHARP
#define	TYPEDEF_CCHARP
typedef const char *		ccharp ;
#endif /* TYPEDEF_CCHARP */

#ifndef	TYPEDEF_CSHORTP
#define	TYPEDEF_CSHORTP
typedef const short *		cshortp ;
#endif /* TYPEDEF_CSHORP */

#ifndef	TYPEDEF_CINTP
#define	TYPEDEF_CINTP
typedef const int *		cintp ;
#endif /* TYPEDEF_CINTP */

#ifndef	TYPEDEF_CLONGP
#define	TYPEDEF_CLONGP
typedef const long *		clongp ;
#endif /* TYPEDEF_CLONGP */


#endif /* UTYPEDEFSCINTP_INCLUDE */


