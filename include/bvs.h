/* bvs HEADER */
/* encoding=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>

#include	<bvses.h>


#define	BVS_MAGIC	0x97677246
#define	BVS		struct bvs_head
#define	BVS_DA		struct bvs_data
#define	BVS_V		struct bvs_verse
#define	BVS_INFO	struct bvs_information
#define	BVS_CA		struct bvs_calls


struct bvs_verse {
	uchar		b, c, v ;
} ;

struct bvs_information {
	time_t		ctime ;
	time_t		mtime ;
	uint		nzbooks ;		/* number of non-zero books */
	uint		nbooks ;
	uint		nchapters ;
	uint		nverses ;
	uint		nzverses ;
} ;

EXTERNC_begin
struct bvs_calls {
	int	(*open)(void *,cchar *,cchar *) noex ;
	int	(*count)(void *) noex ;
	int	(*info)(void *,BVSES_INFO *) noex ;
	int	(*mkmodquery)(void *,BVSES_V *,int) noex ;
	int	(*audit)(void *) ;
	int	(*close)(void *) ;
} ;
EXTERNC_end

struct bvs_head {
	void		*obj ;		/* object pointer */
	modload		loader ;
	BVS_CA		call ;
	uint		magic ;
} ;

typedef	BVS		bvs ;
typedef	BVS_DA		bvs_da ;
typedef	BVS_V		bvs_v ;
typedef	BVS_INFO	bvs_info ;
typedef	BVS_CA		bvs_ca ;

EXTERNC_begin

extern int	bvs_open(bvs *,cchar *,cchar *) noex ;
extern int	bvs_count(bvs *) noex ;
extern int	bvs_info(bvs *,bvs_info *) noex ;
extern int	bvs_mkmodquery(bvs *,bvs_v *,int) noex ;
extern int	bvs_audit(bvs *) noex ;
extern int	bvs_close(bvs *) noex ;

EXTERNC_end


#endif /* BVS_INCLUDE */


