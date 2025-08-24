/* fifoelem HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* FIFO container object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FIFOELEM_INCLUDE
#define	FIFOELEM_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	FIFOELEM_MAGIC	0x12345678
#define	FIFOELEM	struct fifoelem_head
#define	FIFOELEM_ENT	struct fifoelem_entry
#define	FIFOELEM_CUR	struct fifoelem_cursor


struct fifoelem_entry {
	FIFOELEM_ENT	*next ;		/* must be first! */
	FIFOELEM_ENT	*prev ;
	void		*dp ;		/* data-pointer */
	int		dl ;		/* data-length */
} ;


struct fifoelem_head {
	FIFOELEM_ENT	*head ;
	FIFOELEM_ENT	*tail ;
	uint		magic ;
	int		n ;
} ;


struct fifoelem_cursor {
	FIFOELEM_ENT	*current ;
} ;


typedef FIFOELEM	fifoelem ;
typedef FIFOELEM_ENT	fifoelem_ent ;
typedef FIFOELEM_CUR	fifoelem_cur ;

EXTERNC_begin

typedef int	(*fifoelem_cmp)(cvoid *,cvoid *) noex ;

extern int fifoelem_start(fifoelem *) noex ;
extern int fifoelem_finish(fifoelem *) noex ;
extern int fifoelem_add(fifoelem *,void *,int) noex ;
extern int fifoelem_rem(fifoelem *,void *,int) noex ;
extern int fifoelem_get(fifoelem *,fifoelem_ent **) noex ;
extern int fifoelem_del(fifoelem *) noex ;
extern int fifoelem_count(fifoelem *) noex ;
extern int fifoelem_curbegin(fifoelem *,fifoelem_cur *) noex ;
extern int fifoelem_curend(fifoelem *,fifoelem_cur *) noex ;
extern int fifoelem_curdel(fifoelem *,fifoelem_cur *) noex ;
extern int fifoelem_curfetch(fifoelem *,fifoelem_cur *,fifoelem_ent **) noex ;
extern int fifoelem_present(fifoelem *,cvoid *,int,fifoelem_cmp) noex ;

/*
extern int fifoelem_curenum(fifoelem *,fifoelem_cur *,void *) noex ;
extern int fifoelem_search(fifoelem *,char *,int (*)(),char **) ;
extern int fifoelem_find(fifoelem *,void *) ;
*/

EXTERNC_end


#endif /* FIFOELEM_INCLUDE */


