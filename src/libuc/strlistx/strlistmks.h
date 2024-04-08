/* strlistmks HEADER */
/* lang=C20 */

/* make a STRLIST database */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRLISTMKS_INCLUDE
#define	STRLISTMKS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* systems IDs */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<strtab.h>
#include	<localmisc.h>


#define	STRLISTMKS		struct strlistmks_head
#define	STRLISTMKS_OBJ		struct strlistmks_object
#define	STRLISTMKS_REC		struct strlistmks_rectab
#define	STRLISTMKS_FL		struct strlistmks_flags
#define	STRLISTMKS_MAGIC	0x88773423
#define	STRLISTMKS_NENTRIES	(2 * 1024)
#define	STRLISTMKS_VERSION	0


struct strlistmks_object {
	cchar		*name ;
	uint		objsize ;
} ;

struct strlistmks_flags {
	uint		viopen:1 ;
	uint		abort:1 ;
	uint		creat:1 ;
	uint		excl:1 ;
	uint		none:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
} ;

struct strlistmks_rectab {
	uint		(*rectab)[1] ;
	int		i ;			/* highest index */
	int		n ;			/* extent */
} ;

struct strlistmks_head {
	cchar 		*dbname ;
	cchar		*idname ;
	char		*nfname ;
	strtab		strs ;
	STRLISTMKS_REC	rectab ;
	STRLISTMKS_FL	f ;
	gid_t		gid ;
	uint		magic ;
	int		nstrs ;
	int		nfd ;
	mode_t		om ;
} ;

typedef	STRLISTMKS	strlistmks ;
typedef	STRLISTMKS_OBJ	strlistmks_obj ;
typedef	STRLISTMKS_REC	strlistmks_rec ;
typedef	STRLISTMKS_FL	strlistmks_fl ;

EXTERNC_begin

extern int	strlistmks_open(strlistmks *,cchar *,int,mode_t,int) noex ;
extern int	strlistmks_addvar(strlistmks *,cchar *,int) noex ;
extern int	strlistmks_abort(strlistmks *) noex ;
extern int	strlistmks_chgrp(strlistmks *,gid_t) noex ;
extern int	strlistmks_close(strlistmks *) noex ;

EXTERNC_end


#endif /* STRLISTMKS_INCLUDE */


