/* lineindexhdr HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 (confirmance reviewed) */

/* Line-Index-Header */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LINEINDEXHDR_INCLUDE
#define	LINEINDEXHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	LINEINDEXHDR			struct lineindexhdr_head
#define	LINEINDEXHDR_MAGICSIZE		16
#define	LINEINDEXHDR_MAGICSTR		"LINEINDEX"
#define	LINEINDEXHDR_VERSION		0
#define	LINEINDEXHDR_TYPE		0


enum lineindexhdrs {
	lineindexhdr_rectab,		/* source DB file size */
	lineindexhdr_wrtime,
	lineindexhdr_lines,
	lineindexhdr_overlast
} ;

struct lineindexhdr_head {
	uint		rectab ;
	uint		wrtime ;
	uint		lines ;
	uchar		vetu[4] ;
} ;

#ifdef	__cplusplus
struct lineindexhdr : lineindexhdr_head {
	lineindexhdr(const lineindexhdr &) = delete ;
	lineindexhdr &operator = (const lineindexhdr &) = delete ;
	lineindexhdr() = default ;
	int rd(char *,int) noex ;
	int wr(cchar *,int) noex ;
} ; /* end struct (lineindexhdr) */
#else	/* __cplusplus */
typedef LINEINDEXHDR	lineindexhdr ;
#endif	/* __cplusplus */

EXTERNC_begin

extern int	lineindexhdr_rd(lineindexhdr *,char *,int) noex ;
extern int	lineindexhdr_wr(lineindexhdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* LINEINDEXHDR_INCLUDE */


