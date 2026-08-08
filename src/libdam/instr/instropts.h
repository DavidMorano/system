/* instropts HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* in-place c-string manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-05-14, David A­D­ Morano
	This was originally written for a UNIX-based hardware 
	support tool.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

#ifndef	INSTROPTS_INCLUDE
#define	INSTROPTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


enum instropts {
    	instropt_subnul,
    	instropt_subbad,
    	instropt_overlast
} ; /* end enum (instropts) */

#ifdef	__cplusplus
struct instroptms {
    	static constexpr int	subnul	= (1 << instropt_subnul) ;
    	static constexpr int	subbad	= (1 << instropt_subbad) ;
} ; /* end struct (instroptms) */
extern const instroptms		instroptm ;
#endif /* __cplusplus */

#define	INSTROPTM_NONE			(0)
#define	INSTROPTM_SUBNUL		(1 << instropt_subnul)
#define	INSTROPTM_SUBBAD		(1 << instropt_subbad)


#endif /* INSTROPTS_INCLUDE */


