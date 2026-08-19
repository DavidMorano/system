/* cmi HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* ComMand Index object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMI_INCLUDE
#define	CMI_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<cmihdr.h>		/* LIBDAM hash-file-header */


#define	CMI		struct cmi_head
#define	CMI_OBJ		struct cmi_object
#define	CMI_CUR		struct cmi_cursor
#define	CMI_ENT		struct cmi_entry
#define	CMI_LINE	struct cmi_liner
#define	CMI_INFO	struct cmi_information
#define	CMI_FMI		struct cmi_fmidx
#define	CMI_MAGIC	0x88773427
#define	CMI_SUF		"cmi"		/* variable-index */


struct cmi_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct cmi_information {
	uint		idxctime ;	/* IDX creation-time */
	uint		idxmtime ;	/* IDX modification-time */
	uint		dbtime ;	/* DB-file time */
	uint		dbsz ;		/* DB-file size */
	uint		idxsz ;		/* IDX-file size */
	uint		nents ;
	uint		maxent ;
} ; /* end struct */

struct cmi_liner {
	uint		loff ;
	uint		llen ;
} ; /* end struct */

struct cmi_entry {
	CMI_LINE	*lines ;
	uint		eoff ;
	uint		elen ;
	ushort		nlines ;
	ushort		cn ;
} ; /* end struct */

struct cmi_cursor {
	int		i ;
} ; /* end struct */

struct cmi_fmidx {
	charp		mapdata ;	/* file map */
	uint		(*vt)[4] ;	/* mapped verses table */
	uint		(*lt)[2] ;	/* mapped lines table */
	time_t		ti_mod ;	/* time file modication */
	time_t		ti_map ;	/* time file map */
	size_t		mapsize ;
} ; /* end struct */

struct cmi_head {
	cchar 		*dbname ;
	cchar		*fname ;
	CMI_FMI		*fmp ;		/* pointer file-map */
	cmihdr		*fhp ;		/* pointer file-header */
	time_t		ti_lastcheck ;	/* time last check of file */
	uint		magval ;
	int		ncursors ;
} ; /* end struct */

typedef	CMI		cmi ;
typedef	CMI_OBJ		cmi_obj ;
typedef	CMI_CUR		cmi_cur ;
typedef	CMI_ENT		cmi_ent ;
typedef	CMI_LINE	cmi_line ;
typedef	CMI_INFO	cmi_info ;
typedef	CMI_FMI		cmi_fmi ;

EXTERNC_begin

extern int	cmi_open	(cmi *,cchar *) noex ;
extern int	cmi_count	(cmi *) noex ;
extern int	cmi_getinfo	(cmi *,cmi_info *) noex ;
extern int	cmi_read	(cmi *,cmi_ent *,char *,int,uint) noex ;
extern int	cmi_curbegin	(cmi *,cmi_cur *) noex ;
extern int	cmi_curenum	(cmi *,cmi_cur *,cmi_ent *,char *,int) noex ;
extern int	cmi_curend	(cmi *,cmi_cur *) noex ;
extern int	cmi_audit	(cmi *) noex ;
extern int	cmi_close	(cmi *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const cmi_obj		cmi_modinfo ;
#endif /* __cplusplus */


#endif /* CMI_INCLUDE */


