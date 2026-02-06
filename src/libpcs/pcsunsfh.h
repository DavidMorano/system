/* pcsunsfh HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#ifndef	PCSUNSFH_INCLUDE
#define	PCSUNSFH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<usysbase.h>


#define	PCSUNSFH		struct pcsunsfh
#define	PCSUNSFH_MAGICSTR	"PCSUNS"
#define	PCSUNSFH_MAGICLEN	sizeof(PCSUNSFH_MAGICSTR)
#define	PCSUNSFH_MAGICSIZE	16
#define	PCSUNSFH_VERSION	0
#define	PCSUNSFH_IDLEN		20


enum pcsunsfhhs {
	pcsunsfhh_shmsize,
	pcsunsfhh_dbsize,
	pcsunsfhh_dbtime,
	pcsunsfhh_wtime,
	pcsunsfhh_atime,
	pcsunsfhh_acount,
	pcsunsfhh_muoff,
	pcsunsfhh_musize,
	pcsunsfhh_btoff,
	pcsunsfhh_btlen,
	pcsunsfhh_overlast
} ;

struct pcsunsfh {
	uint		shmsize ;
	uint		dbsize ;
	uint		dbtime ;
	uint		wtime ;
	uint		atime ;
	uint		acount ;
	uint		muoff ;
	uint		musize ;
	uint		btoff ;
	uint		btlen ;
	uchar		vetu[4] ;
} ;

EXTERNC_begin

extern int pcsunsfh(PCSUNSFH *,int,char *,int) noex ;

EXTERNC_end


#endif /* PCSUNSFH_INCLUDE */


