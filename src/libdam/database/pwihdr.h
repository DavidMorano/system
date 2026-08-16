/* pwihdr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Password-Index Header (PWI) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PWIHDR_INCLUDE
#define	PWIHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	PWIHDR			struct pwihdr_head
#define	PWIHDR_MAGICSZ		16
#define	PWIHDR_MAGICSTR		"IPASSWD"
#define	PWIHDR_VERSION		0
#define	PWIHDR_TYPE		0


enum pwihdrs {
	pwihdr_fsz,		/* source DB file size */
	pwihdr_wrtime,
	pwihdr_wrcount,
	pwihdr_rectab,
	pwihdr_reclen,
	pwihdr_recsz,
	pwihdr_strtab,
	pwihdr_lenstr,
	pwihdr_strsz,
	pwihdr_idxlen,
	pwihdr_idxsz,
	pwihdr_idxl1,
	pwihdr_idxl3,
	pwihdr_idxf,
	pwihdr_idxfl3,
	pwihdr_idxun,
	pwihdr_overlast
} ; /* end enum */

struct pwihdr_head {
	uint		fsz ;
	uint		wrtime ;
	uint		wrcount ;
	uint		rectab ;
	uint		recsz ;
	uint		reclen ;
	uint		strtab ;
	uint		lenstr ;
	uint		strsz ;
	uint		idxlen ;
	uint		idxsz ;
	uint		idxl1 ;
	uint		idxl3 ;
	uint		idxf ;
	uint		idxfl3 ;
	uint		idxun ;
	uchar		vetu[4] ;
} ; /* end struct */

#ifdef	__cplusplus
struct pwihdr : pwihdr_head {
    	int rd		(char *,int) noex ;
    	int wr		(cchar *,int) noex ;
} ; /* end struct (bvshdr) */
#else /* __cplusplus */
typedef PWIHDR		pwihdr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	pwihdr_rd(pwihdr *,char *,int) noex ;
extern int	pwihdr_wr(pwihdr *,cchar *,int) noex ;

EXTERNC_end


#endif /* PWIHDR_INCLUDE */


