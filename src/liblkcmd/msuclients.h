/* msuclients HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* object to interact with the MSU server */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	MSUCLIENTS_INCLUDE
#define	MSUCLIENTS_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sockaddress.h>


#define	MSUCLIENTS_MAGIC	0x58261221
#define	MSUCLIENTS		struct msuclients_head
#define	MSUCLIENTS_FL		struct msuclients_flags
#define	MSUCLIENTS_OBJ		struct msuclients_object
#define	MSUCLIENTS_DA		struct msuclients_data


struct msuclients_object {
	char		*name ;
	uint		objsize ;
} ;

struct msuclients_data {
	uint		pid ;		/* server PID */
	uint		utime ;		/* last update time */
	uint		btime ;		/* SYSMISC boot-time */
	uint		ncpu ;		/* SYSMISC ncpu */
	uint		nproc ;		/* SYSMISC nproc */
	uint		la[3] ;		/* SYSMISC load-averages */
} ;

struct msuclients_flags {
	uint		srv : 1 ;	/* initialized */
} ;

struct msuclients_head {
	cchar		*pr ;
	cchar		*tmpourdname ;
	cchar		*clientfname ;
	cchar		*reqfname ;
	SOCKADDRESS	srv ;		/* server address */
	time_t		dt ;
	MSUCLIENTS_FL	fl ;
	uint		magic ;
	int		srvlen ;	/* server address length */
	int		fd ;
	int		to ;
} ;

typedef	MSUCLIENTS	msuclients ;
typedef	MSUCLIENTS_FL	msuclients_fl ;
typedef	MSUCLIENTS_OBJ	msuclients_obj ;
typedef	MSUCLIENTS_DA	msuclients_da ;

EXTERNC_begin

extern int	msuclients_open(msuclients *,cchar *,cchar *,int) noex ;
extern int	msuclients_status(msuclients *) noex ;
extern int	msuclients_get(msuclients *,time_t,msucliens_da *) noex ;
extern int	msuclients_close(msuclients *) noex ;

EXTERNC_end


#endif /* MSUCLIENTS_INCLUDE */


