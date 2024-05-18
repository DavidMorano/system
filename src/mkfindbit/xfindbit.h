/* xfindbit HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	XFINDBIT_INCLUDE
#define	XFINDBIT_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int xffbsi(uint) ;
extern int xffbci(uint) ;

extern int xffbsl(ulong) ;
extern int xffbcl(ulong) ;

extern int xflbsi(uint) ;
extern int xflbci(uint) ;

extern int xflbsl(ulong) ;
extern int xflbcl(ulong) ;

extern int xfbscounti(uint) ;
extern int xfbscountl(ulong) ;

#ifdef	__cplusplus
}
#endif

#endif /* XFINDBIT_INCLUDE */


