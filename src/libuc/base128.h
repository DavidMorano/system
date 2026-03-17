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
#include	<usysbase.h>


#define	BASE128_STAGELEN	7	/* staging buffer length */
#define	BASE128_OUTLEN		8	/* output buffer length */


EXTERNC_begin

extern int	base128_e(cchar *,int,char *) noex ;
extern int	base128_d(cchar *,int,char *) noex ;
extern int	base128_enc(int) noex ;
extern int	base128_dec(int) noex ;

EXTERNC_end


#endif /* BASE128_INCLUDE */


