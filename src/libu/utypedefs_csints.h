/* utypedefs_csints HEADER (Integers-Signed-Constant) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UTYPEDEFSCSINTS_INCLUDE
#define	UTYPEDEFSCSINTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_CSCHAR
#define	TYPEDEF_CSCHAR
typedef const signed char	cschar ;
#endif /* TYPEDEF_CSCHAR */

#ifndef	TYPEDEF_CSSHORT
#define	TYPEDEF_CSSHORT
typedef const signed short	csshort ;
#endif /* TYPEDEF_CSSHORT */

#ifndef	TYPEDEF_CSINT
#define	TYPEDEF_CSINT
typedef const signed int	csint ;
#endif /* TYPEDEF_CSINT */

#ifndef	TYPEDEF_CSLONG
#define	TYPEDEF_CSLONG
typedef const signed long	cslong ;
#endif /* TYPEDEF_CSLONG */


#endif /* UTYPEDEFSCSINTS_INCLUDE */


