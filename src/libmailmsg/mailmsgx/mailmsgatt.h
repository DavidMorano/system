/* mailmsgatt HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* mail-message attachment object processing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGATT_INCLUDE
#define	MAILMSGATT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecitem.h>		/* LIBUC */
#include	<mimetypes.h>		/* LIBMAILMSG */
#include	<contentencodings.h>	/* LIBMAILMSG */
#include	<mailmsgattent.h>	/* LIBMAILMSG */


#define	MAILMSGATT	vecitem


typedef MAILMSGATT	mailmsgatt ;

EXTERNC_begin

extern int mailmsgatt_start(mailmsgatt *) noex ;
extern int mailmsgatt_finish(mailmsgatt *) noex ;
extern int mailmsgatt_add(mailmsgatt *,cchar *,cchar *,cchar *,int) noex ;
extern int mailmsgatt_count(mailmsgatt *) noex ;
extern int mailmsgatt_curenum(mailmsgatt *,int,mailmsgattent **) noex ;
extern int mailmsgatt_typeatts(mailmsgatt *,mimetypes *) noex ;

EXTERNC_end


#endif /* MAILMSGATT_INCLUDE */


