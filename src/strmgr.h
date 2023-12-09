/* strmgr INCLUDE */
/* lang=C20 */

/* string manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	STRMGR_INCLUDE
#define	STRMGR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<localmisc.h>


#define	STRMGR		struct strmgr_head


struct strmgr_head {
	char		*dp ;
	int		dl ;
	int		dlen ;
} ;

typedef struct strmgr_head	strmgr ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int strmgr_start(STRMGR *,char *,int) noex ;
extern int strmgr_avail(STRMGR *) noex ;
extern int strmgr_rem(STRMGR *) noex ;
extern int strmgr_str(STRMGR *,cchar *,int) noex ;
extern int strmgr_char(STRMGR *,int) noex ;
extern int strmgr_finish(STRMGR *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* STRMGR_INCLUDE */


