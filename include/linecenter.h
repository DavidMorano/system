/* linecenter HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* text fill (for line-centering) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	LINECENTER_INCLUDE
#define	LINECENTER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<fifostr.h>


#define	LINECENTER_MAGIC	0x88442239
#define	LINECENTER_DEFLINES	7
#define	LINECENTER_FNLEN	3
#define	LINECENTER_DEFPERCENT	0.60
#define	LINECENTER		struct linecenter_head
#define	LINECENTER_CENT		struct linecenter_center


struct linecenter_center {
	uint		linelen ;
	uint		linebrk ;
} ;

struct linecenter_head {
	cchar		**lines ;
	fifostr		*sqp ;		/* ?? */
	uint		magic ;
	int		li ;		/* line-index */
	int		le ;		/* line-extent */
	int		wc ;		/* word-count */
	int		ccnt ;		/* character count (w/ blanks) */
	char		fn[LINECENTER_FNLEN + 1] ;
} ;

typedef	LINECENTER		linecenter ;
typedef	LINECENTER_CENT		linecenter_cent ;

EXTERNC_begin

extern int	linecenter_start(linecenter *,cchar *) noex ;
extern int	linecenter_addword(linecenter *,cchar *,int) noex ;
extern int	linecenter_addline(linecenter *,cchar *,int) noex ;
extern int	linecenter_addlines(linecenter *,cchar *,int) noex ;
extern int	linecenter_mklinefull(linecenter *,char *,int) noex ;
extern int	linecenter_mklinepart(linecenter *,char *,int) noex ;
extern int	linecenter_count(linecenter *) noex ;
extern int	linecenter_mklines(linecenter *,int,int) noex ;
extern int	linecenter_getline(linecenter *,int,cchar **) noex ;
extern int	linecenter_finish(linecenter *) noex ;

EXTERNC_end


#endif /* LINECENTER_INCLUDE */


