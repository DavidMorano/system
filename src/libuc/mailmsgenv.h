/* mailmsgenv HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGENV_INCLUDE
#define	MAILMSGENV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>



/* object defines */

#define	MAILMSGENV		struct mailmsgenv_head
#define	MAILMSGENV_FL		struct mailmsgenv_flags
#define	MAILMSGENV_MAXLINELEN	1092


struct mailmsgenv_flags {
	UINT		start:1 ;	/* was it a starting envelope? */
	UINT		init_time:1 ;	/* did we parse its time yet? */
	UINT		hastime:1 ;	/* did it have a time specification */
	UINT		haszone:1 ;	/* did it have a time zone? */
} ;

struct mailmsgenv_head {
	MAILMSGENV_FL	f ;
	const char	*address ;
	const char	*origdate ;
	const char	*remote ;
	const char	*tzname ;
	time_t		daytime ;
	int		alen ;
} ;

typedef MAILMSGENV	mailmsgenv ;

EXTERNC_begin

extern int mailmsgenv_start(MAILMSGENV *,cchar *,int) noex ;
extern int mailmsgenv_isstart(MAILMSGENV *) noex ;
extern int mailmsgenv_getaddress(MAILMSGENV *,cchar **) noex ;
extern int mailmsgenv_getremote(MAILMSGENV *,cchar **) noex ;
extern int mailmsgenv_gettzname(MAILMSGENV *,cchar **) noex ;
extern int mailmsgenv_gettime(MAILMSGENV *,time_t *) noex ;
extern int mailmsgenv_mkdatestr(MAILMSGENV *,char *,char *,int) noex ;
extern int mailmsgenv_mkenv(MAILMSGENV *,char *,int) noex ;
extern int mailmsgenv_finish(MAILMSGENV *) noex ;

EXTERNC_end


#endif /* MAILMSGENV_INCLUDE */


