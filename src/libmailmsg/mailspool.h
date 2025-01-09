/* mailspool HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a mail-spool file w/ its associated lock */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILSPOOL_INCLUDE
#define	MAILSPOOL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<fcntl.h>		/* file types |mode_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	MAILSPOOL	struct mailspool_head
#define	MAILSPOOL_MAGIC	0x95437652


struct mailspool_head {
	cchar		*lfname ;	/* lock file-name */
	int		mfd ;		/* mail-file descriptor */
	int		f_created ;
	uint		magic ;
} ;

typedef	MAILSPOOL	mailspool ;

EXTERNC_begin

extern int mailspool_open(mailspool *,cchar *,cchar *,int,mode_t,int) noex ;
extern int mailspool_setlockinfo(mailspool *,cchar *,int) noex ;
extern int mailspool_close(mailspool *) noex ;

EXTERNC_end


#endif /* MAILSPOOL_INCLUDE */


