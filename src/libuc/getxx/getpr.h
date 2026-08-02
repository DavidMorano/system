/* getpr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get protocol entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPR_INCLUDE
#define	GETPR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucentpr.h>		/* LIBUC */


EXTERNC_begin

extern int getpr_begin	(int) noex ;
extern int getpr_ent	(ucentpr *,char *,int) noex ;
extern int getpr_name	(ucentpr *,char *,int,cchar *) noex ;
extern int getpr_num	(ucentpr *,char *,int,int) noex ;
extern int getpr_end	() noex ;

EXTERNC_end


#endif /* GETPR_INCLUDE */


