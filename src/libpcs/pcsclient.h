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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	PCSCLIENT_MAGIC		0x58261227
#define	PCSCLIENT		struct pcsclient_head
#define	PCSCLIENT_OBJ		struct pcsclient_obj
#define	PCSCLIENT_DATA		struct pcsclient_d
#define	PCSCLIENT_FL		struct pcsclient_flags


struct pcsclient_obj {
	char		*name ;
	uint		objsize ;
} ;

struct pcsclient_d {
	uint		intstale ;
	uint		utime ;
	uint		btime ;
	uint		ncpu ;
	uint		nproc ;
	uint		la[3] ;
} ;

struct pcsclient_flags {
	uint		shm:1 ;			/* initialized */
} ;

struct pcsclient_head {
	cchar		*pr ;
	cchar		*shmname ;
	char		*mapdata ;
	uint		*shmtable ;
	time_t		dt ;
	time_t		ti_shm ;		/* DB file modification */
	time_t		ti_map ;		/* DB map */
	time_t		ti_lastcheck ;		/* last check of file */
	PCSCLIENT_FL	fl ;
	uint		magic ;
	int		nodenamelen ;
	int		pagesize ;
	int		mapsize ;
	int		shmsize ;
} ;

typedef PCSCLIENT	pcsclient ;
typedef PCSCLIENT_DATA	pcsclient_data ;

EXTERNC_begin

extern int	pcsclient_open(pcsclient *,cchar *) noex ;
extern int	pcsclient_get(pcsclient *,time_t,int,pcsclient_data *) noex ;
extern int	pcsclient_close(pcsclient *) noex ;

EXTERNC_end


#endif /* PCSCLIENT_INCLUDE */


