/* usupport_strwcpy HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRWCPY_INCLUDE
#define	USUPPORTSTRWCPY_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern char *strwcpy(char *,cchar *,int = -1) noex ;
}


#endif /* __cplusplus */
#endif /* USUPPORTSTRWCPY_INCLUDE */


