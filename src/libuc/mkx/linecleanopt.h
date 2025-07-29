/* linecleanopt HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* line-cleaning options */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LINECLEANOPT_INCLUDE
#define	LINECLEANOPT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


enum linecleanoptos {
    	linecleanopto_nonsub,		/* non-known -> substitute */
    	linecleanopto_nulsub,		/* NUL -> substitute */
    	linecleanopto_nulign,		/* NUL -> continue (ignore) */
    	linecleanopto_overlast
} ; /* end enum (linecleanoptos) */

#ifdef	__cplusplus
struct linecleanoptms {
    	static cint	nonsub ;
    	static cint	nulsub ;
    	static cint	nulign ;
} ; /* end struct (linecleanoptms) */
#endif /* __cplusplus */

#define	LINECLEANOPT_MNONSUB	(1 << linecleanopto_nonsub)
#define	LINECLEANOPT_MNULSUB	(1 << linecleanopto_nulsub)
#define	LINECLEANOPT_MNULIGN	(1 << linecleanopto_nulign)


#ifdef	__cplusplus

extern const linecleanoptms	linecleanoptm ;

#endif /* __cplusplus */


#endif /* LINECLEANOPT_INCLUDE */


