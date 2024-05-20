/* proginfo HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGINFO_INCLUDE
#define	PROGINFO_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>


#define	PROGINFO	struct proginfo_head


struct proginfo_head {
	mainv		argv ;
	mainv		envv ;
	int		argc ;
} ;

typedef	PROGINFO	proginfo ;

EXTERNC_begin

extern int proginfo_start(proginfo *,cchar **,cchar *,cchar *) noex ;
extern int proginfo_setprogroot(proginfo *,cchar *,int) noex ;
extern int proginfo_rootprogdname(proginfo *) noex ;
extern int proginfo_rootexecname(proginfo *,cchar *) noex ;
extern int proginfo_setentry(proginfo *,cchar **,cchar *,int) noex ;
extern int proginfo_setversion(proginfo *,cchar *) noex ;
extern int proginfo_setbanner(proginfo *,cchar *) noex ;
extern int proginfo_setsearchname(proginfo *,cchar *,cchar *) noex ;
extern int proginfo_setprogname(proginfo *,cchar *) noex ;
extern int proginfo_setexecname(proginfo *,cchar *) noex ;
extern int proginfo_pwd(proginfo *) noex ;
extern int proginfo_getpwd(proginfo *,char *,int) noex ;
extern int proginfo_getename(proginfo *,char *,int) noex ;
extern int proginfo_nodename(proginfo *) noex ;
extern int proginfo_finish(proginfo *) noex ;

EXTERNC_end


#endif /* PROGINFO_INCLUDE */


