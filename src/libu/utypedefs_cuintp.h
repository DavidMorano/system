/* utypedefs_cuintp HEADER (Integers-UnSigned-Constant) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	UTYPEDEFSCUINTP_INCLUDE
#define	UTYPEDEFSCUINTP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclunsion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_CUCHARP
#define	TYPEDEF_CUCHARP
typedef const unsigned char *	cucharp ;
#endif /* TYPEDEF_CUCHARP */

#ifndef	TYPEDEF_CUSHORTP
#define	TYPEDEF_CUSHORTP
typedef const unsigned short *	cushortp ;
#endif /* TYPEDEF_CUSHORTP */

#ifndef	TYPEDEF_CUINTP
#define	TYPEDEF_CUINTP
typedef const unsigned int *	cuintp ;
#endif /* TYPEDEF_CUINTP */

#ifndef	TYPEDEF_CULONGP
#define	TYPEDEF_CULONGP
typedef const unsigned long *	culongp ;
#endif /* TYPEDEF_CULONGP */


#endif /* UTYPEDEFSCUINTP_INCLUDE */


