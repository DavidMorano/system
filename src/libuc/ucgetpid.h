/* ucgetpid HEADER */
/* lang=C20 */

/* slightly tuned UNIX® look-alike for |getpid(2)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCGETPID_INCLUDE
#define	UCGETPID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>

#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_getpid() noex ;
extern void	uc_setpid(pid_t) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCGETPID_INCLUDE */


