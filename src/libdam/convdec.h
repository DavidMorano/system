/* convdec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a long (signed or unsigned) integer to its decimal string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONVDEC_INCLUDE
#define	CONVDEC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char	*convdecsi	(sint,		char *) noex ;
extern char	*convdecsl	(slong,		char *) noex ;
extern char	*convdecsll	(slonglong,	char *) noex ;

extern char	*convdecui	(uint,		char *) noex ;
extern char	*convdecul	(ulong,		char *) noex ;
extern char	*convdecull	(ulonglong,	char *) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline char	*convdec(sint		sv,char *endp) noex {
    	return convdecsi(sv,endp) ;
} /* end */
inline char	*convdec(slong		sv,char *endp) noex {
    	return convdecsl(sv,endp) ;
} /* end */
inline char	*convdec(slonglong	sv,char *endp) noex {
    	return convdecsll(sv,endp) ;
} /* end */
inline char	*convdec(uint		uv,char *endp) noex {
    	return convdecui(uv,endp) ;
} /* end */
inline char	*convdec(ulong		uv,char *endp) noex {
    	return convdecul(uv,endp) ;
} /* end */
inline char	*convdec(ulonglong	uv,char *endp) noex {
    	return convdecull(uv,endp) ;
} /* end */
#endif /* __cplusplus */


#endif /* CONVDEC_INCLUDE */


