/* pcsclient HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perfmrm functions to be a PCS client to the PCS server daemon */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSCLIENT_INCLUDE
#define	PCSCLIENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	PCSCLIENT		struct pcsclient_head
#define	PCSCLIENT_FL		struct pcsclient_flags
#define	PCSCLIENT_OBJ		struct pcsclient_object
#define	PCSCLIENT_DA		struct pcsclient_data
#define	PCSCLIENT_MAGIC		0x58261227


struct pcsclient_object {
	char		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct pcsclient_data {
	uint		intstale ;
	uint		utime ;
	uint		btime ;
	uint		ncpu ;
	uint		nproc ;
	uint		la[3] ;
} ; /* end struct */

struct pcsclient_flags {
	uint		shm:1 ;			/* initialized */
} ; /* end struct */

struct pcsclient_head {
	cchar		*pr ;
	cchar		*shmname ;
	char		*mapdata ;
	uint		*shmtable ;
	csize		mapsize ;
	time_t		dt ;
	time_t		ti_shm ;		/* DB file modification */
	time_t		ti_map ;		/* DB map */
	time_t		ti_lastcheck ;		/* last check of file */
	PCSCLIENT_FL	fl ;
	uint		magval ;
	int		nodenamelen ;
	int		pagesz ;
	int		shmsz ;
} ; /* end struct */

typedef PCSCLIENT	pcsclient ;
typedef PCSCLIENT_FL	pcsclient_fl ;
typedef PCSCLIENT_OBJ	pcsclient_obj ;
typedef PCSCLIENT_DA	pcsclient_da ;

EXTERNC_begin

extern int	pcsclient_open(pcsclient *,cchar *) noex ;
extern int	pcsclient_get(pcsclient *,time_t,int,pcsclient_da *) noex ;
extern int	pcsclient_close(pcsclient *) noex ;

EXTERNC_end


#endif /* PCSCLIENT_INCLUDE */


