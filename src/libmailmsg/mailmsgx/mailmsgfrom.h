/* mailmsgfrom HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */


/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGFROM_INCLUDE
#define	MAILMSGFROM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	MAILMSGFROM	struct mailmsgfrom_head


struct mailmsgfrom_head {
	time_t		ti_msg ;
	char		*fbuf ;		/* FROM-buffer */
	int		flen ;		/* FROM-buffer size */
	int		fl ;		/* FROM-buffer result length */
} ;

typedef	MAILMSGFROM	mailmsgfrom ;

EXTERNC_begin

extern int	mailmsgfrom_start(mailmsgfrom *,char *,int) noex ;
extern int	mailmsgfrom_test(mailmsgfrom *,time_t) noex ;
extern int	mailmsgfrom_loadfrom(mailmsgfrom *,cchar *,int) noex ;
extern int	mailmsgfrom_finish(mailmsgfrom *) noex ;

EXTERNC_end


#endif /* MAILMSGFROM_INCLUDE */


