/* mailmsgatt HEADER */
/* lang=C20 */

/* message attachment object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGATT_INCLUDE
#define	MAILMSGATT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<vecitem.h>
#include	<mimetypes.h>
#include	<contentencodings.h>

#include	<mailmsgattent.h>


#define	MAILMSGATT		VECITEM


EXTERNC_begin

extern int mailmsgatt_start(MAILMSGATT *) noex ;
extern int mailmsgatt_finish(MAILMSGATT *) noex ;
extern int mailmsgatt_add(MAILMSGATT *,cchar *,cchar *,cchar *,int) noex ;
extern int mailmsgatt_count(MAILMSGATT *) noex ;
extern int mailmsgatt_enum(MAILMSGATT *,int,MAILMSGATTENT **) noex ;
extern int mailmsgatt_typeatts(MAILMSGATT *,MIMETYPES *) noex ;

EXTERNC_end


#endif /* MAILMSGATT_INCLUDE */


