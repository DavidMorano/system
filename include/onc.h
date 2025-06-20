/* onc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* ONC operations */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	ONC_INCLUDE
#define	ONC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int onckeyalready(cchar *) noex ;
extern int onckeygetset(cchar *,cchar *) noex ;
extern int onckeylogin(cchar *) noex ;

EXTERNC_end


#endif /* ONC_INCLUDE */


