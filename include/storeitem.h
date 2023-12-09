/* storeitem INCLUDE */
/* lang=C20 */

/* storage object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-17, David A­D­ Morano
	This object was originally written for use in some old
	Personal Communication Services (PCS) code (from some time
	ago, 1993?).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STOREITEM_INCLUDE
#define	STOREITEM_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>


#define	STOREITEM	struct storeitem_head


struct storeitem_head {
	char		*dbuf ;
	int		dlen ;
	int		index ;
	int		f_overflow ;
} ;

typedef STOREITEM	storeitem ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int storeitem_start(storeitem *,char *,int) noex ;
extern int storeitem_strw(storeitem *,cchar *,int,cchar **) noex ;
extern int storeitem_buf(storeitem *,cvoid *,int,cchar **) noex ;
extern int storeitem_dec(storeitem *,int,cchar **) noex ;
extern int storeitem_char(storeitem *,int,cchar **) noex ;
extern int storeitem_nul(storeitem *,cchar **) noex ;
extern int storeitem_ptab(storeitem *,int,void ***) noex ;
extern int storeitem_block(storeitem *,int,int,void **) noex ;
extern int storeitem_getlen(storeitem *) noex ;
extern int storeitem_finish(storeitem *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* storeitem_INCLUDE */


