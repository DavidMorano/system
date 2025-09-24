/* mhcom HEADER (Message Header Comment) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* comment-parse (for RFC822) small strings (like for stupid RFC822 date) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MHCOM_INCLUDE
#define	MHCOM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	MHCOM_MAGIC	0x98638451
#define	MHCOM		struct mhcom_head


struct mhcom_head {
	char		*a ;
	char		*value ;
	char		*comment ;
	uint		magic ;
	int		vlen ;
	int		clen ;
} ;

#ifdef	__cplusplus
enum mhcommems {
	mhcommem_finish,
	mhcommem_overlast
} ;
struct mhcom ;
struct mhcom_co {
	mhcom		*op = nullptr ;
	int		w = -1 ;
	void operator () (mhcom *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (mhcom_co) */
struct mhcom : mhcom_head {
	mhcom_co	finish ;
	mhcom() noex {
	    finish	(this,mhcommem_finish) ;
	    magic = 0 ;
	} ;
	mhcom(const mhcom &) = delete ;
	mhcom &operator = (const mhcom &) = delete ;
	int start(cchar *,int = -1) noex ;
	int getval(cchar **) noex ;
	int getcom(cchar **) noex ;
	void dtor() noex ;
	destruct mhcom() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (mhcom) */
#else	/* __cplusplus */
typedef MHCOM		mhcom ;
#endif /* __cplusplus */

EXTERNC_begin

extern int mhcom_start(mhcom *,cchar *,int) noex ;
extern int mhcom_getval(mhcom *,cchar **) noex ;
extern int mhcom_getcom(mhcom *,cchar **) noex ;
extern int mhcom_finish(mhcom *) noex ;

EXTERNC_end


#endif /* MHCOM_INCLUDE */


