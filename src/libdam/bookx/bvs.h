/* bvs HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Bible Verse Structure */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVS_INCLUDE
#define	BVS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBUC */

#include	<bvses.h>


#define	BVS		struct bvs_head
#define	BVS_FL		struct bvs_flags
#define	BVS_DA		struct bvs_data
#define	BVS_V		struct bvs_verse
#define	BVS_INFO	struct bvs_information
#define	BVS_CA		struct bvs_calls
#define	BVS_MAGIC	0x97677246


struct bvs_flags {
        uint		modload:1 ;
} ; /* end struct */

struct bvs_verse {
	uchar		b, c, v ;
} ; /* end struct */

struct bvs_information {
	time_t		ctime ;
	time_t		mtime ;
	uint		nzbooks ;		/* number of non-zero books */
	uint		nbooks ;
	uint		nchapters ;
	uint		nverses ;
	uint		nzverses ;
} ; /* end struct */

struct bvs_head {
	modload		*mlp ;		/* module-load-pointer */
	void		*obj ;		/* object pointer */
	void		*callp ;
	BVS_FL		fl ;
	uint		magval ;
	int		objsz ;
	int		cursz ;
} ; /* end struct */

typedef	BVS		bvs ;
typedef	BVS_FL		bvs_fl ;
typedef	BVS_DA		bvs_da ;
typedef	BVS_V		bvs_v ;
typedef	BVS_INFO	bvs_info ;
typedef	BVS_CA		bvs_ca ;

EXTERNC_begin

extern int	bvs_open	(bvs *,cchar *,cchar *) noex ;
extern int	bvs_count	(bvs *) noex ;
extern int	bvs_getinfo	(bvs *,bvs_info *) noex ;
extern int	bvs_mkmodq	(bvs *,bvs_v *,int) noex ;
extern int	bvs_audit	(bvs *) noex ;
extern int	bvs_close	(bvs *) noex ;

EXTERNC_end


#endif /* BVS_INCLUDE */


