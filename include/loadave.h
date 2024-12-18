/* loadave HEADER */
/* lang=C20 */

/* loadave operations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LOADAVE_INCLUDE
#define	LOADAVE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<kstat.h>
#include	<usystem.h>
#include	<localmisc.h>


/* object defines */
#define	LOADAVE		struct loadave_head
#define	LOADAVE_VALUES	struct loadave_values
#define	LOADAVE_MID	struct loadave_mid

#define	LOADAVE_IDLEN		31


struct loadave_mid {
	time_t		tim_read ;		/* time of read */
	uint		hostid ;
	char		provider[LOADAVE_IDLEN + 1] ; /* vendor name? */
	char		serial[LOADAVE_IDLEN + 1] ; /* vendor serial number */
} ;

struct loadave_values {
	time_t		tim_read ;	/* time of read */
	time_t		tim_boot ;	/* boot time of machine */
	uint		ncpu ;
	uint		nproc ;
	uint		la1min ;
	uint		la5min ;
	uint		la15min ;
} ;

struct loadave_flags {
	uint		open:1 ;
} ;

struct loadave_head {
	uint		magic ;
	kstat_ctl_t	*kcp ;		/* the KSTAT chain pointer */
	kstat_t		*ksp ;		/* a KSTAT pointer */
	struct loadave_flags	f ;
	struct loadave_values	v ;	/* cached values */
	struct loadave_mid	mid ;	/* machine ID information */
	time_t		tim_open ;	/* time of KSTAT open */
	time_t		tim_update ;	/* time of last KSTAT chain update */
	time_t		tim_access ;	/* time of last read */
} ;


EXTERNC_begin

extern int loadave_start(LOADAVE *) noex ;
extern int loadave_readvalues(LOADAVE *,LOADAVE_VALUES *) noex ;
extern int loadave_readmid(LOADAVE *,LOADAVE_MID *) noex ;
extern int loadave_check(LOADAVE *,time_t) noex ;
extern int loadave_finish(LOADAVE *) noex ;

EXTERNC_end


#endif /* LOADAVE_INCLUDE */


