/* tmz HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* time and timezone parsing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TMZ_INCLUDE
#define	TMZ_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	TMZ_MAGIC	0x26292511
#define	TMZ		tmz
#define	TMZ_FLAGS	tmz_flags


struct tmz_flags {
	uint		zoff:1 ;	/* zone offset is present */
	uint		year:1 ;	/* year is present */
} ;

struct tmz {
	char		*zname ;
	TM		st ;
	TMZ_FLAGS	f ;
	short		zoff ;		/* minutes west of GMT */
	int clear() noex ;
	void dtor() noex ;
	~tmz() {
	    dtor() ;
	} ;
} ;

EXTERNC_begin

extern int	tmz_init(tmz *) noex ;
extern int	tmz_std(tmz *,cchar *,int) noex ;
extern int	tmz_msg(tmz *,cchar *,int) noex ;
extern int	tmz_touch(tmz *,cchar *,int) noex ;
extern int	tmz_toucht(tmz *,cchar *,int) noex ;
extern int	tmz_strdig(tmz *,cchar *,int) noex ;
extern int	tmz_logz(tmz *,cchar *,int) noex ;
extern int	tmz_day(tmz *,cchar *,int) noex ;
extern int	tmz_isset(tmz *) noex ;
extern int	tmz_hasyear(tmz *) noex ;
extern int	tmz_haszoff(tmz *) noex ;
extern int	tmz_haszone(tmz *) noex ;
extern int	tmz_setday(tmz *,int,int,int) noex ;
extern int	tmz_setyear(tmz *,int) noex ;
extern int	tmz_setzone(tmz *,cchar *,int) noex ;
extern int	tmz_gettm(tmz *,TM *) noex ;
extern int	tmz_getdst(tmz *) noex ;
extern int	tmz_getzoff(tmz *) noex ;

EXTERNC_end


#endif /* __cplusplus */
#endif /* TMZ_INCLUDE */


