/* usupport_ctdec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCTDEC_INCLUDE
#define	USUPPORTCTDEC_INCLUDE


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
    extern int ctdecui(char *,int,uint) noex ;
    extern int ctdecul(char *,int,ulong) noex ;
    extern int ctdecull(char *,int,ulonglong) noex ;
    template<typename T> inline int ctdec(char *,int,T v) noex {
	return 0 ;
    }
    template<> inline int ctdec(char *dp,int dl,uint v) noex {
	return ctdecui(dp,dl,v) ;
    }
    template<> inline int ctdec(char *dp,int dl,ulong v) noex {
	return ctdecul(dp,dl,v) ;
    }
    template<> inline int ctdec(char *dp,int dl,ulonglong v) noex {
	return ctdecull(dp,dl,v) ;
    }
}

#endif /* __cplusplus */


#endif /* USUPPORTCTDEC_INCLUDE */


