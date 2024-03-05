/* getpe HEADER */
/* lang=C20 */

/* get protocol entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPE_INCLUDE
#define	GETPE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


EXTERNC_begin

extern int getpe_begin(int) noex ;
extern int getpe_ent(ucentpr *,char *,int) noex ;
extern int getpe_name(ucentpr *,char *,int,cchar *) noex ;
extern int getpe_num(ucentpr *,char *,int,int) noex ;
extern int getpe_end() noex ;

EXTERNC_end


#endif /* GETPE_INCLUDE */


