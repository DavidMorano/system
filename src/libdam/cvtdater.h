/* cvtdater HEADER */
/* lang=C20 */



/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CVTDATER_INCLUDE
#define	CVTDATER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#define	CVTDATER	struct cvtdater_head


struct cvtdater_head {
	time_t		daytime ;
} ;

typedef CVTFATER	cvtdater ;

EXTERNC_begin

extern int	cvtdater_start(CVTDATER *,time_t) noex ;
extern int	cvtdater_load(CVTDATER *,time_t *,cchar *,int) noex ;
extern int	cvtdater_finish(CVTDATER *) noex ;

EXTERNC_end
#

#endif /* CVTDATER_INCLUDE */


