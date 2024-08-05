/* svckv HEADER */
/* lang=C20 */

/* these subroutines perform key-value type functions */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-10-13, David A­D­ Morano
	This was split out of the HOMEPAGE program (where it was
	originally local).

*/

/* Copyright © 2018 David A­D­ Morano.  All rights reserved. */

#ifndef	SVCKV_INCLUDE
#define	SVCKV_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	svckv_val(cchar *(*)[2],int,cchar *,cchar **) noex ;
extern int	svckv_dequote(cchar *(*)[2],int,cchar *,cchar **) noex ;
extern int	svckv_isfile(cchar *(*)[2],int,cchar **) noex ;
extern int	svckv_ispass(cchar *(*)[2],int,cchar **) noex ;
extern int	svckv_islib(cchar *(*)[2],int,cchar **) noex ;
extern int	svckv_isprog(cchar *(*)[2],int,cchar **) noex ;
extern int	svckv_svcopts(cchar *(*)[2],int) noex ;

EXTERNC_end


#endif /* SVCKV_INCLUDE */


