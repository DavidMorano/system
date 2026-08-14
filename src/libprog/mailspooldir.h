/* mailspooldir HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* check for a valid mail spool directory */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILSPOOLDIR_INCLUDE
#define	MAILSPOOLDIR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<proginfo.hh>		/* LIBPROG */


EXTERNC_begin

extern int	mailspooldir(proginfo *,char *,int) noex ;

EXTERNC_end


#endif /* MAILSPOOLDIR_INCLUDE */


