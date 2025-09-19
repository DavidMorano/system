/* bvimk HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVIMK_INCLUDE
#define	BVIMK_INCLUDE


#include	<envstandards.h>
#include	<vecobj.h>


#define	BVIMK_MAGIC	0x88773422
#define	BVIMK		struct bvimk_head
#define	BVIMK_FL	struct bvimk_flags
#define	BVIMK_OBJ	struct bvimk_object
#define	BVIMK_VER	struct bvimk_verse
#define	BVIMK_LN	struct bvimk_line
#define	BVIMK_INFO	struct bvimk_information
#define	BVIMK_INTOPEN	(10*60)
#define	BVIMK_INTSTALE	(5*60)


struct bvimk_object {
	cchar		*name ;
	uint		objsize ;
} ;

struct bvimk_information {
	uint		maxbook ;
	uint		maxchapter ;
	uint		maxverse ;
	uint		nverses ;
	uint		nzverses ;
} ;

struct bvimk_line {
	uint		loff ;
	uint		llen ;
} ;

struct bvimk_verse {
	BVIMK_LINE	*lines ;
	uint		voff ;
	uint		vlen ;
	uchar		nlines, b, c, v ;
} ;

struct bvimk_flags {
	uint		notsorted:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
	uint		abort:1 ;
} ;

struct bvimk_head {
	cchar 		*dbname ;
	cchar		*idname ;
	char		*nidxfname ;
	vecobj		verses ;
	vecobj		lines ;
	BVIMK_FL	fl ;
	uint		magic ;
	uint		pcitation ;
	uint		maxbook ;
	uint		maxchapter ;
	uint		maxverse ;
	uint		nverses ;
	uint		nzverses ;
	int		nfd ;
	mode_t		om ;
} ;

typedef	BVIMK		bvimk ;
typedef	BVIMK_FL	bvimk_fl ;
typedef	BVIMK_OBJ	bvimk_obj ;
typedef	BVIMK_VER	bvimk_ver ;
typedef	BVIMK_LN	bvimk_ln ;
typedef	BVIMK_INFO	bvimk_info ;

EXTERNC_begin

extern int	bvimk_open(bvimk *,cchar *,int,mode_t) noex ;
extern int	bvimk_add(bvimk *,bvimk_ver *) noex ;
extern int	bvimk_abort(bvimk *,int) noex ;
extern int	bvimk_info(bvimk *,nbimk_ver *) noex ;
extern int	bvimk_close(bvimk *) noex ;

EXTERNC_end


#endif /* BVIMK_INCLUDE */


