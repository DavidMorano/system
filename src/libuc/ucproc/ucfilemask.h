/* ucfilemask HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® information (a cache for |filemask(2)| and sisters) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCFILEMASK_INCLUDE
#define	UCFILEMASK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int uc_filemask_init() noex ;
extern int uc_filemask_fini() noex ;

extern int uc_filemaskset(mode_t) noex ;
extern int uc_filemaskget() noex ;

EXTERNC_end


#endif /* UCFILEMASK_INCLUDE */


