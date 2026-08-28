/* pcsunsfh HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#ifndef	PCSUNSFH_INCLUDE
#define	PCSUNSFH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	PCSUNSFH		struct pcsunsfh
#define	PCSUNSFH_MAGICSTR	"PCSUNS"
#define	PCSUNSFH_MAGICLEN	szof(PCSUNSFH_MAGICSTR)
#define	PCSUNSFH_MAGICSZ	16
#define	PCSUNSFH_VERSION	0
#define	PCSUNSFH_IDLEN		20


enum pcsunsfhhs {
	pcsunsfhh_shmsz,
	pcsunsfhh_dbsz,
	pcsunsfhh_dbtime,
	pcsunsfhh_wtime,
	pcsunsfhh_atime,
	pcsunsfhh_acount,
	pcsunsfhh_muoff,
	pcsunsfhh_musz,
	pcsunsfhh_btoff,
	pcsunsfhh_btlen,
	pcsunsfhh_overlast
} ; /* end enum */

struct pcsunsfh {
	uint		shmsz ;
	uint		dbsz ;
	uint		dbtime ;
	uint		wtime ;
	uint		atime ;
	uint		acount ;
	uint		muoff ;
	uint		musz ;
	uint		btoff ;
	uint		btlen ;
	uchar		vetu[4] ;
} ; /* end struct */

EXTERNC_begin

extern int pcsunsfh(PCSUNSFH *,int,char *,int) noex ;

EXTERNC_end


#endif /* PCSUNSFH_INCLUDE */


