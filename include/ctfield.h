/* ctfield INCLUDE */
/* lang=C20 */

/* structure definition for ctfield extraction calls */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	File was originally written.
	
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CTFIELD_INCLUDE
#define	CTFIELD_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#if	(! defined(__EXTENSIONS__)) && (! defined(_KERNEL))
#if	defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)

#ifndef	TYPEDEFS_UNSIGNED
#define	TYPEDEFS_UNSIGNED

#ifndef	TYPEDEF_USHORT
#define	TYPEDEF_USHORT
typedef unsigned short	ushort ;
#endif

#ifndef	TYPEDEF_UINT
#define	TYPEDEF_UINT
typedef unsigned int	uint ;
#endif

#ifndef	TYPEDEF_ULONG
#define	TYPEDEF_ULONG
typedef unsigned long	ulong ;
#endif

#endif /* TYPEDEFS_UNSIGNED */

#endif
#endif


#ifndef	TYPEDEF_UCHAR
#define	TYPEDEF_UCHAR
typedef unsigned char	uchar ;
#endif /* TYPEDEF_UCHAR */


struct ctfield {
	char	*lp ;			/* line pointer */
	char	*fp ;			/* ctfield pointer */
	int	rlen ;			/* line length remaining */
	int	flen ;			/* ctfield length */
	int	term ;			/* terminating character */
} ;


#define	CTFIELD		struct ctfield


#ifdef	__cplusplus
extern "C" {
#endif

extern int field_start(CTFIELD *,const char *,int) noex ;
extern int field_get(CTFIELD *,const unsigned char *) noex ;
extern int field_term(CTFIELD *,const unsigned char *) noex ;
extern int field_sharg(CTFIELD *,const unsigned char *,char *,int) noex ;
extern int field_finish(CTFIELD *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CTFIELD_INCLUDE */


