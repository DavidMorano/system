/* txtindexmks HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a TXTINDEX database */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	TXTINDEXMKS_INCLUDE
#define	TXTINDEXMKS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<strtab.h>
#include	<bfile.h>

#include	<txtindexhdr.h>		/* this is the hash-file-header */


#define	TXTINDEXMKS_MAGIC	0x88773422
#define	TXTINDEXMKS		struct txtindexmks_head
#define	TXTINDEXMKS_FL		struct txtindexmks_flags
#define	TXTINDEXMKS_OBJ		struct txtindexmks_object
#define	TXTINDEXMKS_PA		struct txtindexmks_patient
#define	TXTINDEXMKS_TAG		struct txtindexmks_tagitems
#define	TXTINDEXMKS_KEY		struct txtindexmks_keyer
#define	TXTINDEXMKS_TI		struct txtindexmks_titan
#define	TXTINDEXMKS_INTOPEN	(10*60)
#define	TXTINDEXMKS_INTSTALE	(5*60)
#define	TXTINDEXMKS_MINWLEN	3
#define	TXTINDEXMKS_MAXWLEN	10


struct txtindexmks_object {
	cchar		*name ;
	uint		objsize ;
	uint		dummy ;
} ;

struct txtindexmks_patient {
	cchar		*sdn ;
	cchar		*sfn ;
	uint		tablen ;	/* hash-table length */
	uint		minwlen ;	/* minimum key-word length */
	uint		maxwlen ;	/* maximum key-word length */
} ;

struct txtindexmks_keyer {
	cchar		*kp ;
	int		kl ;
} ;

struct txtindexmks_tagitems {
	TXTINDEXMKS_KEY	*keys ;
	cchar		*fname ;
	uint		recoff ;
	uint		reclen ;
	uint		nkeys ;
} ;

struct txtindexmks_titan {
	uint		nkeys ;		/* total number of keys */
	uint		ntags ;		/* total number of tags */
	uint		maxtags ;	/* maximum tags per list */
} ;

struct txtindexmks_flags {
	uint		tagopen:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
	uint		abort:1 ;
} ;

struct txtindexmks_head {
	cchar 		*dbname ;
	cchar		*idname ;
	char		*nfname ;
	char		*nidxfname ;	/* hash index */
	char		*ntagfname ;	/* tags */
	void		*lists ;	/* the lists */
	strtab		*eigenp ;
	bfile		*tagfilep ;
	TXTINDEXMKS_PA	pi ;
	TXTINDEXMKS_TI	ti ;
	TXTINDEXMKS_FL	fl ;
	uint		tagoff ;	/* tag-file running offset */
	uint		tagsize ;	/* tag-file size (after completed) */
	uint		magic ;
	int		nfd ;
	int		clists ;
	mode_t		om ;
} ;

typedef	TXTINDEXMKS		txtindexmks ;
typedef	TXTINDEXMKS_FL		txtindexmks_fl ;
typedef	TXTINDEXMKS_OBJ		txtindexmks_obj ;
typedef	TXTINDEXMKS_PA		txtindexmks_pa ;
typedef	TXTINDEXMKS_TAG		txtindexmks_tag ;
typedef	TXTINDEXMKS_KEY		txtindexmks_key ;
typedef	TXTINDEXMKS_TI		txtindexmks_ti ;

EXTERNC_begin

extern int txtindexmks_open(txtindexmks *,txtindexmks_pa *,cchar *,int,
		mode_t) noex ;
extern int txtindexmks_addeigens(txtindexmks *,txtindexmks_key *,int) noex ;
extern int txtindexmks_addtags(txtindexmks *,txtindexmks_tag *,int) noex ;
extern int txtindexmks_noop(txtindexmks *) noex ;
extern int txtindexmks_abort(txtindexmks *) noex ;
extern int txtindexmks_close(txtindexmks *) noex ;

EXTERNC_end


#endif /* TXTINDEXMKS_INCLUDE */


