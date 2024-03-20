/* mailmsgmatenv HEADER */
/* lang=C++20 */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGMATENV_INCLUDE
#define	MAILMSGMATENV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* object defines */
#define	MAILMSGENV		struct mailmsgenv_head
#define	MAILMSGENV_FL		struct mailmsgenv_flags
#define	MAILMSGENV_ELEM		struct mailmsgenv_elemement


struct mailmsgenv_flags {
	uint		start:1 ;	/* starting envelope? (most are) */
} ;

struct mailmsgenv_elemement {
	cchar		*ep ;
	int		el ;
} ;

struct mailmsgenv_head {
	MAILMSGENV_ELEM		a ;	/* address (EMA) */
	MAILMSGENV_ELEM		d ;	/* date */
	MAILMSGENV_ELEM		r ;	/* remove-from (EMA) */
	MAILMSGENV_FL		f ;	/* flags */
	int			rt ;	/* remote type */
} ;

typedef MAILMSGENV		mailmsgenv ;
typedef MAILMSGENV_FL		mailmsgenv_fl ;
typedef MAILMSGENV_ELEM		mailmsgenv_elem ;

EXTERNC_begin

extern int mailmsgmatenv(mailmsgenv *,cchar *,int) noex ;

EXTERNC_end


#endif /* MAILMSGMATENV_INCLUDE */


