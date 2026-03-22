/* usupport_cfdec HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCFEC_INCLUDE
#define	USUPPORTCFEC_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* |nullptr_t| */
#include	<stdlib.h>
#include	<stdint.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<stdintx.h>


namespace libu {
    extern int cfdec(cchar *,int,int *) noex ;
    extern int cfdec(cchar *,int,long *) noex ;
    extern int cfdec(cchar *,int,longlong *) noex ;
    extern int cfdec(cchar *,int,uint *) noex ;
    extern int cfdec(cchar *,int,ulong *) noex ;
    extern int cfdec(cchar *,int,ulonglong *) noex ;
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTCFEC_INCLUDE */


