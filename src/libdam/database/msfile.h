/* msfile HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate a MSFILE file */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	MSFILE_INCLUDE
#define	MSFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mapstrint.h>

#include	<msfilee.h>
#include	<ebuf.h>


/* object defines */
#define	MSFILE			struct msfile_head
#define	MSFILE_CUR		struct msfile_cursor
#define	MSFILE_ENT		MSFILEE_ALL
#define	MSFILE_FL		struct msfile_flags
#define	MSFILE_H		struct msfile_her

#define	MSFILE_MAGIC		918245634
#define	MSFILE_FILEMAGIC	"MS"
#define	MSFILE_FILEMAGICSIZE	16
#define	MSFILE_FILEMAGICLEN	sizeof(MSFILE_FILEMAGIC)
#define	MSFILE_FILEVERSION	0
#define	MSFILE_NODENAMELEN	MSFILEE_LNODENAME
#define	MSFILE_IDLEN		(16 + sizeof(uint))
#define	MSFILE_HEADTABLEN	(3 * sizeof(uint))
#define	MSFILE_TOPLEN		(MSFILE_IDLEN + MSFILE_HEADTABLEN)

/* entry flags */
#define	MSFILE_FLA		0x01	/* flag-loadaverage */
#define	MSFILE_FUSERS		0x02	/* flag-users */
#define	MSFILE_FPMAVAIL		0x04	/* flag-percent_memory_available */


struct msfile_her {
	uint		nentries ;
	uint		wtime ;
	uint		wcount ;
} ;

struct msfile_flags {
	uint		fileinit:1 ;		/* file init'ed */
	uint		writable:1 ;
	uint		lockedread:1 ;
	uint		lockedwrite:1 ;
	uint		cursorlockbroken:1 ;	/* cursor lock broken */
	uint		cursoracc:1 ;		/* accessed while cursored? */
	uint		remote:1 ;		/* remote mounted file */
	uint		bufvalid:1 ;		/* buffer valid */
	uint		ebuf:1 ;		/* EBUF active */
} ;

struct msfile_head {
	ebuf		ebm ;		/* entry-buffer-manager */
	mapstrint	ni ;		/* nodename index */
	cchar		*fname ;
	MSFILE_FL	f ;
	MSFILE_H	h ;
	time_t		ti_open ;		/* file open time */
	time_t		ti_access ;		/* file access time */
	time_t		ti_mod ;		/* file modification time */
	uint		magic ;
	int		oflags ;
	int		pagesize ;
	int		filesize ;
	int		topsize ;
	int		fd ;
	int		ncursors ;
	int		fileversion, fileencoding, filetype ;
	mode_t		operm ;
	char		topbuf[MSFILE_TOPLEN + 1] ;	/* top-buffer */
} ;

struct msfile_cursor {
	int		i ;
} ;

typedef MSFILE		msfile ;
typedef MSFILE_CUR	msfile_cur ;
typedef MSFILE_ENT	msfile_ent ;

EXTERNC_begin

extern int msfile_open(msfile *,cchar *,int,mode_t) noex ;
extern int msfile_curbegin(msfile *,msfile_cur *) noex ;
extern int msfile_curend(msfile *,msfile_cur *) noex ;
extern int msfile_enum(msfile *,msfile_cur *,msfile_ent *) noex ;
extern int msfile_match(msfile *,time_t,cchar *,int,msfile_ent *) noex ;
extern int msfile_write(msfile *,time_t,cchar *,int,msfile_ent *) noex ;
extern int msfile_update(msfile *,time_t,msfile_ent *) noex ;
extern int msfile_check(msfile *,time_t) noex ;
extern int msfile_count(msfile *) noex ;
extern int msfile_close(msfile *) noex ;

EXTERNC_end


#endif /* MSFILE_INCLUDE */


