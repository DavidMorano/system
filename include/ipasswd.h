/* ipasswd HEADER */
/* lang=C20 */

/* indexed PASSWD GECOS DB */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-31, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	IPASSWD_INCLUDE
#define	IPASSWD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<realname.h>
#include	<localmisc.h>


#define	IPASSWD			struct ipasswd_head
#define	IPASSWD_CUR		struct ipasswd_cursor
#define	IPASSWD_INFO		struct ipasswd_information
#define	IPASSWD_ENT		struct ipasswd_entry
#define	IPASSWD_OBJ		struct ipasswd_obj
#define	IPASSWD_FL		struct ipasswd_flags

#define	IPASSWD_SUF		"pwi"
#define	IPASSWD_MAGIC		0x23456787
#define	IPASSWD_CURMAGIC	0x23456788

#define	IPASSWD_USERNAMELEN	32
#define	IPASSWD_NINDICES	5	/* number of indices */

/* database ("read") options */
#define	IPASSWD_ROSEC		0x01	/* use second hash */
#define	IPASSWD_RORANDLC	0x02	/* use 'randlc()' */

/* fetch options */
#define	IPASSWD_FOLASTFULL	0x01	/* require full last name for match */


struct ipasswd_obj {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct ipasswd_cursor {
	uint		magic ;
	int		wi ;
	int		i[IPASSWD_NINDICES] ;
} ;

struct ipasswd_information {
	time_t		writetime ;	/* time DB written */
	uint		writecount ;	/* write counter */
	uint		entries ;	/* total number of entries */
	uint		version ;
	uint		encoding ;
	uint		type ;
	uint		collisions ;
} ;

struct ipasswd_entry {
	uint		username ;
	uint		last ;
	uint		first ;
	uint		m1 ;
	uint		m2 ;
} ;

struct ipasswd_flags {
	uint		remote:1 ;
	uint		cursor:1 ;
	uint		cursorlockbroken:1 ;
	uint		cursoracc:1 ;
	uint		held:1 ;
	uint		lockedread:1 ;
	uint		lockedwrite:1 ;
} ;

struct ipasswd_head {
	cchar		*fname ;
	caddr_t		mapdata ;
	cchar		*stab ;
	uint		(*recind[IPASSWD_NINDICES])[2] ;
	IPASSWD_ENT	*rectab ;
	IPASSWD_FL	f ;
	time_t		mtime ;
	time_t		ti_open ;
	time_t		ti_access ;
	time_t		ti_map ;
	size_t		mapsize ;
	uint		magic ;
	uint		pagesize ;
	uint		filesize ;
	uint		stcount ;
	uint		stsize ;
	uint		rtlen ;
	uint		rtsize ;
	uint		rilen ;
	uint		collisions ;
	uint		ropts ;
	int		fd ;
	int		oflags ;
	int		operm ;
} ;

typedef IPASSWD			ipasswd ;
typedef IPASSWD_CUR		ipasswd_cur ;
typedef IPASSWD_INFO		ipasswd_info ;

EXTERNC_begin

extern int	ipasswd_open(ipasswd *,cchar *) noex ;
extern int	ipasswd_getinfo(ipasswd *,ipasswd_info *) noex ;
extern int	ipasswd_curbegin(ipasswd *,ipasswd_cur *) noex ;
extern int	ipasswd_curend(ipasswd *,ipasswd_cur *) noex ;
extern int	ipasswd_enum(ipasswd *,ipasswd_cur *,char *,cchar **,
			char *,int) noex ;
extern int	ipasswd_fetcher(ipasswd *,ipasswd_cur *,int,char *,cchar **,
			int) noex ;
extern int	ipasswd_fetch(ipasswd *,REALNAME *,ipasswd_cur *,
			int,char *) noex ;
extern int	ipasswd_close(ipasswd *) noex ;

EXTERNC_end


#endif /* IPASSWD_INCLUDE */


