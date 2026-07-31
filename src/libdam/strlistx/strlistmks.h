/* strlistmks HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a STRLIST database */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRLISTMKS_INCLUDE
#define	STRLISTMKS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® systems IDs */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strtab.h>		/* LIBUC */
#include	<srectab.h>		/* LIBUC */


#define	STRLISTMKS		struct strlistmks_head
#define	STRLISTMKS_OBJ		struct strlistmks_object
#define	STRLISTMKS_FL		struct strlistmks_flags
#define	STRLISTMKS_MAGIC	0x88773423
#define	STRLISTMKS_NENTRIES	(2 * 1024)
#define	STRLISTMKS_VERSION	0


#ifdef	__cplusplus
struct strlistmks_params {
	static constexpr int	magic	= 0x88773423 ;
	static constexpr int	nents	= (2 * 1024) ;
	static constexpr int	version	= 0 ;
} ; /* end struct */
#endif /* __cplusplus */

struct strlistmks_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct strlistmks_flags {
	uint		viopen:1 ;
	uint		abort:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		none:1 ;
	uint		inprogress:1 ;
	uint		fcreated:1 ;
} ; /* end struct */

struct strlistmks_head {
	cchar 		*dbname ;	/* data-base name */
	cchar		*idname ;	/* index-directory-name */
	char		*nfname ;	/* new-file-name */
	strtab		*stp ;		/* string-tab-pointer */
	srectab		*rtp ;		/* string-record-table-pointer */
	gid_t		gid ;
	STRLISTMKS_FL	fl ;
	uint		magval ;
	int		nstrs ;
	int		nfd ;
	mode_t		om ;
} ; /* end struct */

typedef	STRLISTMKS	strlistmks ;
typedef	STRLISTMKS_FL	strlistmks_fl ;
typedef	STRLISTMKS_OBJ	strlistmks_obj ;

EXTERNC_begin

extern int strlistmks_open	(strlistmks *,cchar *,int,mode_t,int) noex ;
extern int strlistmks_addvar	(strlistmks *,cchar *,int) noex ;
extern int strlistmks_abort	(strlistmks *) noex ;
extern int strlistmks_chgrp	(strlistmks *,gid_t) noex ;
extern int strlistmks_close	(strlistmks *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
local inline int strlistmks_magic(strlistmks *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == STRLISTMKS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (strlistmks_magic) */

#endif /* __cplusplus */

extern const strlistmks_obj	strlistmks_modinfo ;

#ifdef	__cplusplus
extern const strlistmks_params strlistmks_param ;
#endif /* __cplusplus */


#endif /* STRLISTMKS_INCLUDE */


