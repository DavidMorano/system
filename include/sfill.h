/* sfill INCLUDE */
/* lang=C20 */

/* text fill */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SFILL_INCLUDE
#define	SFILL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<system.h>
#include	<bfile.h>
#include	<fifostr.h>
#include	<localmisc.h>


#define	SFILL		struct sfill_head


struct sfill_head {
	bfile		*ofp ;
	fifostr		sq ;
	int		clen ;
	int		indent ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	sfill_start(SFILL *,int,bfile *) noex ;
extern int	sfill_remaining(SFILL *) noex ;
extern int	sfill_proc(SFILL *,int,cchar *,int) noex ;
extern int	sfill_wline(SFILL *,int) noex ;
extern int	sfill_finish(SFILL *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SFILL_INCLUDE */


