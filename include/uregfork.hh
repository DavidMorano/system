/* uregfork HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* At-Fork management */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	UREGFORK_INCLUDE
#define	UREGFORK_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


namespace libu {
    extern int	uregfork_rec(void_f,void_f,void_f) noex ;
    extern int	uregfork_exp(void_f,void_f,void_f) noex ;
    extern int	uregfork_init() noex ;
    extern int	uregfork_fini() noex ;
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* UREGFORK_INCLUDE */


