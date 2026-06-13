/* bpimk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BPIMK_INCLUDE
#define	BPIMK_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */


#define	BPIMK		struct bpimk_head
#define	BPIMK_OBJ	struct bpimk_object
#define	BPIMK_FL	struct bpimk_flags
#define	BPIMK_V		struct bpimk_verse
#define	BPIMK_INFO	struct bpimk_information
#define	BPIMK_MAGIC	0x88773423
#define	BPIMK_INTOPEN	(10*60)
#define	BPIMK_INTSTALE	(5*60)


struct bpimk_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct bpimk_information {
	uint		maxbook ;
	uint		maxchapter ;
	uint		maxverse ;
	uint		nverses ;
	uint		nzverses ;
} ; /* end struct */

struct bpimk_verse {
	uchar		nlines, b, c, v ;
} ; /* end struct */

struct bpimk_flags {
	uint		notsorted:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
	uint		abort:1 ;
} ; /* end struct */

struct bpimk_head {
	cchar 		*dbname ;
	cchar		*idname ;
	char		*nidxfname ;
	vecobj		*vlp ;		/* verse-list-pointer */
	BPIMK_FL	fl ;
	uint		magval ;
	uint		pcitation ;
	uint		maxbook ;
	uint		maxchapter ;
	uint		maxverse ;
	uint		nverses ;
	uint		nzverses ;
	int		nfd ;
	mode_t		om ;
} ; /* end struct (bpimk_head) */

typedef BPIMK		bpimk ;
typedef	BPIMK_FL	bpimk_fl ;
typedef	BPIMK_OBJ	bpimk_obj ;
typedef	BPIMK_V		bpimk_v ;
typedef	BPIMK_INFO	bpimk_info ;

EXTERNC_begin

extern int	bpimk_open	(bpimk *,cchar *,int,mode_t) noex ;
extern int	bpimk_add	(bpimk *,bpimk_v *) noex ;
extern int	bpimk_abort	(bpimk *,int) noex ;
extern int	bpimk_getinfo	(bpimk *,bpimk_info *) noex ;
extern int	bpimk_close	(bpimk *) noex ;

EXTERNC_end

extern const bpimk_obj		bpimk_modinfo ;


#endif /* BPIMK_INCLUDE */


