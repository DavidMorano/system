/* mailfiles HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* manage mail files (mailboxes) */
/* version %I% last-modified %G% */


/* revision history:

	= 1988-02-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1988 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILFILES_INCLUDE
#define	MAILFILES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<time.h>		/* |time_t| */
#include	<fcntl.h>		/* |off_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecitem.h>


#define	MAILFILES		vecitem
#define	MAILFILES_ENT		struct mailfiles_entry


struct mailfiles_entry {
	char		*mailfname ;
	time_t		lasttime ;
	off_t		lastsize ;
	int		f_changed ;
} ;

typedef	MAILFILES		mailfiles ;
typedef	MAILFILES_ENT		mailfiles_ent ;

EXTERNC_begin

extern int mailfiles_init(mailfiles *) noex ;
extern int mailfiles_free(mailfiles *) noex ;
extern int mailfiles_add(mailfiles *,cchar *,int) noex ;
extern int mailfiles_addpath(mailfiles *,cchar *,int) noex ;
extern int mailfiles_get(mailfiles *,int,mailfiles_ent **) noex ;
extern int mailfiles_check(mailfiles *) noex ;
extern int mailfiles_count(mailfiles *) noex ;
extern int mailfiles_parse(mailfiles *,cchar *) noex ;

EXTERNC_end


#endif /* MAILFILES_INCLUDE */



