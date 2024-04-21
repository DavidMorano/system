/* motd HEADER */
/* lang=C20 */

/* object to help (manage) MOTD messages */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MOTD_INCLUDE
#define	MOTD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/param.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ptm.h>
#include	<lockrw.h>
#include	<paramfile.h>
#include	<finduid.h>
#include	<vechand.h>
#include	<localmisc.h>


#define	MOTD_MAGIC	0x75648941
#define	MOTD		struct motd_head
#define	MOTD_ID		struct motd_ident
#define	MOTD_MAPPER	struct motd_mapper
#define	MOTD_FL		struct motd_flags


struct motd_ident {
	cchar		*groupname ;
	cchar		*username ;
	uid_t		uid ;
	gid_t		gid ;
} ;

struct motd_mapper {
	LOCKRW		rwm ;
	PARAMFILE	dirsfile ;
	vechand		mapdirs ;
	cchar		*fname ;
	time_t		ti_mtime ;
	time_t		ti_check ;
	uint		magic ;
} ;

struct motd_flags {
	uint		ufind:1 ;
} ;

struct motd_head {
	MOTD_FL		open ;
	MOTD_MAPPER	mapper ;
	PTM		m ;		/* this is for all of the data */
	FINDUID		ufind ;
	cchar		**envv ;
	cchar		*pr ;
	cchar		*fe ;		/* file-ending */
	time_t		ti_lastcheck ;
	uint		magic ;
	int		nmaps ;
	int		nenv ;
} ;

typedef	MOTD		motd ;
typedef	MOTD_FL		motd_fl ;
typedef	MOTD_ID		motd_id ;

EXTERNC_begin

extern int	motd_open(motd *,cchar *) noex ;
extern int	motd_check(motd *,time_t) noex ;
extern int	motd_process(motd *,cchar *,cchar **,int) noex ;
extern int	motd_processid(motd *,motd_id *,cchar **,int) noex ;
extern int	motd_close(motd *) noex ;

extern int	motdid_load(motd_id *,cchar *,cchar *,uid_t,gid_t) noex ;

EXTERNC_end


#endif /* MOTD_INCLUDE */


