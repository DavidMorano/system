/* usupport_prime HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTPRIME_INCLUDE
#define	USUPPORTPRIME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<time.h>		/* |time_t| */
#include	<string.h>		/* |memset(3c)| + |memcpy(3c)| */
#include	<stdint.h>
#include	<clanguage.h>
#include	<usysbase.h>


#ifndef	SUBROUTINE_MSLEEP
#define	SUBROUTINE_MSLEEP
EXTERNC_begin
extern int	msleep(int) noex ;
EXTERNC_end
#endif

#ifndef	TYPEDEF_MTIME
#define	TYPEDEF_MTIME
typedef int64_t	mtime_t ;
#endif

#ifndef	SUBROUTINE_UMTIME
#define	SUBROUTINE_UMTIME
EXTERNC_begin
extern mtime_t	umtime(void) noex ;
EXTERNC_end
#endif


#endif /* USUPPORTPRIME_INCLUDE */


