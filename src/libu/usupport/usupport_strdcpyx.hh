/* usupport_strdcpyx HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* USUPPORT string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRDCPYX_INCLUDE
#define	USUPPORTSTRDCPYX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace libu {
    extern char *strdcpyx(char *,int,int,...) noex ;
} /* end namespace */

namespace libu {
    extern char *strdcpy1(char *,int,cc *) noex ;
    extern char *strdcpy2(char *,int,cc *,cc *) noex ;
    extern char *strdcpy3(char *,int,cc *,cc *,cc *) noex ;
    extern char *strdcpy4(char *,int,cc *,cc *,cc *,cc *) noex ;
    extern char *strdcpy5(char *,int,cc *,cc *,cc *,cc *,cc *) noex ;
    extern char *strdcpy6(char *,int,cc *,cc *,cc *,cc *,cc *,cc *) noex ;
} /* end namespace */

namespace libu {
    template<typename ... Args>
    inline char *strdcpy(char *dp,int dl,Args ... args) noex {
	cint	na = npack(Args) ;
	return libu::strdcpyx(dp,dl,na,args ...) ;
    } /* end subroutine */
} /* end namespace */


#endif /* __cplusplus */
#endif /* USUPPORTSTRDCPYX_INCLUDE */


