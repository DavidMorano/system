/* usupport_itimer HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTITIMER_INCLUDE
#define	USUPPORTITIMER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#ifndef	__cplusplus

namespace libu {
    extern int uitimer_get(int,ITIMERVAL *) noex ;
    extern int uitimer_set(int,CITIMERVAL *,ITIMERVAL *) noex ;
}

#endif /* __cplusplus */


#endif /* USUPPORTITIMER_INCLUDE */


