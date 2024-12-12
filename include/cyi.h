/* cyi HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CYI_INCLUDE
#define	CYI_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>

#include	<cyihdr.h>		/* this is the hash-file-header */
#include	<calcite.h>


#define	CYI_MAGIC	0x88773421
#define	CYI		struct cyi_head
#define	CYI_OBJ		struct cyi_object
#define	CYI_Q		CALCITE
#define	CYI_QUERY	CALCITE
#define	CYI_CUR		struct cyi_cursor
#define	CYI_ENT		struct cyi_entry
#define	CYI_LINE	struct cyi_liner
#define	CYI_INFO	struct cyi_information
#define	CYI_FMI		struct cyi_fmier


struct cyi_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct cyi_information {
	time_t		ctime ;		/* index create-time */
	time_t		mtime ;		/* index modification-time */
	uint		count ;
	uint		year ;		/* year index was made for */
} ;

struct cyi_liner {
	uint		loff ;
	uint		llen ;
} ;

struct cyi_entry {
	CYI_LINE	*lines ;
	uint		voff ;
	uint		vlen ;
	uint		hash ;
	ushort		y ;
	uchar		nlines, m, d ;	/* maximum 255 lines per entry */
} ;

struct cyi_cursor {
	uint		magic ;
	uint		citekey ;
	int		i ;
} ;

struct cyi_fmier {
	void		*mapdata ;	/* file map */
	uint		(*vt)[5] ;	/* mapped entries table */
	uint		(*lt)[2] ;	/* mapped lines table */
	time_t		ti_mod ;	/* time file modication */
	time_t		ti_map ;	/* time file map */
	size_t		mapsize ;
} ;

struct cyi_head {
	cchar		*fname ;
	CYI_FMI		fmi ;		/* file-map information */
	CYIHDR		fhi ;		/* file-header information */
	time_t		ti_lastcheck ;	/* time last check of file */
	uint		magic ;
	int		ncursors ;
	int		year ;
} ;

typedef	CYI		cyi ;
typedef	CYI_OBJ		cyi_obj ;
typedef	CYI_Q		cyi_q ;
typedef	CYI_QUERY	cyi_query ;
typedef	CYI_CUR		cyi_cur ;
typedef	CYI_ENT		cyi_ent ;
typedef	CYI_LINE	cyi_line ;
typedef	CYI_INFO	cyi_info ;
typedef	CYI_FMI		cyi_fmi ;

EXTERNC_begin

extern int	cyi_open(cyi *,int,cchar *,cchar *) noex ;
extern int	cyi_count(cyi *) noex ;
extern int	cyi_getinfo(cyi *,cyi_info *) noex ;
extern int	cyi_curbegin(cyi *,cyi_cur *) noex ;
extern int	cyi_curcite(cyi *,cyi_cur *,cyi_q *) noex ;
extern int	cyi_curread(cyi *,cyi_cur *,cyi_ent *,char *,int) noex ;
extern int	cyi_curenum(cyi *,cyi_cur *,cyi_ent *,char *,int) noex ;
extern int	cyi_curend(cyi *,cyi_cur *) noex ;
extern int	cyi_audit(cyi *) noex ;
extern int	cyi_close(cyi *) noex ;

EXTERNC_end


#endif /* CYI_INCLUDE */


