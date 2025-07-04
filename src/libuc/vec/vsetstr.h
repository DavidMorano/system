/* vsetstr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Vector-Implemented Ordered-Set-String object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VSETSTR_INCLUDE
#define	VSETSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecpstr.h>


#define	VSETSTR_MAGIC	0x09097641
#define	VSETSTR_CSIZE	10
#define	VSETSTR		struct vsetstr_head
#define	VSETSTR_CUR	struct vsetstr_cursor


struct vsetstr_head {
	vecpstr		*elp ;
	uint		magic ;
} ;

struct vsetstr_cursor {
	int		i ;
} ;


typedef	VSETSTR		vsetstr ;
typedef	VSETSTR_CUR	vsetstr_cur ;

EXTERNC_begin

extern int vsetstr_start(vsetstr *,int) noex ;
extern int vsetstr_already(vsetstr *,cchar *,int) noex ;
extern int vsetstr_del(vsetstr *,cchar *,int) noex ;
extern int vsetstr_count(vsetstr *) noex ;
extern int vsetstr_extent(vsetstr *) noex ;
extern int vsetstr_curbegin(vsetstr *,vsetstr_cur *) noex ;
extern int vsetstr_curend(vsetstr *,vsetstr_cur *) noex ;
extern int vsetstr_curdel(vsetstr *,vsetstr_cur *) noex ;
extern int vsetstr_curenum(vsetstr *,vsetstr_cur *,cchar **) noex ;
extern int vsetstr_curnext(vsetstr *,vsetstr_cur *) noex ;
extern int vsetstr_finish(vsetstr *) noex ;

EXTERNC_end


#endif /* vsetstr_INCLUDE */


