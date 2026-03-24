/* usupport_ustrftime HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRFTIME_INCLUDE
#define	USUPPORTSTRFTIME_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern int ustrftime(char *,int,cchar *,CTM *) noex ;
}


#endif /* __cplusplus */
#endif /* USUPPORTSTRFTIME_INCLUDE */


