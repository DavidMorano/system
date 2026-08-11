/* babycalcs HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* baby calculator */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BABYCALCS_INCLUDE
#define	BABYCALCS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |time_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<babieshdr.h>		/* LIBDAM */


#define	BABYCALCS		struct babycalcs_head
#define	BABYCALCS_FL		struct babycalcs_flags
#define	BABYCALCS_OBJ		struct babycalcs_object
#define	BABYCALCS_INFO		struct babycalcs_information
#define	BABYCALCS_ENT		struct babycalcs_entry
#define	BABYCALCS_DBNAME	"babies"
#define	BABYCALCS_MAGIC		0x43628199


struct babycalcs_flags {
	uint		shm:1 ;
	uint		txt:1 ;
	uint		sorted:1 ;
	uint		needinit:1 ;
} ; /* end struct */

struct babycalcs_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct babycalcs_information {
	time_t		wtime ;
	time_t		atime ;
	uint		acount ;
} ; /* end struct */

struct babycalcs_entry {
	time_t		date ;
	uint		count ;
} ; /* end struct */

struct babycalcs_head {
	BABYCALCS_ENT	*table ;
	cchar		*pr ;
	cchar		*dbfname ;
	cchar		*shmname ;
	caddr_t		mapdata ;	/* SHM data */
	ptm		*mxp ;		/* pointer to SHM mutex */
	BABIESHDR	hf ;
	time_t		ti_mdb ;	/* db-mtime */
	time_t		ti_map ;	/* map-time */
	time_t		ti_lastcheck ;
	size_t		mapsize ;	/* SHM map-size */
	BABYCALCS_FL	fl ;
	uint		magval ;
	int		pagesz ;
	int		dbsz ;
	int		shmsz ;
	int		nentries ;	
} ; /* end struct */

typedef	BABYCALCS		babycalcs ;
typedef	BABYCALCS_FL		babycalcs_fl ;
typedef	BABYCALCS_OBJ		babycalcs_obj ;
typedef	BABYCALCS_INFO		babycalcs_info ;
typedef	BABYCALCS_ENT		babycalcs_ent ;

EXTERNC_begin

extern int	babycalcs_open		(babycalcs *,cchar *,cchar *) noex ;
extern int	babycalcs_check		(babycalcs *,time_t) noex ;
extern int	babycalcs_lookup	(babycalcs *,time_t,uint *) noex ;
extern int	babycalcs_getinfo	(babycalcs *,babycalcs_info *) noex ;
extern int	babycalcs_close		(babycalcs *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const babycalcs_obj	babycalcs_modinfo ;
#endif /* __cplusplus */


#endif /* BABYCALCS_INCLUDE */


