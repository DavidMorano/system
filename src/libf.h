/* libf HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* library of additional STDIO functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LIBF_INCLUDE
#define	LIBF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdio.h>		/* CSTD |FILE(3stdio)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	<freadln.h>

EXTERNC_begin

extern int	fprint(FILE *,cchar *,int) noex ;
extern int	fputch(FILE *,int) noex ;
extern int	fwriter(FILE *,cvoid *,int) noex ;

extern FILE *fopenroot(cchar *,cchar *,
			char * = nullptr,cchar * = nullptr) noex ;

local inline int fprintln(FILE *fp,cchar *lp,int ll) noex {
    	return fprint(fp,lp,ll) ;
} /* end subroutine */

EXTERNC_end

/* the following subroutine are C++ overloaded */
#ifndef	__cplusplus

extern int	fwrite(FILE *,cvoid *,int) noex ;

#endif /* __cplusplus */


#endif /* LIBF_INCLUDE */


