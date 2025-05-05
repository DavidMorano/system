/* base64 HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* perform BASE64 encoding and decoding */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BASE64_INCLUDE
#define	BASE64_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>		/* |base64_d| can return an SR */


extern cuchar	base64_et[] ;

EXTERNC_begin

extern int	base64_e(cchar *,int,char *) noex ;
extern int	base64_d(cchar *,int,char *) noex ;
extern int	base64_enc(int) noex ;
extern int	base64_dec(int) noex ;

EXTERNC_end


#endif /* BASE64_INCLUDE */


