/* usupport_sncpyx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPOETSNCPYX_INCLUDE
#define	USUPPOETSNCPYX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#ifdef	__cplusplus

namespace libu {
    extern int sncpyx(char *,int,int,...) noex ;
    inline int sncpy1(char *dp,int dl,cc *s1) noex {
	return sncpyx(dp,dl,1,s1) ;
    }
    inline int sncpy2(char *dp,int dl,cc *s1,cc *s2) noex {
	return sncpyx(dp,dl,2,s1,s2) ;
    }
    inline int sncpy3(char *dp,int dl,cc *s1,cc *s2,cc *s3) noex {
	return sncpyx(dp,dl,3,s1,s2,s3) ;
    }
    inline int sncpy4(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return sncpyx(dp,dl,4,s1,s2,s3,s4) ;
    }
    inline int sncpy5(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return sncpyx(dp,dl,5,s1,s2,s3,s4,s5) ;
    }
    inline int sncpy6(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,
	    cc *s6) noex {
	return sncpyx(dp,dl,6,s1,s2,s3,s4,s5,s6) ;
    }
    template<typename ... Args>
    inline int sncpy(char *dp,int dl,Args ... args) noex {
        cint            na = npack(Args) ;
        return sncpyx(dp,dl,na,args ...) ;
    }
} /* end namespace (libu) */

#endif /* __cplusplus */


#endif /* USUPPOETSNCPYX_INCLUDE */


