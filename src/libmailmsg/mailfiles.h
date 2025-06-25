/* mailfiles HEADER */
/* charset=ISO8859-1 */
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
#include	<vecobj.h>


#define	MAILFILES_MAGIC		0x21658252
#define	MAILFILES		struct mailfiles_head
#define	MAILFILES_ENT		struct mailfiles_entry


struct mailfiles_head {
    	vecobj		*elp ;
	uint		magic ;
} ;

struct mailfiles_entry {
	cchar		*mailfname ;
	time_t		lasttime ;
	off_t		lastsize ;
	int		f_changed ;
} ;

typedef	MAILFILES		mailfiles ;
typedef	MAILFILES_ENT		mailfiles_ent ;

EXTERNC_begin

extern int mailfiles_start(mailfiles *) noex ;
extern int mailfiles_finish(mailfiles *) noex ;
extern int mailfiles_add(mailfiles *,cchar *,int) noex ;
extern int mailfiles_addpath(mailfiles *,cchar *,int) noex ;
extern int mailfiles_get(mailfiles *,int,mailfiles_ent **) noex ;
extern int mailfiles_check(mailfiles *) noex ;
extern int mailfiles_count(mailfiles *) noex ;
extern int mailfiles_parse(mailfiles *,cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
static inline int mailfiles_magic(mailfiles *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MAILFILES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailfiles_magic) */

#endif /* __cplusplus */


#endif /* MAILFILES_INCLUDE */



