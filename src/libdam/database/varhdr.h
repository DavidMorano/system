/* varhdr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* text-index header for VAR-INDEX file */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VARHDR_INCLUDE
#define	VARHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	VARHDR			struct varhdr_head
#define	VARHDR_MAGICSZ		16
#define	VARHDR_MAGICSTR		"VARIND"
#define	VARHDR_VERSION		0
#define	VARHDR_FSUF		"vi"


struct varhdr_head {
	uint		fsz ;
	uint		wtime ;
	uint		ksoff ;
	uint		kslen ;
	uint		vsoff ;
	uint		vslen ;
	uint		rtoff ;
	uint		rtlen ;
	uint		itoff ;
	uint		itlen ;
	uint		nvars ;
	uint		nskip ;
	uchar		vetu[4] ;
} ; /* end struct */

#ifdef	__cplusplus
struct varhdr : varhdr_head {
    	int rd		(char *,int) noex ;
    	int wr		(cchar *,int) noex ;
} ; /* end struct (bvshdr) */
#else /* __cplusplus */
typedef	VARHDR		varhdr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	varhdr_rd(varhdr *,char *,int) noex ;
extern int	varhdr_wr(varhdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* VARHDR_INCLUDE */


