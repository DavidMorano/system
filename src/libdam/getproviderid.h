/* getproviderid HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* get a provider ID */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPROVIDERID_INCLUDE
#define	GETPROVIDERID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>


#define	PROVIDERID_UNKNOWN	0
#define	PROVIDERID_SUN		1
#define	PROVIDERID_COMPAQ	2
#define	PROVIDERID_SGI		3
#define	PROVIDERID_DELL		4
#define	PROVIDERID_HP		5
#define	PROVIDERID_OVERLAST	6


EXTERNC_begin

extern int getproviderid(cchar *,int) noex ;

EXTERNC_end


#endif /* GETPROVIDERID_INCLUDE */


