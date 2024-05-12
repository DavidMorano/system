/* mailaddrquote SUPPORT */
/* lang=C++20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILADDRQUOTE_INCLUDE
#define	MAILADDRQUOTE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<bufstr.h>
#include	<localmisc.h>


#define	MAILADDRQUOTE		struct mailaddrquote_head
#define	MAILADDRQUOTE_FL	struct mailaddrquote_flags
#define	MAILADDRQUOTE_LEN	100		/* default value */


struct mailaddrquote_flags {
	uint		qaddr:1 ;
} ;

struct mailaddrquote_head {
	bufstr			qaddr ;
	MAILADDRQUOTE_FL	f ;
} ;

typedef	MAILADDRQUOTE		mailaddrquote ;
typedef	MAILADDRQUOTE_FL	mailaddrquote_fl ;

EXTERNC_begin

extern int mailaddrquote_start(MAILADDRQUOTE *,cchar *,int,cchar **) noex ;
extern int mailaddrquote_finish(MAILADDRQUOTE *) noex ;

EXTERNC_end


#endif /* MAILADDRQUOTE_INCLUDE */


