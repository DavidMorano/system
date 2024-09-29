/* readkeycmd HEADER */
/* lang=C20 */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */


#ifndef	READCMDKEY_INCLUDE
#define	READCMDKEY_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<uterm.h>


#define	READCMDKEY	struct readcmdkey_head


struct readcmdkey_head {
	int	type ;			/* key-type */
	int	inter ;			/* intermediate value (numeric) */
} ;

typedef	READCMDKEY	readcmdkey ;

EXTERNC_begin

extern int	readcmdkey(readcmdkey *,utern *,int,int) noex ;

EXTERNC_end


#endif /* READCMDKEY_INCLUDE */


