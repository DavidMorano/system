/* md5hash HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MD5HASH_INCLUDE
#define	MD5HASH_INCLUDE	


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<md5.h>


#define	MD5HASH		struct md5hash_head


struct md5hash_head {
	MD5_CTS		ctx ;
	uint		magic ;
} ;

typedef	MD5HASH		md5hash ;

EXTERNC_begin

extern int	md5hash_init(MD5HASH *) noex ;
extern int	md5hash_add(MD5HASH *,void *,int) noex ;
extern int	md5hash_get(MD5HASH *,ulong *) noex ;
extern int	md5hash_free(MD5HASH *) noex ;

EXTERNC_end


#endif /* MD5HASH_INCLUDE */


