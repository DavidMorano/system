/* wordfill HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* text fill */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-04, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

#ifndef	WORDFILL_INCLUDE
#define	WORDFILL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<fifostr.h>


#define	WORDFILL_MAGIC	0x88442239
#define	WORDFILL	struct wordfill_head


struct wordfill_head {
	fifostr		*sqp ;		/* string-queue-pointer */
	uint		magic ;
	int		wc ;		/* word-count */
	int		chrc ;		/* character count (w/ blanks) */
} ;

typedef	WORDFILL	wordfill ;

EXTERNC_begin

extern int	wordfill_start(wordfill *,cchar *,int) noex ;
extern int	wordfill_addword(wordfill *,cchar *,int) noex ;
extern int	wordfill_addline(wordfill *,cchar *,int) noex ;
extern int	wordfill_addlines(wordfill *,cchar *,int) noex ;
extern int	wordfill_mklinefull(wordfill *,char *,int) noex ;
extern int	wordfill_mklinepart(wordfill *,char *,int) noex ;
extern int	wordfill_finish(wordfill *) noex ;

EXTERNC_end


#endif /* WORDFILL_INCLUDE */


