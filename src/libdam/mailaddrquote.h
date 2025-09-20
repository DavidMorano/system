/* mailaddrquote HEADER */
/* lang=C20 */

/* quote mail-addresses */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILADDRQUOTE_INCLUDE
#define	MAILADDRQUOTE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bufstr.h>


#define	MAILADDRQUOTE		struct mailaddrquote_head
#define	MAILADDRQUOTE_FL	struct mailaddrquote_flags
#define	MAILADDRQUOTE_LEN	100		/* default value */
#define	MAILADDRQUOTE_MAGIC	0x08938725	/* magic */


struct mailaddrquote_flags {
	uint		qaddr:1 ;
} ;

struct mailaddrquote_head {
	bufstr			*bsp ;	/* buffer-string-pointer */
	MAILADDRQUOTE_FL	fl ;
	uint			magic ;
} ;

typedef	MAILADDRQUOTE		mailaddrquote ;
typedef	MAILADDRQUOTE_FL	mailaddrquote_fl ;

EXTERNC_begin

extern int mailaddrquote_start(mailaddrquote *,cchar *,int,cchar **) noex ;
extern int mailaddrquote_finish(mailaddrquote *) noex ;

EXTERNC_end


#endif /* MAILADDRQUOTE_INCLUDE */


