/* getpwentry HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrieve a PWENTRY given a name or UID */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPWENTRY_INCLUDE
#define	GETPWENTRY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |uid_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<pwentry.h>		/* LIBUC */


EXTERNC_begin

extern int getpwentry_name	(pwentry *,char *,int,cchar *) noex ;
extern int getpwentry_uid	(pwentry *,char *,int,uid_t) noex ;

EXTERNC_end


#endif /* GETPWENTRY_INCLUDE */


