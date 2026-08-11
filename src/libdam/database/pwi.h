/* pwi HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PassWord Index manager */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PWI_INCLUDE
#define	PWI_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ipasswd.h>		/* LIBDAM */


#define	PWI		struct pwi_head
#define	PWI_CUR		struct pwi_cursor
#define	PWI_MAGIC	0x99889998


struct pwi_cursor {
	int		i ;
} ; /* end struct */

struct pwi_head {
	ipasswd		*dbp ;
	uint		magval ;
} ; /* end struct */

typedef PWI		pwi ;
typedef PWI_CUR		pwi_cur ;

EXTERNC_begin

extern int pwi_open	(pwi *,cchar *,cchar *) noex ;
extern int pwi_lookup	(pwi *,char *,int,cchar *) noex ;
extern int pwi_close	(pwi *) noex ;

EXTERNC_end


#endif /* PWI_INCLUDE */


