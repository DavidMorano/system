/* bvsmk HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a BVS database */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVSMK_INCLUDE
#define	BVSMK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>


#define	BVSMK_MAGIC	0x88773423
#define	BVSMK		struct bvsmk_head
#define	BVSMK_FL	struct bvsmk_flags
#define	BVSMK_OBJ	struct bvsmk_object
#define	BVSMK_INTOPEN	(10*60)
#define	BVSMK_INTSTALE	(5*60)


struct bvsmk_object {
	cchar		*name ;
	uint		objsize ;
} ;

struct bvsmk_flags {
	uint		notsorted:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		inprogress:1 ;
	uint		created :1 ;
	uint		abort:1 ;
} ;

struct bvsmk_head {
	cchar		*a ;		/* memory-allocation (pr, db) */
	cchar		*pr ;
	cchar 		*db ;
	cchar		*idname ;
	char		*nidxfname ;
	vecobj		books ;
	BVSMK_FL	fl ;
	uint		magic ;
	int		nverses ;
	int		nzverses ;
	int		maxbook ;
	int		nfd ;
	mode_t		om ;
} ;

typedef	BVSMK		bvsmk ;
typedef	BVSMK_FL	bvsmk_fl ;
typedef	BVSMK_OBJ	bvsmk_obj ;

EXTERNC_begin

extern int	bvsmk_open(bvsmk *,cchar *,cchar *,int,mode_t) noex ;
extern int	bvsmk_add(bvsmk *,int,uchar *,int) noex ;
extern int	bvsmk_abort(bvsmk *,int) noex ;
extern int	bvsmk_close(bvsmk *) noex ;

EXTERNC_end


#endif /* BVSMK_INCLUDE */


