/* utypedefs_msints HEADER (Integer-Signed-Mutable) */
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

#ifndef	UTYPEDEFSMSINTS_INCLUDE
#define	UTYPEDEFSMSINTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_SCHAR
#define	TYPEDEF_SCHAR
typedef signed char		schar ;
#endif /* TYPEDEF_SCHAR */

#ifndef	TYPEDEF_SSHORT
#define	TYPEDEF_SSHORT
typedef signed short		sshort ;
#endif /* TYPEDEF_SSHORT */

#ifndef	TYPEDEF_SINT
#define	TYPEDEF_SINT
typedef signed int		sint ;
#endif /* TYPEDEF_SINT */

#ifndef	TYPEDEF_SLONG
#define	TYPEDEF_SLONG
typedef signed long		slong ;
#endif /* TYPEDEF_SLONG */


#endif /* UTYPEDEFSMSINTS_INCLUDE */


