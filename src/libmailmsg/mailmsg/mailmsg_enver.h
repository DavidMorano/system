/* mailmsg_enver HEADER */
/* lang=C++20 */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGENVER_INCLUDE
#define	MAILMSGENVER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<mailmsg.h>


#define	MAILMSG_ENVER		struct mailmsg_enver
#define	MAILMSG_ENVSTR		struct mailmsg_envstr


struct mailmsg_envstr {
	cchar		*ep ;
	int		el ;
} ;

struct mailmsg_enver {
	MAILMSG_ENVSTR	a ;		/* address */
	MAILMSG_ENVSTR	d ;		/* ?? */
	MAILMSG_ENVSTR	r ;		/* route-address */
} ;

typedef MAILMSG_ENVER	mailmsg_enver ;

EXTERNC_begin

extern int	mailmsg_enver(mailmsg *,int,mailmsg_enver *) noex ;

EXTERNC_end


#endif /* MAILMSGENVER_INCLUDE */


