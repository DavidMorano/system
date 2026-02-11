/* uperm HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* link a file as it should be */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UPERM_INCLUDE
#define	UPERM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	u_perm(cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
namespace libu {
    extern int uperm(cchar *,int) noex ;
}
#endif /* __cplusplus */


#endif /* UPERM_INCLUDE */


