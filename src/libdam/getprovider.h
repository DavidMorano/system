/* getprovider HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* get a provider ID */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPROVIDERID_INCLUDE
#define	GETPROVIDERID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


enum providerids {
    providerid_unknown,
    providerid_sun,
    providerid_comppaq,
    providerid_sgi,
    providerid_dell,
    providerid_hp,
    providerid_overlast
} ;

#define	PROVIDERID_UNKNOWN	providerif_unknown
#define	PROVIDERID_SUN		providerif_sun
#define	PROVIDERID_COMPAQ	providerif_compaq
#define	PROVIDERID_SGI		providerif_sgi
#define	PROVIDERID_DELL		providerif_dell
#define	PROVIDERID_HP		providerif_hp
#define	PROVIDERID_OVERLAST	providerif_overlast


EXTERNC_begin

extern int getproviderid(cchar *,int) noex ;
extern int getprovider(char *,int) noex ;
extern int getvendor(char *,int) noex ;

EXTERNC_end


#endif /* GETPROVIDERID_INCLUDE */


