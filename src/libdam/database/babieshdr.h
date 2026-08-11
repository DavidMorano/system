/* babieshdr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* header management for BABIES shared-memory segment */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BABIESHDR_INCLUDE
#define	BABIESHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	BABIESHDR		struct babieshdr_head
#define	BABIESHDR_MAGICSIZE	16
#define	BABIESHDR_MAGICSTR	"BABIES"
#define	BABIESHDR_VERSION	0
#define	BABIESHDR_IDLEN		20


enum babieshdrhs {
	babieshdrh_shmsz,
	babieshdrh_dbsz,
	babieshdrh_dbtime,
	babieshdrh_wtime,
	babieshdrh_atime,
	babieshdrh_acount,		/* access count */
	babieshdrh_muoff,
	babieshdrh_musz,
	babieshdrh_btoff,
	babieshdrh_btlen,
	babieshdrh_overlast
} ; /* end enum */

struct babieshdr_head {
	uint		shmsz ;
	uint		dbsz ;
	uint		dbtime ;
	uint		wtime ;
	uint		atime ;		/* access time */
	uint		acount ;	/* access count */
	uint		muoff ;
	uint		musz ;
	uint		btoff ;
	uint		btlen ;
	uchar		vetu[4] ;
} ; /* end struct */

#ifdef	__cplusplus
struct babieshdr : babieshdr_head {
    	int rd		(char *,int) noex ;
    	int wr		(cchar *,int) noex ;
} ; /* end struct (bvshdr) */
#else /* __cplusplus */
typedef	BABIESHDR	babieshdr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	babieshdr_rd(babieshdr *,char *,int) noex ;
extern int	babieshdr_wr(babieshdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* BABIESHDR_INCLUDE */


