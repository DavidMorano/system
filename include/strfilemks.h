/* strfilemks HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a STRFILE database */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	STRFILEMKS_INCLUDE
#define	STRFILEMKS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	STRFILEMKS		struct strfilemks_head
#define	STRFILEMKS_OBJ		struct strfilemks_object
#define	STRFILEMKS_FL		struct strfilemks_flags
#define	STRFILEMKS_IM		struct strfilemks_imap
#define	STRFILEMKS_MAGIC	0x88773423
#define	STEFILEMKS_MAXFILESIZE	(10*1024*1024)
#define	STRFILEMKS_NENTRIES	(2 * 1024)


struct strfilemks_object {
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
	cchar 		*dbname ;
	cchar		*idname ;
	char		*nfname ;
	void		*recorder ;
	void		*idx ;
	STRFILEMKS_IMAP	imap ;
	STRFILEMKS_FL	f ;
	gid_t		gid ;
	uint		magic ;
	int		pagesize ;
	int		nstrs ;
	int		nfd ;
	mode_t		om ;
} ;

typedef	STRFILEMKS	strfilemks ;
typedef	STRFILEMKS_OBJ	strfilemks_obj ;
typedef	STRFILEMKS_FL	strfilemks_fl ;
typedef	STRFILEMKS_IM	strfilemks_im ;

EXTERNC_begin

extern int	strfilemks_open(strfilemks *,cchar *,int,mode_t,int) noex ;
extern int	strfilemks_addfile(strfilemks *,cchar *,int) noex ;
extern int	strfilemks_abort(strfilemks *) noex ;
extern int	strfilemks_chgrp(strfilemks *,gid_t) noex ;
extern int	strfilemks_close(strfilemks *) noex ;

EXTERNC_end


#endif /* STRFILEMKS_INCLUDE */


