/* uccontext HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ucontext.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

EXTERNC_begin

    typedef int (*context_f)(int) noex ;

extern 
    int uc_contextmake(ucontext_t *,context_f,int) noex ;
extern 
    int uc_contextswap(ucontext_t *,const ucontext_t *) noex ;
extern 
    int uc_contextset(ucontext_t *) noex ;

EXTERNC_end


