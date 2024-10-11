/* whitelist HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* whitelist mail management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	WHITELIST_INCLUDE
#define	WHITELIST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<vecstr.h>


#define	WHITELIST_MAGIC		0x65437296
#define	WHITELIST_DEFENTS	10
#define	WHITELIST		struct whitelist_head
#define	WHITELIST_CUR		struct whitelist_cursor


struct whitelist_head {
	vecstr		*wlp ;
	uint		magic ;
} ;

struct whitelist_c {
	int		i ;
} ;

typedef	WHITELIST	whitelist ;
typedef	WHITELIST_CUR	whitelist_cur ;

EXTERNC_begin

extern int whitelist_open(whitelist *,cchar *) noex ;
extern int whitelist_fileadd(whitelist *,cchar *) noex ;
extern int whitelist_count(whitelist *) noex ;
extern int whitelist_get(whitelist *,int,cchar **) noex ;
extern int whitelist_read(whitelist *,char *,int,int) noex ;
extern int whitelist_prematch(whitelist *,cchar *) noex ;
extern int whitelist_audit(whitelist *) noex ;
extern int whitelist_close(whitelist *) noex ;

EXTERNC_end


#endif /* WHITELIST_INCLUDE */


