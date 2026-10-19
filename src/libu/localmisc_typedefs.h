/* localmisc_typedefs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscellaneous pre-processor definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is a collection of miscellaneious pre-processor
	defintions that were eidely used on the various embedded
	code projects and also on many of the support tool projects.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	These are basic scalar types.

*******************************************************************************/

#ifndef	LOCALMISCTYPEDEFS_INCLUDE
#define	LOCALMISCTYPEDEFS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* type |time_t| */


#ifndef	TYPEDEF_SCHAR
#define	TYPEDEF_SCHAR
typedef signed char		schar ;
#endif /* TYPEDEF_SCHAR */

#ifndef	TYPEDEF_UCHAR
#define	TYPEDEF_UCHAR
typedef unsigned char		uchar ;
#endif

#ifndef	TYPEDEF_USHORT
#define	TYPEDEF_USHORT
typedef unsigned short		ushort ;
#endif

#ifndef	TYPEDEF_UINT
#define	TYPEDEF_UINT
typedef unsigned int		uint ;
#endif

#ifndef	TYPEDEF_ULONG
#define	TYPEDEF_ULONG
typedef unsigned long		ulong ;
#endif

#ifndef	TYPEDEF_CSCHAR
#define	TYPEDEF_CSCHAR
typedef const signed char	cschar ;
#endif /* TYPEDEF_SCHAR */

#ifndef	TYPEDEF_CCHAR
#define	TYPEDEF_CCHAR
typedef cchar		cchar ;
#endif /* TYPEDEF_SCHAR */

#ifndef	TYPEDEF_CSHORT
#define	TYPEDEF_CSHORT
typedef const short		cshort ;
#endif /* TYPEDEF_CSHORT */

#ifndef	TYPEDEF_CINT
#define	TYPEDEF_CINT
typedef const int		cint ;
#endif /* TYPEDEF_CINT */

#ifndef	TYPEDEF_CLONG
#define	TYPEDEF_CLONG
typedef const long		clong ;
#endif /* TYPEDEF_CLONG */

#ifndef	TYPEDEF_CUCHAR
#define	TYPEDEF_CUCHAR
typedef const unsigned char	cuchar ;
#endif /* TYPEDEF_UCHAR */

#ifndef	TYPEDEF_CUSHORT
#define	TYPEDEF_CUSHORT
typedef const unsigned short	cushort ;
#endif

#ifndef	TYPEDEF_CUINT
#define	TYPEDEF_CUINT
typedef const unsigned int	cuint ;
#endif

#ifndef	TYPEDEF_CULONG
#define	TYPEDEF_CULONG
typedef const unsigned long	culong ;
#endif

#ifndef	TYPEDEF_USTIME
#define	TYPEDEF_USTIME
typedef time_t			ustime ;
#endif

#ifndef	TYPEDEF_CUSTIME
#define	TYPEDEF_CUSTIME
typedef const time_t		custime ;
#endif

#ifndef	TYPEDEF_CC
#define	TYPEDEF_CC
typedef const char		cc ;
#endif


#endif /* LOCALMISCTYPEDEFS_INCLUDE */


