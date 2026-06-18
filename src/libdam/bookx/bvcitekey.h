/* bvcitekey HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage BV cite key */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVCITEKEY_INCLUDE
#define	BVCITEKEY_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	BVCITEKEY	struct bvcitekey_head


const uint		bvcitekey_vmask = 0x00FFFFFFU ;

struct bvcitekey_head {
	uchar		nlines ;
	uchar		b ;
	uchar		c ;
	uchar		v ;
 } ; /* end struct */

typedef BVCITEKEY	bvcitekey ;

EXTERNC_begin

extern int	bvcitekey_set(bvcitekey *,uint *) noex ;
extern int	bvcitekey_get(bvcitekey *,uint *) noex ;

EXTERNC_end


#endif /* BVCITEKEY_INCLUDE */


