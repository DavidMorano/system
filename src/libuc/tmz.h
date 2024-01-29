/* tmz HEADER */
/* lang=C20 */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TMZ_INCLUDE
#define	TMZ_INCLUDE


#include	<envstandards.h>	/* first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	TMZ_MAGIC	0x26292511
#define	TMZ		TMz_head
#define	TMZ_FLAGS	TMz_flags
#define	TMZ_ZNAMESIZE	8


TMz_flags {
	uint		zoff:1 ;		/* zone offset is present */
	uint		year:1 ;		/* year is present */
} ;

TMz_head {
	TM		st ;
	TMZ_FLAGS	f ;
	short		zoff ;			/* minutes west of GMT */
	char		zname[TMZ_ZNAMESIZE] ;
} ;

EXTERNC_begin

extern int	tmz_init(TMZ *) noex ;
extern int	tmz_std(TMZ *,cchar *,int) noex ;
extern int	tmz_msg(TMZ *,cchar *,int) noex ;
extern int	tmz_touch(TMZ *,cchar *,int) noex ;
extern int	tmz_toucht(TMZ *,cchar *,int) noex ;
extern int	tmz_strdig(TMZ *,cchar *,int) noex ;
extern int	tmz_logz(TMZ *,cchar *,int) noex ;
extern int	tmz_day(TMZ *,cchar *,int) noex ;
extern int	tmz_isset(TMZ *) noex ;
extern int	tmz_hasyear(TMZ *) noex ;
extern int	tmz_haszoff(TMZ *) noex ;
extern int	tmz_haszone(TMZ *) noex ;
extern int	tmz_setday(TMZ *,int,int,int) noex ;
extern int	tmz_setyear(TMZ *,int) noex ;
extern int	tmz_setzone(TMZ *,cchar *,int) noex ;
extern int	tmz_gettm(TMZ *,TM *) noex ;
extern int	tmz_getdst(TMZ *) noex ;
extern int	tmz_getzoff(TMZ *) noex ;

EXTERNC_end


#endif /* TMZ_INCLUDE */


