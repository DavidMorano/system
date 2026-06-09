/* varray HEADER (Void-Array) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* object implementing variable-length array of elements */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VARRAY_INCLUDE
#define	VARRAY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<lookaside.h>		/* LIBUC */


#define	VARRAY		struct varray_head
#define	VARRAY_DEFENTS	4


struct varray_head {
	void		**va ;
	lookaside	*lap ;
	int		esz ;		/* element size */
	int		cnt ;		/* count of items in array */
	int		ext ;		/* extent of array */
	int		umax ;		/* maximum used */
} ; /* end struct (varray_head) */

EXTERNC_begin
typedef int (*varray_vcmp)(cvoid **,cvoid **) noex ;
EXTERNC_end

#ifdef	__cplusplus
enum varraymems {
	varraymem_count,
    	varraymem_delall,
	varraymem_audit,
	varraymem_finish,
	varraymem_overlast
} ; /* end enum (varraymems) */
struct varray ;
struct varray_co {
	varray		*op = nullptr ;
	int		w = -1 ;
	void operator () (varray *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (varray_co) */
struct varray : varray_head {
	varray_co	count ;
	varray_co	delall ;
	varray_co	audit ;
	varray_co	finish ;
	varray() noex {
	    count	(this,varraymem_count) ;
	    delall	(this,varraymem_delall) ;
	    audit	(this,varraymem_audit) ;
	    finish	(this,varraymem_finish) ;
	    va = nullptr ;
	} ; /* end ctor */
	varray(const varray &) = delete ;
	varray &operator = (const varray &) = delete ;
	int start	(int,int) noex ;
	int enumer	(int,void *) noex ;
	int acc		(int,void *) noex ;
	int mk		(int,void *) noex ;
	int del		(int = -1) noex ;
	int find	(void *) noex ;
	int search	(void *,varray_vcmp,void *) noex ;
	operator int () noex ;
	void dtor() noex ;
	destruct varray() {
	    if (va) dtor() ;
	} ; /* end destruct */
} ; /* end struct (varray) */
#else	/* __cplusplus */
typedef VARRAY		varray ;
#endif /* __cplusplus */

EXTERNC_begin

extern int varray_start		(varray *,int,int) noex ;
extern int varray_enumer	(varray *,int,void *) noex ;
extern int varray_acc		(varray *,int,void *) noex ;
extern int varray_mk		(varray *,int,void *) noex ;
extern int varray_del		(varray *,int) noex ;
extern int varray_delall	(varray *) noex ;
extern int varray_count		(varray *) noex ;
extern int varray_find		(varray *,void *) noex ;
extern int varray_search	(varray *,void *,varray_vcmp,void *) noex ;
extern int varray_audit		(varray *) noex ;
extern int varray_finish	(varray *) noex ;

EXTERNC_end


#endif /* VARRAY_INCLUDE */


