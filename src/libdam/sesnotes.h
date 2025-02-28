/* sesnotes HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* send notes to sessions */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

#ifndef	SESNOTES_INCLUDE
#define	SESNOTES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sockaddress.h>


#define	SESNOTES		struct sesnotes_head
#define	SESNOTES_MAGIC		0x04431590
#define	SESNOTES_SESDNAME	"/tmp/sessions"


struct sesnotes_head {
	char		*sfname ;
	char		*unbuf ;	/* memory-allocated */
	pid_t		pid ;
	uint		magic ;
	int		fd ;
} ;

typedef	SESNOTES	sesnotes ;

EXTERNC_begin

extern int sesnotes_open(sesnotes *,cchar *) noex ;
extern int sesnotes_send(sesnotes *,int,cchar *,int,pid_t) noex ;
extern int sesnotes_sendbiff(sesnotes *,cchar *,int,pid_t) noex ;
extern int sesnotes_sendgen(sesnotes *,cchar *,int,pid_t) noex ;
extern int sesnotes_close(sesnotes *) noex ;

EXTERNC_end


#endif /* SESNOTES_INCLUDE */


