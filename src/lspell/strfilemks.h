/* strfilemks */
/* lang=C20 */

/* make a STRFILE database */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRFILEMKS_INCLUDE
#define	STRFILEMKS_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<localmisc.h>


#define	STRFILEMKS		struct strfilemks_head
#define	STRFILEMKS_OBJ		struct strfilemks_obj
#define	STRFILEMKS_FL		struct strfilemks_flags
#define	STRFILEMKS_IMAP		struct strfilemks_imap
#define	STRFILEMKS_MAGIC	0x88773423
#define	STEFILEMKS_MAXFILESIZE	(10*1024*1024)
#define	STRFILEMKS_NENTRIES	(2 * 1024)


struct strfilemks_obj {
	char		*name ;
	uint		objsize ;
} ;

struct strfilemks_flags {
	uint		viopen:1 ;
	uint		abort:1 ;
	uint		creat:1 ;
	uint		excl:1 ;
	uint		none:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
} ;

struct strfilemks_imap {
	void		*mdata ;
	size_t		msize ;
} ;

struct strfilemks_head {
	uint		magic ;
	const char 	*dbname ;
	const char	*idname ;
	char		*nfname ;
	void		*recorder ;
	void		*idx ;
	STRFILEMKS_IMAP	imap ;
	STRFILEMKS_FL	f ;
	mode_t		om ;
	gid_t		gid ;
	int		pagesize ;
	int		nstrs ;
	int		nfd ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	strfilemks_open(STRFILEMKS *,const char *,int,mode_t,int) ;
extern int	strfilemks_addfile(STRFILEMKS *,const char *,int) ;
extern int	strfilemks_abort(STRFILEMKS *) ;
extern int	strfilemks_chgrp(STRFILEMKS *,gid_t) ;
extern int	strfilemks_close(STRFILEMKS *) ;

#ifdef	__cplusplus
}
#endif


#endif /* STRFILEMKS_INCLUDE */


