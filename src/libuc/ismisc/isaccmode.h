/* isaccmode HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does a file access-mode have a property? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISACCMODE_INCLUDE
#define	ISACCMODE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifdef	__cplusplus
struct isaccmoder {
    bool rd	(int) const noex ;
    bool wr	(int) const noex ;
} ; /* end struct (isaccmoder) */
#endif /* __cplusplus */

EXTERNC_begin

extern bool	isaccmode_readable(int) noex ;
extern bool	isaccmode_writable(int) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern isaccmoder	isaccmode ;
#endif /* __cplusplus */


#endif /* ISACCMODE_INCLUDE */


