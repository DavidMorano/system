/* bpimk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BPIMK_INCLUDE
#define	BPIMK_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>


#define	BPIMK_MAGIC	0x88773423
#define	BPIMK		struct bpimk_head
#define	BPIMK_OBJ	struct bpimk_object
#define	BPIMK_VER	struct bpimk_verser
#define	BPIMK_INFO	struct bpimk_information
#define	BPIMK_FL	struct bpimk_flags
#define	BPIMK_INTOPEN	(10*60)
#define	BPIMK_INTSTALE	(5*60)


struct bpimk_object {
	cchar		*name ;
	uint		objsize ;
} ;

struct bpimk_information {
	uint		maxbook ;
	uint		maxchapter ;
	uint		maxverse ;
	uint		nverses ;
	uint		nzverses ;
} ;

struct bpimk_verser {
	uchar		nlines, b, c, v ;
} ;

struct bpimk_flags {
	uint		notsorted:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
	uint		abort:1 ;
} ;

struct bpimk_head {
	uint		magic ;
	cchar 		*dbname ;
	cchar		*idname ;
	char		*nidxfname ;
	vecobj		verses ;
	BPIMK_FL	fl ;
	uint		pcitation ;
	uint		maxbook ;
	uint		maxchapter ;
	uint		maxverse ;
	uint		nverses ;
	uint		nzverses ;
	int		nfd ;
	mode_t		om ;
} ;

typedef BPIMK		bpimk ;
typedef	BPIMK_FL	bpimk_fl ;
typedef	BPIMK_OBJ	bpimk_obj ;
typedef	BPIMK_VER	bpimk_ver ;
typedef	BPIMK_INFO	bpimk_info ;

EXTERNC_begin

extern int	bpimk_open(bpimk *,cchar *,int,mode_t) noex ;
extern int	bpimk_add(bpimk *,bpimk_ver *) noex ;
extern int	bpimk_abort(bpimk *,int) noex ;
extern int	bpimk_info(bpimk *,bpimk_info *) noex ;
extern int	bpimk_close(bpimk *) noex ;

EXTERNC_end


#endif /* BPIMK_INCLUDE */


