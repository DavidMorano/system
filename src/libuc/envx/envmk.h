/* envmk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make new environment for a program */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ENVMK_INCLUDE
#define	ENVMK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vechand.h>		/* LIBUC */
#include	<strpack.h>		/* LIBUC */


#define	ENVMK		struct envmk_head
#define	ENVMK_MAGIC	0x87485926


struct envmk_head {
	cchar		*un ;		/* user-name */
	cchar		*uh ;		/* user-home */
	mainv		envv ;
	vechand		*envp ;
	strpack		*storep ;
	uint		magval ;
} ; /* end struct (envmk_head) */

typedef ENVMK		envmk ;

EXTERNC_begin

extern int envmk_start	(envmk *,mainv) noex ;
extern int envmk_envset	(envmk *,cchar *,cchar *,int) noex ;
extern int envmk_getvec	(envmk *,mainv *) noex ;
extern int envmk_finish	(envmk *) noex ;

EXTERNC_end


#endif /* ENVMK_INCLUDE */


