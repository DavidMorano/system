/* userinfo HEADER */
/* lang=C++20 */

/* get user information from various databases */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	USERINFO_INCLUDE
#define	USERINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<netdb.h>


#define	USERINFO		struct userinfo
#define	USERINFO_FL		struct userinfo_flags
#define	USERINFO_MAGIC		0x33216271
#define	USERINFO_LEN		((3 * 2048) + MAXHOSTNAMELEN)
#define	USERINFO_LOGIDLEN	15


struct userinfo_flags {
	unsigned int	sysv_rt:1 ;
	unsigned int	sysv_ct:1 ;
} ;

struct userinfo {
	uint		magic ;
	cchar		*sysname ;	/* UNAME OS system-name */
	cchar		*release ;	/* UNAME OS release */
	cchar		*version ;	/* UNAME OS version */
	cchar		*machine ;	/* UNAME machine */
	cchar		*nodename ;	/* OS nodename (no domain) */
	cchar		*domainname ;	/* INET domain */
	cchar		*username ;	/* PASSWD username */
	cchar		*password ;	/* PASSWD password */
	cchar		*gecos ;	/* PASSWD GECOS field */
	cchar		*homedname ;	/* PASSWD (home) directory */
	cchar		*shell ;	/* PASSWD SHELL */
	cchar		*organization ;	/* GECOS organization */
	cchar		*gecosname ;	/* GECOS name */
	cchar		*account ;	/* GECOS account */
	cchar		*bin ;		/* GECOS printer-bin */
	cchar		*office ;	/* GECOS office */
	cchar		*wphone ;	/* GECOS work-phone */
	cchar		*hphone ;	/* GECOS home-phone */
	cchar		*printer ;	/* GECOS printer */
	cchar		*realname ;	/* processed GECOS-name */
	cchar		*mailname ;	/* best compacted mail-name */
	cchar		*fullname ;	/* best fullname */
	cchar		*name ;		/* best compacted name */
	cchar		*groupname ;	/* login groupname */
	cchar		*project ;	/* user project name */
	cchar		*tz ;		/* user time-zone */
	cchar		*md ;		/* mail-spool directory */
	cchar		*wstation ;	/* user weather-station */
	cchar		*logid ;	/* suggested ID for logging */
	cchar		*a ;		/* memory allocation */
	USERINFO_FL	f ;
	pid_t		pid ;
	uid_t		uid, euid ;
	gid_t		gid, egid ;
} ;

EXTERNC_begin

extern int userinfo_start(USERINFO *,cchar *) noex ;
extern int userinfo_finish(USERINFO *) noex ;
extern int userinfo(USERINFO *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* USERINFO_INCLUDE */


