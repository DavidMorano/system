/* votdchdr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VOTDCHDR_INCLUDE
#define	VOTDCHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#define	VOTDCHDR		struct votdchdr_head
#define	VOTDCHDR_MAGICSIZE	16
#define	VOTDCHDR_MAGICSTR	"VOTDC"
#define	VOTDCHDR_VERSION	0
#define	VOTDCHDR_IDLEN		20	/* front matter */


enum votdchdrhs {
	votdchdrh_shmsz,
	votdchdrh_wtime,
	votdchdrh_atime,
	votdchdrh_wcount,		/* write count */
	votdchdrh_acount,		/* access count */
	votdchdrh_muoff,		/* MUTEX offset */
	votdchdrh_musz,
	votdchdrh_bookoff,		/* book table */
	votdchdrh_booklen,		
	votdchdrh_recoff,		/* verse (record) table */
	votdchdrh_reclen,		
	votdchdrh_balloff,		/* SHM allocator (for books) */
	votdchdrh_ballsz,
	votdchdrh_valloff,		/* SHM allocator (for verses) */
	votdchdrh_vallsz,
	votdchdrh_bstroff,		/* book-name string table */
	votdchdrh_blenstr,
	votdchdrh_vstroff,		/* verse string table */
	votdchdrh_vlenstr,
	votdchdrh_overlast
} ; /* end enum */

struct votdchdr_head {
	uint		shmsz ;
	uint		wtime ;
	uint		atime ;
	uint		wcount ;	/* write count */
	uint		acount ;	/* access count */
	uint		muoff ;		/* MUTEX offset */
	uint		musz ;
	uint		bookoff ;	/* book table */
	uint		booklen ;
	uint		recoff ;	/* record (verse) table */
	uint		reclen ;
	uint		balloff ;	/* book SHM allocator object */
	uint		ballsz ;
	uint		valloff ;	/* verse SHM allocator object */
	uint		vallsz ;
	uint		bstroff ;	/* book-name string table (aligned) */
	uint		blenstr ;
	uint		vstroff ;	/* verse string table (aligned) */
	uint		vlenstr ;
	uchar		vetu[4] ;
} ; /* end struct */

typedef	VOTDCHDR	votdchdr ;

EXTERNC_begin

extern int votdchdr_rd(votdchdr *,char *,int) noex ;
extern int votdchdr_wr(votdchdr *,cchar *,int) noex ;

extern int votdchdr_xx(votdchdr *,int,char *,int) noex ;

EXTERNC_end


#endif /* VOTDCHDR_INCLUDE */


