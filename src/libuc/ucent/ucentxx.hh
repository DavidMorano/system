/* ucentxx HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UCENT object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCENTXX_INCLUDE
#define	UCENTXX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sbuf.h>
#include	<storeitem.h>
#include	<vechand.h>


namespace ucent {
    int sbuf_fmtstr(sbuf *,cchar *) noex ;
    int si_loadnames(storeitem *,vechand *,cchar *,int) noex ;
    int si_loadname(storeitem *,vechand *,cchar *,int) noex ;
    int si_copystr(storeitem *,char **,cchar *) noex ;
}


#endif /* __cplusplus */
#endif /* UCENTXX_INCLUDE */


