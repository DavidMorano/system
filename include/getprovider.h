/* getprovider HEADER */
/* charset=ISO8859-1 */
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

#define	PROVIDERID_UNKNOWN	providerid_unknown
#define	PROVIDERID_SUN		providerid_sun
#define	PROVIDERID_COMPAQ	providerid_compaq
#define	PROVIDERID_SGI		providerid_sgi
#define	PROVIDERID_DELL		providerid_dell
#define	PROVIDERID_HP		providerid_hp
#define	PROVIDERID_OVERLAST	providerid_overlast


EXTERNC_begin

extern int getproviderid(cchar *,int) noex ;
extern int getprovider(char *,int) noex ;
extern int getvendor(char *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

namespace libdam {
    struct provider {
	operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
    } ; /* end struct (provider) */
}

extern libdam::provider		providerid ;

#endif /* __cplusplus */


#endif /* GETPROVIDERID_INCLUDE */


