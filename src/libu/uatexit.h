/* uatexit HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* record and expunge fork hanlers */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UATEXIT_INCLUDE
#define	UATEXIT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#ifndef	ATEXITF_SUBROUTINE
#define	ATEXITF_SUBROUTINE
EXTERNC_begin
typedef void	(*atexit_f)(void) noex ;
EXTERNC_end
#endif /* ATEXITF_SUBROUTINE */


EXTERNC_begin

extern int u_atexit(atexit_f f) noex ;

EXTERNC_end


#endif /* UATEXIT_INCLUDE */


