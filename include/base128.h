/* base128 HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perform BASE128 encoding and decoding */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BASE128_INCLUDE
#define	BASE128_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>		/* |base128_d| can return an SR */


EXTERNC_begin

extern int	base128_e(cchar *,int,char *) noex ;
extern int	base128_d(cchar *,int,char *) noex ;
extern int	base128_enc(int) noex ;
extern int	base128_dec(int) noex ;

EXTERNC_end


#endif /* BASE128_INCLUDE */


