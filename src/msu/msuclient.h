/* msuclient HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	MSUCLIENT_INCLUDE
#define	MSUCLIENT_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	MSUCLIENT_MAGIC		0x58261227
#define	MSUCLIENT		struct msuclient_head
#define	MSUCLIENT_FL		struct msuclient_flags
#define	MSUCLIENT_OBJ		struct msuclient_object
#define	MSUCLIENT_DA		struct msuclient_data


struct msuclient_objext {
	char		*name ;
	uint		objsize ;
} ;

struct msuclient_data {
	uint		intstale ;
	uint		utime ;
	uint		btime ;
	uint		ncpu ;
	uint		nproc ;
	uint		la[3] ;
} ;

struct msuclient_flags {
	uint		shm:1 ;			/* initialized */
} ;

struct msuclient_head {
	cchar		*pr ;
	cchar		*shmname ;
	char		*mapdata ;
	uint		*shmtable ;
	time_t		dt ;
	time_t		ti_shm ;		/* DB file modification */
	time_t		ti_map ;		/* DB map */
	time_t		ti_lastcheck ;		/* last check of file */
	MSUCLIENT_FL	fl ;
	uint		magic ;
	int		nodenamelen ;
	int		pagesize ;
	int		mapsize ;
	int		shmsize ;
} ; /* end struct (msuclient_head) */

typedef	MSUCLIENT	msuclient ;
typedef	MSUCLIENT_FL	msuclient_fl ;
typedef	MSUCLIENT_OBJ	msuclient_obj ;
typedef	MSUCLIENT_DA	msuclient_da ;

EXTERNC_begin

extern int	msuclient_open(msuclient *,cchar *) noex ;
extern int	msuclient_get(msuclient *,time_t,int,msgclient_da *) noex ;
extern int	msuclient_close(msuclient *) noex ;

EXTERNC_end


#endif /* MSUCLIENT_INCLUDE */


