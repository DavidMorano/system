/* biblecur HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* bible-cursor -- perform some output processsing activities */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-01, David A­D­ Morano
	This subroutine was written as an enhancement for adding
	back-matter (end pages) to the output document.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLECUR_INCLUDE
#define	BIBLECUR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<biblebook.h>


#define	BIBLECUR_MAGIC	0x87361524
#define	BIBLECUR	struct biblecur_head
#define	BIBLECUR_FL	struct biblecur_flags


struct biblecur_flags {
	uint		newbook:1 ;
	uint		newchapter:1 ;
	uint		newverse:1 ;
} ;

struct biblecur_head {
	BIBLECUR_FL	f ;
	uint		magic ;
	int		book ;
	int		chapter ;
	int		verse ;
	char		bookname[BIBLEBOOK_LEN + 1] ;
} ;

typedef	BIBLECUR	biblecur ;
typedef	BIBLECUR_FL	biblecur_fl ;

EXTERNC_begin

extern int biblecur_start(biblecur *) noex ;
extern int biblecur_finish(biblecur *) noex ;
extern int biblecur_check(biblecur *,cchar *,int) noex ;
extern int biblecur_newbook(biblecur *,biblebook *) noex ;
extern int biblecur_newchapter(biblecur *) noex ;
extern int biblecur_newverse(biblecur *,int) noex ;

EXTERNC_end


#endif /* BIBLECUR_INCLUDE */


