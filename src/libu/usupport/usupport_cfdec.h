/* usupport_cfdec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCFEC_INCLUDE
#define	USUPPORTCFEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<stdint.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<stdintx.h>


#ifdef	__cplusplus

namespace libu {
    extern int cfdec(cchar *,int,int *) noex ;
    extern int cfdec(cchar *,int,long *) noex ;
    extern int cfdec(cchar *,int,longlong *) noex ;
}

#endif /* __cplusplus */


#endif /* USUPPORTCFEC_INCLUDE */


