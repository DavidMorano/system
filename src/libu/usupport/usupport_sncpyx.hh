/* usupport_sncpyx HEADER */
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

#ifndef	USUPPORTSNCPYX_INCLUDE
#define	USUPPORTSNCPYX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace libu {
    extern int sncpyx(char *,int,int,...) noex ;
    inline int sncpy1(char *dp,int dl,cc *s1) noex {
	return libu::sncpyx(dp,dl,1,s1) ;
    }
    inline int sncpy2(char *dp,int dl,cc *s1,cc *s2) noex {
	return libu::sncpyx(dp,dl,2,s1,s2) ;
    }
    inline int sncpy3(char *dp,int dl,cc *s1,cc *s2,cc *s3) noex {
	return libu::sncpyx(dp,dl,3,s1,s2,s3) ;
    }
    inline int sncpy4(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return libu::sncpyx(dp,dl,4,s1,s2,s3,s4) ;
    }
    inline int sncpy5(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,
	    cc *s5) noex {
	return libu::sncpyx(dp,dl,5,s1,s2,s3,s4,s5) ;
    }
    inline int sncpy6(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,
	    cc *s6) noex {
	return libu::sncpyx(dp,dl,6,s1,s2,s3,s4,s5,s6) ;
    }
    template<typename ... Args>
    inline int sncpy(char *dp,int dl,Args ... args) noex {
        cint	na = npack(Args) ;
        return libu::sncpyx(dp,dl,na,args ...) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSNCPYX_INCLUDE */


