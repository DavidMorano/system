/* ffile HEADER */
/* lang=C++20 */


/* Copyright © 1986 David A­D­ Morano.  All rights reserved. */


#ifndef	FFILE_INCLUDE
#define	FFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdio.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	FFILE	struct ffile_head


struct ffile_head {
	FILE		*sfp ;
} ;

typedef	FFILE		ffile ;

EXTERNC_begin

extern int	ffopen(ffile *,cchar *,cchar *) noex ;
extern int	ffread(ffile *,void *,int) noex ;
extern int	ffgetc(ffile *) noex ;
extern int	ffwrite(ffile *,cvoid *,int) noex ;
extern int	ffprintf(ffile *,cchar *,...) noex ;
extern int	ffputc(ffile *,int) noex ;
extern int	ffseek(ffile *,off_t,int) noex ;
extern int	fftell(ffile *,off_t *) noex ;
extern int	ffclose(ffile *) noex ;

EXTERNC_end


#endif /* FFILE_INCLUDE */



