/* motd HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MOTD_INCLUDE
#define	MOTD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<ptm.h>
#include	<lockrw.h>
#include	<paramfile.h>
#include	<finduid.h>
#include	<vechand.h>
#include	<localmisc.h>


#define	MOTD_MAGIC	0x75648941
#define	MOTD		struct motd_head
#define	MOTD_ID		struct motd_id
#define	MOTD_MAPPER	struct motd_mapper
#define	MOTD_FL		struct motd_flags


struct motd_id {
	cchar		*groupname ;
	cchar		*username ;
	uid_t		uid ;
	gid_t		gid ;
} ;

struct motd_mapper {
	uint		magic ;
	LOCKRW		rwm ;
	PARAMFILE	dirsfile ;
	vechand		mapdirs ;
	cchar	*fname ;
	time_t		ti_mtime ;
	time_t		ti_check ;
} ;

struct motd_flags {
	uint		ufind:1 ;
} ;

struct motd_head {
	uint		magic ;
	MOTD_FL		open ;
	MOTD_MAPPER	mapper ;
	PTM		m ;		/* this is for all of the data */
	FINDUID		ufind ;
	cchar		**envv ;
	cchar		*pr ;
	cchar		*fe ;		/* file-ending */
	time_t		ti_lastcheck ;
	int		nmaps ;
	int		nenv ;
} ;

typedef	MOTD		motd ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	motd_open(MOTD *,cchar *) noex ;
extern int	motd_check(MOTD *,time_t) noex ;
extern int	motd_process(MOTD *,cchar *,cchar **,int) noex ;
extern int	motd_processid(MOTD *,MOTD_ID *,cchar **,int) noex ;
extern int	motd_close(MOTD *) noex ;

extern int	motdid_load(MOTD_ID *,cchar *,cchar *,uid_t,gid_t) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MOTD_INCLUDE */


