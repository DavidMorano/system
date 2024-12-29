/* cvtdater HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CVTDATER_INCLUDE
#define	CVTDATER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	CVTDATER	struct cvtdater_head


struct cvtdater_head {
	time_t		daytime ;
} ;

typedef CVTDATER	cvtdater ;

EXTERNC_begin

extern int	cvtdater_start(cvtdater *,time_t) noex ;
extern int	cvtdater_load(cvtdater *,time_t *,cchar *,int) noex ;
extern int	cvtdater_finish(cvtdater *) noex ;

EXTERNC_end

#endif /* CVTDATER_INCLUDE */


