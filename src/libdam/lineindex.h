/* lineindex HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* line indexing object */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-06-11, David A­D­ Morano
	I snarfed this object (flavor) from the SS-Hammock processing
	and lookup (access management) code.  We will see how it
	works out!

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	LINEINDEX_INCLUDE
#define	LINEINDEX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |caddr_t| */
#include	<sys/stat.h>		/* |USTAT| */
#include	<time.h>		/* |time_t| */
#include	<fcntl.h>		/* |mode_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	LINEINDEX		struct lineindex_head
#define	LINEINDEX_FL		struct lineindex_flags
#define	LINEINDEX_CUR		struct lineindex_cursor
#define	LINEINDEX_INFO		struct lineindex_information

#define	LINEINDEX_MAGIC		0x23456787
#define	LINEINDEX_FILEMAGIC	"LINEINDEX"
#define	LINEINDEX_FILEMAGICSIZE	16
#define	LINEINDEX_FILEMAGICLEN	sizeof(LINEINDEX_FILEMAGIC)
#define	LINEINDEX_FILEVERSION	1
#define	LINEINDEX_FILETYPE	0


struct lineindex_cursor {
	int		i ;
} ;

struct lineindex_information {
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
	cchar		*ifn ;		/* index-file-name */
	cchar		*tfn ;		/* text-file-name */
	uint		*rectab ;
	USTAT		*sbp ;
	caddr_t		mapdata ;
	time_t		tiwrite ;
	time_t		tifmod ;
	time_t		tiopen ;
	time_t		timap ;
	time_t		ticheck ;
	time_t		tiaccess ;
	size_t		mapsize ;
	LINEINDEX_FL	fl ;
	uint		magic ;
	int		pagesize ;
	int		filesize ;
	int		lines ;
	int		cursors ;
	int		fd ;
	int		of ;
	int		ropts ;
	mode_t		om ;
} ; /* end struct (lineindex_head) */

typedef	LINEINDEX		lineindex ;
typedef	LINEINDEX_FL		lineindex_fl ;
typedef	LINEINDEX_CUR		lineindex_cur ;
typedef	LINEINDEX_INFO		lineindex_info ;

EXTERNC_begin

extern int lineindex_open(lineindex *,cchar *,int,mode_t,cchar *) noex ;
extern int lineindex_lookup(lineindex *,int,off_t *) noex ;
extern int lineindex_curbegin(lineindex *,lineindex_cur *) noex ;
extern int lineindex_curend(lineindex *,lineindex_cur *) noex ;
extern int lineindex_curenum(lineindex *,lineindex_cur *,off_t *) noex ;
extern int lineindex_check(lineindex *,time_t) noex ;
extern int lineindex_count(lineindex *) noex ;
extern int lineindex_close(lineindex *) noex ;

EXTERNC_end


#endif /* LINEINDEX_INCLUDE */


