/* uatfork HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* record and expunge fork hanlers */
/* version %I% last-modified %G% */


/* Copyright © 1998,2014 David A­D­ Morano.  All rights reserved. */

#ifndef	UATFORK_INCLUDE
#define	UATFORK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int uatfork_init() noex ;
extern int uatfork_fini() noex ;
extern int uatfork_trackbegin() noex ;
extern int u_atforkrec(void_f,void_f,void_f) noex ;
extern int u_atforkexp(void_f,void_f,void_f) noex ;

EXTERNC_end

#endif /* UATFORK_INCLUDE */


