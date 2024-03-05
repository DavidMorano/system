/* getse HEADER */
/* lang=C20 */

/* get service entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETSE_INCLUDE
#define	GETSE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


EXTERNC_begin

extern int getse_begin(int) noex ;
extern int getse_ent(ucentsv *,char *,int) noex ;
extern int getse_name(ucentsv *,char *,int,cchar *,cchar *) noex ;
extern int getse_port(ucentsv *,char *,int,int,cchar *) noex ;
extern int getse_end() noex ;

EXTERNC_end


#endif /* GETSE_INCLUDE */


