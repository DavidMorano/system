/* setoint HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* ordered set of integers */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	SETOINT_INCLUDE
#define	SETOINT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	SETOINT		struct setoint_head
#define	SETOINT_CUR	struct setoint_cursor


struct setoint_cursor {
	void		*interp ;
} ;

struct setoint_head {
	void		*setp ;
} ;

typedef SETOINT		setoint ;
typedef SETOINT_CUR	setoint_cur ;

EXTERNC_begin

extern int setoint_start(setoint *) noex ;
extern int setoint_finish(setoint *) noex ;
extern int setoint_addval(setoint *,int) noex ;
extern int setoint_delval(setoint *,int) noex ;
extern int setoint_count(setoint *) noex ;
extern int setoint_mkvec(setoint *,int *) noex ;
extern int setoint_extent(setoint *) noex ;
extern int setoint_curbegin(setoint *,setoint_cur *) noex ;
extern int setoint_curenum(setoint *,setoint_cur *,int *) noex ;
extern int setoint_curend(setoint *,setoint_cur *) noex ;
extern int setoint_find(setoint *,int) noex ;

EXTERNC_end


#endif /* SETOINT_INCLUDE */


