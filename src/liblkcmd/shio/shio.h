/* shio HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* the SHell-IO hack */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	SHIO_INCLUDE
#define	SHIO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>		/* |USTAT| */
#include	<stdarg.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bfile.h>


#define	SHIO_MAGIC	0x43628192
#define	SHIO		struct shio_head
#define	SHIO_FL		struct shio_flags

/* these are control commands */
#define	SHIO_CNOP		0	/* no-operation */
#define	SHIO_CSETBUFWHOLE	1	/* use whole buffering */
#define	SHIO_CSETBUFLINE	2	/* perform line-buffer flushing */
#define	SHIO_CSETBUFNONE	3	/* do not buffer */
#define	SHIO_CSETBUFDEF		4	/* buffer as default */
#define	SHIO_CFD		5	/* get the underlying FD */
#define	SHIO_CSETFLAGS		6	/* set flags */
#define	SHIO_CGETFLAGS		7	/* get flags */
#define	SHIO_CNONBLOCK		8	/* nonblocking mode */
#define	SHIO_CSTAT		9	/* stat */

/* flags */
#define	SHIO_FBUFWHOLE		(1<<0)
#define	SHIO_FBUFLINE		(1<<1)
#define	SHIO_FBUFNONE		(1<<2)
#define	SHIO_FBUFDEF		(1<<3)


enum shiobms {
	shiobm_all,
	shiobm_whole,
	shiobm_line,
	shiobm_none,
	shiobm_overlast
} ;

struct shio_flags {
	uint		sfio:1 ;
	uint		stdfname:1 ;
	uint		bufwhole:1 ;
	uint		bufline:1 ;
	uint		bufnone:1 ;
	uint		bufdef:1 ;
	uint		terminal:1 ;
	uint		nullfile:1 ;
} ;

struct shio_head {
	void		*outstore ;	/* for OUTSORE if needed */
	void		*fp ;
	bfile		*bfp ;		/* memory-allocated */
	cchar		*fname ;	/* memory allocated */
	SHIO_FL		fl ;
	uint		magic ;
} ;

typedef	SHIO		shio ;
typedef	SHIO_FL		shio_fl ;

EXTERNC_begin

extern int	shio_opene(shio *,cchar *,cchar *,mode_t,int) noex ;
extern int	shio_opentmp(shio *,mode_t) noex ;
extern int	shio_open(shio *,cchar *,cchar *,mode_t) noex ;
extern int	shio_reade(shio *,void *,int,int,int) noex ;
extern int	shio_read(shio *,void *,int) noex ;
extern int	shio_readline(shio *,char *,int) noex ;
extern int	shio_readlines(shio *,char *,int,int *) noex ;
extern int	shio_readlinetimed(shio *,char *,int,int) noex ;
extern int	shio_write(shio *,cvoid *,int) noex ;
extern int	shio_println(shio *,cchar *,int) noex ;
extern int	shio_print(shio *,cchar *,int) noex ;
extern int	shio_printline(shio *,cchar *,int) noex ;
extern int	shio_vprintf(shio *,cchar *,va_list) noex ;
extern int	shio_printf(shio *,cchar *,...) noex ;
extern int	shio_putc(shio *,int) noex ;
extern int	shio_seek(shio *,off_t,int) noex ;
extern int	shio_flush(shio *) noex ;
extern int	shio_control(shio *,int,...) noex ;
extern int	shio_getfd(shio *) noex ;
extern int	shio_getlines(shio *) noex ;
extern int	shio_reserve(shio *,int) noex ;
extern int	shio_isterm(shio *) noex ;
extern int	shio_isseekable(shio *) noex ;
extern int	shio_stat(shio *,USTAT *) noex ;
extern int	shio_writefile(shio *,cchar *) noex ;
extern int	shio_close(shio *) noex ;

extern int	shio_readintr(shio *,void *,int,int,int **) noex ;

EXTERNC_end


#endif /* SHIO_INCLUDE */


