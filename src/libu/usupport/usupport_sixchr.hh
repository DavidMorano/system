/* usupport_sixchr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSIXCHR_INCLUDE
#define	USUPPORTSIXCHR_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern int	siochr(cchar *,int,int) noex ;
    extern int	sirchr(cchar *,int,int) noex ;
    inline int	sichr(cchar *sp,int sl,int sch) noex {
	return libu::siochr(sp,sl,sch) ;
    }
    inline int	sichr(cchar *sp,int sch) noex {
	return libu::siochr(sp,-1,sch) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSIXCHR_INCLUDE */


