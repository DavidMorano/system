/* opendial HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Open-Dial */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENDIAL_INCLUDE
#define	OPENDIAL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<opendials.h>		/* LIBUC */


enum opendialopts {
	opendialopt_long,
	opendialopt_overlast
} ; /* end enum (opendialopts) */

#ifdef	__cplusplus
struct opendialoptms {
    	static constexpr int	mlong		= (1 << opendialopt_long) ;
} ; /* end struct (opendialoptms) */
#endif /* __cplusplus */

/* options */
#define	OPENDIAL_OLONG		(1 << opendialopt_long)	/* FINGER "long" */

EXTERNC_begin

extern int opendial(int,int,cc *,cc *,cc *,con mainv,con mainv,int,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const opendialoptms	optdialoptm ;
#endif /* __cplusplus */


#endif /* OPENDIAL_INCLUDE */


