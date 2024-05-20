/* lineindex HEADER */
/* lang=C20 */

/* line indexing object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LINEINDEX_INCLUDE
#define	LINEINDEX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<time.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#define	LINEINDEX		struct lineindex_head
#define	LINEINDEX_INFO		struct lineindex_i
#define	LINEINDEX_CUR		struct lineindex_c

#define	LINEINDEX_MAGIC		0x23456787
#define	LINEINDEX_FILEMAGIC	"LINEINDEX"
#define	LINEINDEX_FILEMAGICSIZE	16
#define	LINEINDEX_FILEMAGICLEN	sizeof(LINEINDEX_FILEMAGIC)
#define	LINEINDEX_FILEVERSION	1
#define	LINEINDEX_ENDIAN	1	/* always network order */
#define	LINEINDEX_FILETYPE	0


struct lineindex_c {
	int		i ;
} ;

struct lineindex_i {
	time_t		wtime ;		/* time DB written */
	uint		lines ;		/* total number of entries */
	uint		version ;
	uint		encoding ;
	uint		type ;
} ;

struct lineindex_flags {
	uint		remote:1 ;
	uint		fileinit:1 ;
	uint		cursorlockbroken:1 ;
	uint		cursoracc:1 ;
	uint		wantwrite:1 ;
} ;

struct lineindex_head {
	uint		magic ;
	const char	*idxfname ;
	const char	*txtfname ;
	caddr_t		mapbuf ;
	uint		*rectab ;
	struct lineindex_flags	f ;
	time_t		wtime ;
	time_t		mtime ;
	time_t		ti_open ;
	time_t		ti_map ;
	time_t		ti_check ;
	time_t		ti_access ;
	size_t		mapsize ;
	uint		pagesize ;
	uint		filesize ;
	uint		lines ;
	int		cursors ;
	int		fd ;
	int		oflags, operm ;
	int		ropts ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int lineindex_open(LINEINDEX *,const char *,int,mode_t,const char *) ;
extern int lineindex_lookup(LINEINDEX *,uint,off_t *) ;
extern int lineindex_curbegin(LINEINDEX *,LINEINDEX_CUR *) ;
extern int lineindex_curend(LINEINDEX *,LINEINDEX_CUR *) ;
extern int lineindex_enum(LINEINDEX *,LINEINDEX_CUR *,off_t *) ;
extern int lineindex_check(LINEINDEX *,time_t) ;
extern int lineindex_count(LINEINDEX *) ;
extern int lineindex_close(LINEINDEX *) ;

#ifdef	__cplusplus
}
#endif


#endif /* LINEINDEX_INCLUDE */


