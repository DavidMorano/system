/* loadave HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* load-average gathering and preparation */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-12-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	LOADAVE_INCLUDE
#define	LOADAVE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


/* object defines */
#define	LOADAVE			struct loadave_head
#define	LOADAVE_FL		struct loadave_flags
#define	LOADAVE_MID		struct loadave_machid 
#define	LOADAVE_VAL		struct loadave_value
#define	LOADAVE_MAGIC		0x16865934
#define	LOADAVE_IDLEN		31


struct loadave_machid {
	time_t		tim_read ;	/* time of read */
	uint		hostid ;
	char		provider[LOADAVE_IDLEN + 1] ;	/* vendor name */
	char		serial	[LOADAVE_IDLEN + 1] ;	/* vendor serial */
} ; /* end struct */

struct loadave_value {
	time_t		tim_boot ;	/* boot time of machine */
	time_t		tim_la ;	/* load-average */
	time_t		tim_ncpu ;	/* n-cpu */
	time_t		tim_nprocs ;	/* n-procs */
	uint		ncpu ;		/* number CPUs */
	uint		nprocs ;	/* number of processes */
	uint		la1min ;
	uint		la5min ;
	uint		la15min ;
} ; /* end struct */

struct loadave_flags {
	uint		open:1 ;
} ; /* end struct */

struct loadave_head {
	LOADAVE_MID	*midp ;		/* machine ID information */
	LOADAVE_VAL	*valp ;		/* cached values */
	time_t		tim_open ;	/* time of KSTAT open */
	time_t		tim_read ;	/* time of last read */
	LOADAVE_FL	fl ;
	uint		magval ;
} ; /* end struct */

typedef	LOADAVE			loadave ;
typedef	LOADAVE_FL		loadave_fl ;
typedef	LOADAVE_MID		loadave_mid ;
typedef	LOADAVE_VAL		loadave_val ;

EXTERNC_begin

extern int loadave_start	(loadave *) noex ;
extern int loadave_readmid	(loadave *,loadave_mid *) noex ;
extern int loadave_readval	(loadave *,loadave_val *) noex ;
extern int loadave_check	(loadave *,time_t) noex ;
extern int loadave_finish	(loadave *) noex ;

EXTERNC_end


#endif /* LOADAVE_INCLUDE */


