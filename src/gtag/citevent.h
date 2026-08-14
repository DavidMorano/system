/* citevent HREADER (Citation Event) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process the citevent of finding a citætion tag */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	CITEVENT_INCLUDE
#define	CITEVENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	CITEVENT	struct citevent_head
#define	CITEVENT_MAGIC	0x78685756


struct citevent_head {
	cchar		*citekey ;
	off_t		offset ;
} ; /* end struct */

typedef	CITEVENT		citevent ;

EXTERNC_begin

extern int citevent_start	(citevent *,off_t,cchar *) noex ;
extern int citevent_offset	(citevent *,off_t *) noex ;
extern int citevent_citekey	(citevent *,cchar **) noex ;
extern int citevent_finish	(citevent *) noex ;

EXTERNC_end


#endif /* CITEVENT_INCLUDE */


