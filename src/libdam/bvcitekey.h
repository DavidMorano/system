/* bvcitekey HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVCITEKEY_INCLUDE
#define	BVCITEKEY_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	BVCITEKEY		struct bvcitekey_head


struct bvcitekey_head {
	uchar		nlines ;
	uchar		b ;
	uchar		c ;
	uchar		v ;
 } ;

typedef BVCITEKEY	bvcitekey ;

EXTERNC_begin

extern int	bvcitekey_set(bvcitekey *,uint *) noex ;
extern int	bvcitekey_get(bvcitekey *,uint *) noex ;

EXTERNC_end


#endif /* BVCITEKEY_INCLUDE */


