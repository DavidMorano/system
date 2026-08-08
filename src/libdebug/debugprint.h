/* debugprint HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* debug utilities */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEBUGPRINT_INCLUDE
#define	DEBUGPRINT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdarg.h>		/* CSTD |va_list(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#ifdef	__cplusplus
#define DEBUGPRINTF(FMT, ...)	({ 				\
	int rsdebug = 0 ;					\
        if_constexpr (f_debug) {				\
            rsdebug = debugprintx(__func__, FMT __VA_OPT__(,) __VA_ARGS__) ; \
        } ; rsdebug ; \
    })
#else /* __cplusplus */
#define DEBUGPRINTF(FMT, ...)					\
    debugprintx(__func__,FMT __VA_OPT__(,) __VA_ARGS__)
#endif /* __cplusplus */

#define DEBUGCLOSE(FMT, ...)					\
	debugclose() ;


EXTERNC_begin

extern int debugopen		(cchar *) noex ;
extern int debugclose		(void) noex ;
extern int debugwrite		(cchar *,int) noex ;
extern int debugprint		(cchar *,int) noex ;
extern int debugprintf		(cchar *,...) noex ;
extern int debugprintx		(cchar *,cchar *,...) noex ; /* special */
extern int debugvprintf		(cchar *,va_list) noex ;
extern int debugprintfsize	(cchar *,int) noex ;

extern int debugsetfd		(int) noex ;
extern int debuggetfd		(void) noex ;

extern int debugprinthexblock	(cchar *,int,cvoid *,int) noex ;
extern int debugprintdeci	(cchar *,int) noex ;
extern int debugprinthexi	(cchar *,int) noex ;
extern int debugprintnum	(cchar *,int) noex ;


EXTERNC_end

#ifdef	__cplusplus

inline int debugwrite(cchar *sp) noex {
    	return debugwrite(sp,-1) ;
}
inline int debugprint(cchar *sp) noex {
    	return debugprint(sp,-1) ;
}

#endif /* __cplusplus */


#endif /* DEBUGPRINT_INCLUDE */


