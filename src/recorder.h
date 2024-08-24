/* recorder HEADER */
/* lang=C20 */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RECORDER_INCLUDE
#define	RECORDER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	RECORDER		struct recorder_head
#define	RECORDER_ENT		struct recorder_e
#define	RECORDER_INFO		struct recorder_i
#define	RECORDER_MAGIC		0x12856734
#define	RECORDER_STARTNUM	100	/* starting number records */
#define	RECORDER_NINDICES	5
#define	RECORDER_NCOLLISIONS	10

/* options */
#define	RECORDER_OSEC		(1<<0)
#define	RECORDER_ORANDLC	(1<<1)


struct recorder_i {
	uint		cden[RECORDER_NINDICES][RECORDER_NCOLLISIONS] ;
	uint		c_l1 ;
	uint		c_l3 ;
	uint		c_f ;
	uint		c_fl3 ;
	uint		c_un ;
	uint		ilen ;		/* index length */
} ;

struct recorder_e {
	uint		username ;
	uint		last ;
	uint		first ;
	uint		m1 ;
	uint		m2 ;
} ;

struct recorder_head {
	RECORDER_ENT	*rectab ;
	RECORDER_INFO	s ;
	uint		magic ;
	int		i ;		/* current length */
	int		e ;		/* current buffer extent */
	int		c ;		/* count */
	int		opts ;
} ;

EXTERNC_begin

extern int	recorder_start(RECORDER *,int,int) noex ;
extern int	recorder_finish(RECORDER *) noex ;
extern int	recorder_add(RECORDER *,RECORDER_ENT *) noex ;
extern int	recorder_already(RECORDER *,RECORDER_ENT *) noex ;
extern int	recorder_gettab(RECORDER *,RECORDER_ENT **) noex ;
extern int	recorder_rtlen(RECORDER *) noex ;
extern int	recorder_count(RECORDER *) noex ;
extern int	recorder_indlen(RECORDER *) noex ;
extern int	recorder_indsize(RECORDER *) noex ;
extern int	recorder_mkindun(RECORDER *,cchar *,uint (*)[2],int) noex ;
extern int	recorder_mkindl1(RECORDER *,cchar *,uint (*)[2],int) noex ;
extern int	recorder_mkindl3(RECORDER *,cchar *,uint (*)[2],int) noex ;
extern int	recorder_mkindf(RECORDER *,cchar *,uint (*)[2],int) noex ;
extern int	recorder_mkindfl3(RECORDER *,cchar *,uint (*)[2],int) noex ;
extern int	recorder_info(RECORDER *,RECORDER_INFO *) noex ;

EXTERNC_end


#endif /* RECORDER_INCLUDE */


