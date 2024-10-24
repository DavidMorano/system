/* ucsysmisc HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* slightly tuned UNIX® look-alike for |getpid(2)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCSYSMISC_INCLUDE
#define	UCSYSMISC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	uc_nprocessors(int) noex ;
extern int	uc_gethz(int) noex ;
extern int	uc_syspages(int) noex ;
extern int	uc_pagesize() noex ;

EXTERNC_end

struct ucpagesizer {
    	int		pagesize ;
    	operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ; /* end struct (ucpagesizer) */

extern ucpagesizer	ucpagesize ;


#endif /* UCSYSMISC_INCLUDE */


