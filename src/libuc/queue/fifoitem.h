/* fifoitem HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* FIFO container object */
/* version %I% last-modified %G% */


/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

#ifndef	FIFOITEM_INCLUDE
#define	FIFOITEM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	FIFOITEM	struct fifoitem_head
#define	FIFOITEM_ENT	struct fifoitem_entry
#define	FIFOITEM_CUR	struct fifoitem_cursor
#define	FIFOITEM_MAGIC	0x12345678


struct fifoitem_entry {
	FIFOITEM_ENT	*next ;
	FIFOITEM_ENT	*prev ;
	cvoid		*dp ;
	int		dl ;
} ; /* end struct (fifoitem_entry) */

struct fifoitem_head {
	FIFOITEM_ENT	*head ;
	FIFOITEM_ENT	*tail ;
	int		magval ;
	int		n ;
} ; /* end struct (fifoitem_head) */

struct fifoitem_cursor {
	FIFOITEM_ENT	*current ;
} ; /* end struct (fifoitem_cursor) */

typedef FIFOITEM	fifoitem ;
typedef FIFOITEM_ENT	fifoitem_ent ;
typedef FIFOITEM_CUR	fifoitem_cur ;

EXTERNC_begin

typedef int (*fifoitem_cmp)(cvoid *,cvoid *) noex ;

extern int fifoitem_start	(fifoitem *) noex ;
extern int fifoitem_finish	(fifoitem *) noex ;
extern int fifoitem_ins		(fifoitem *,cvoid *,int) noex ;
extern int fifoitem_rem		(fifoitem *,void *,int) noex ;
extern int fifoitem_del		(fifoitem *) noex ;
extern int fifoitem_count	(fifoitem *) noex ;
extern int fifoitem_curbegin	(fifoitem *,fifoitem_cur *) noex ;
extern int fifoitem_curend	(fifoitem *,fifoitem_cur *) noex ;
extern int fifoitem_curdel	(fifoitem *,fifoitem_cur *) noex ;
extern int fifoitem_curenum	(fifoitem *,fifoitem_cur *,cvoid **) noex ;
extern int fifoitem_present	(fifoitem *,cvoid *,int,fifoitem_cmp) noex ;

EXTERNC_end


#endif /* FIFOITEM_INCLUDE */


