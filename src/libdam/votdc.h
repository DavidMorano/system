/* votdc HEADER (VOTD Shared-memory management) */
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
#define	VOTDC_OBJ		struct votdc_obj
#define	VOTDC_BOOK		struct votdc_book
#define	VOTDC_VERSE		struct votdc_verse
#define	VOTDC_INFO		struct votdc_info
#define	VOTDC_CITE		struct votdc_cite
#define	VOTDC_Q			struct votdc_cite
#define	VOTDC_TC		struct votdc_titlecache
#define	VOTDC_VCUR		struct votdc_vcur


struct votdc_obj {
	cchar		*name ;
	uint		objsize ;
} ;

struct votdc_vcur {
	int		i ;
} ;

struct votdc_titlecache {
	cchar		**titles ;
	cchar		*a ;
	int		wmark ;
	int		amark ;
	char		lang[VOTDC_LANGLEN+1] ;
} ;

struct votdc_book {
	time_t		ctime ;
	time_t		atime ;
	int		wmark ;
	int		amark ;
	int		b[VOTDC_NTITLES+1] ;
	char		lang[VOTDC_LANGLEN+1] ;
} ;

struct votdc_verse {
	time_t		ctime ;
	time_t		atime ;
	int		mjd ;
	int		voff ;
	int		vlen ;
	int		wmark ;		/* write mark */
	int		amark ;		/* access mark */
	uchar		book, chapter, verse, lang ;
} ;

struct votdc_cite {
	uchar		b, c, v, l ;
} ;

struct votdc_info {
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
	uint		magic ;
	VOTDC_FL	f ;
	cchar		*a ;		/* object string allocations */
	cchar		*pr ;
	cchar		*lang ;
	cchar		*shmname ;
	caddr_t		mapdata ;	/* SHM data */
	ptm		*mp ;		/* pointer to SHM mutex */
	VOTDC_BOOK	*books ;	/* book-records */
	VOTDC_VERSE	*verses ;	/* verse-records */
	shmalloc	*ball ;		/* book-string allocator */
	shmalloc	*vall ;		/* verse-string allocator */
	char		*bstr ;		/* book string-table */
	char		*vstr ;		/* verse string-table */
	votdchdr	hdr ;
	VOTDC_TC	tcs[VOTDC_NBOOKS] ;
	time_t		ti_map ;	/* map-time */
	time_t		ti_lastcheck ;
	size_t		mapsize ;	/* SHM map-size */
	int		pagesize ;
	int		shmsize ;
	int		nents ;	
	int		fd ;
} ;

typedef	VOTDC			votdc ;
typedef	VOTDC_FL		votdc_fl ;
typedef	VOTDC_OBJ		struct votdc_obj
typedef	VOTDC_BOOK		struct votdc_book
typedef	VOTDC_VERSE		struct votdc_verse
typedef	VOTDC_INFO		struct votdc_info
typedef	VOTDC_CITE		struct votdc_cite
typedef	VOTDC_Q			struct votdc_cite
typedef	VOTDC_TC		struct votdc_titlecache
typedef	VOTDC_VCUR		struct votdc_vcur

EXTERNC_begin
#endif

extern int	votdc_open(VOTDC *,cchar *,cchar *,int) ;
extern int	votdc_titlelang(VOTDC *,cchar *) ;
extern int	votdc_titleloads(VOTDC *,cchar *,cchar **) ;
extern int	votdc_titleget(VOTDC *,char *,int,int,int) ;
extern int	votdc_titlefetch(VOTDC *,char *,int,cchar *,int) ;
extern int	votdc_titlematch(VOTDC *,cchar *,cchar *,int) ;
extern int	votdc_vcurbegin(VOTDC *,VOTDC_VCUR *) ;
extern int	votdc_vcurenum(VOTDC *,VOTDC_VCUR *,VOTDC_CITE *,char *,int) ;
extern int	votdc_vcurend(VOTDC *,VOTDC_VCUR *) ;
extern int	votdc_verseload(VOTDC *,cchar *,VOTDC_CITE *,int,cchar *,int) ;
extern int	votdc_versefetch(VOTDC *,VOTDC_CITE *,char *,int,cchar *,int) ;
extern int	votdc_info(VOTDC *,VOTDC_INFO *) ;
extern int	votdc_close(VOTDC *) ;

EXTERNC_end


#endif /* VOTDC_INCLUDE */


