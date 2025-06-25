/* votdc HEADER (VOTD Shared-memory management) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* VOTDs system Cache management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VOTDC_INCLUDE
#define	VOTDC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |caddr_t| */
#include	<time.h>		/* |time_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ptm.h>
#include	<shmalloc.h>
#include	<votdchdr.h>


#ifndef	SVCBUFLEN
#define	SVCBUFLEN		32
#endif

#define	VOTDC_MAGIC		0x43628190
#define	VOTDC_DEFLANG		"english"
#define	VOTDC_LANGLEN		SVCBUFLEN
#define	VOTDC_NTITLES		(66+1)
#define	VOTDC_NLANGS		4
#define	VOTDC_NBOOKS		4
#define	VOTDC_NVERSES		20
#define	VOTDC_BSTRSIZE		(VOTDC_NBOOKS*VOTDC_NTITLES*30)
#define	VOTDC_VSTRSIZE		(VOTDC_NVERSES*320)
#define	VOTDC			struct votdc_head
#define	VOTDC_FL		struct votdc_flags
#define	VOTDC_OBJ		struct votdc_object
#define	VOTDC_BOOK		struct votdc_booker
#define	VOTDC_VERSE		struct votdc_verser
#define	VOTDC_INFO		struct votdc_information
#define	VOTDC_CITE		struct votdc_citation
#define	VOTDC_Q			struct votdc_citation
#define	VOTDC_TC		struct votdc_titlecache
#define	VOTDC_VCUR		struct votdc_vcursor


struct votdc_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ;

struct votdc_vcursor {
	int		i ;
} ;

struct votdc_titlecache {
	cchar		**titles ;
	cchar		*a ;
	int		wmark ;
	int		amark ;
	char		lang[VOTDC_LANGLEN+1] ;
} ;

struct votdc_booker {
	time_t		ctime ;
	time_t		atime ;
	int		wmark ;
	int		amark ;
	int		b[VOTDC_NTITLES+1] ;
	char		lang[VOTDC_LANGLEN+1] ;
} ;

struct votdc_verser {
	time_t		ctime ;
	time_t		atime ;
	int		mjd ;
	int		voff ;
	int		vlen ;
	int		wmark ;		/* write mark */
	int		amark ;		/* access mark */
	uchar		book, chapter, verse, lang ;
} ;

struct votdc_citation {
	uchar		b, c, v, l ;
} ;

struct votdc_information {
	time_t		wtime ;
	time_t		atime ;
	int		nbooks ;
	int		nverses ;
} ;

struct votdc_flags {
	uint		shm:1 ;
	uint		txt:1 ;
	uint		sorted:1 ;
} ;

struct votdc_head {
	cchar		*a ;		/* object string allocations */
	cchar		*pr ;
	cchar		*lang ;
	cchar		*shmname ;
	caddr_t		mapdata ;	/* SHM data */
	ptm		*mxp ;		/* pointer to SHM mutex */
	VOTDC_BOOK	*books ;	/* book-records */
	VOTDC_VERSE	*verses ;	/* verse-records */
	shmalloc	*ball ;		/* book-string allocator */
	shmalloc	*vall ;		/* verse-string allocator */
	char		*bstr ;		/* book string-table */
	char		*vstr ;		/* verse string-table */
	votdchdr	hdr ;
	time_t		ti_map ;	/* map-time */
	time_t		ti_lastcheck ;
	size_t		mapsize ;	/* SHM map-size */
	VOTDC_TC	tcs[VOTDC_NBOOKS] ;
	VOTDC_FL	fl ;
	uint		magic ;
	int		pagesize ;
	int		shmsize ;
	int		nents ;	
	int		fd ;
} ;

typedef	VOTDC			votdc ;
typedef	VOTDC_FL		votdc_fl ;
typedef	VOTDC_OBJ		votdc_obj ;
typedef	VOTDC_BOOK		votdc_book ;
typedef	VOTDC_VERSE		votdc_verse ;
typedef	VOTDC_INFO		votdc_info ;
typedef	VOTDC_CITE		votdc_cite ;
typedef	VOTDC_Q			votdc_q ;
typedef	VOTDC_TC		votdc_tc ;
typedef	VOTDC_VCUR		votdc_vcur ;

EXTERNC_begin

extern int votdc_open(votdc *,cchar *,cchar *,int) noex ;
extern int votdc_titlelang(votdc *,cchar *) noex ;
extern int votdc_titleloads(votdc *,cchar *,cchar **) noex ;
extern int votdc_titleget(votdc *,char *,int,int,int) noex ;
extern int votdc_titlefetch(votdc *,char *,int,cchar *,int) noex ;
extern int votdc_titlematch(votdc *,cchar *,cchar *,int) noex ;
extern int votdc_vcurbegin(votdc *,votdc_vcur *) noex ;
extern int votdc_vcurenum(votdc *,votdc_vcur *,votdc_cite *,char *,int) noex ;
extern int votdc_vcurend(votdc *,votdc_vcur *) noex ;
extern int votdc_verseload(votdc *,cchar *,votdc_cite *,int,cchar *,int) noex ;
extern int votdc_versefetch(votdc *,votdc_cite *,char *,int,cchar *,int) noex ;
extern int votdc_getinfo(votdc *,votdc_info *) noex ;
extern int votdc_close(votdc *) noex ;

EXTERNC_end


#endif /* VOTDC_INCLUDE */


