/* bfile HEADER */
/* lang=C20 */

/* header file for the Basic I/O library package */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This file was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BFILE_INCLUDE
#define	BFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<stdarg.h>
#include	<usystem.h>
#include	<localmisc.h>


#define	BFILE		struct bfile_head
#define	BFILE_BD	struct bfile_bd
#define	BFILE_BDFLAGS	struct bfile_bdflags
#define	BFILE_MAP	struct bfile_map
#define	BFILE_FLAGS	struct bfile_flags
#define	BFILE_MAPFLAGS	struct bfile_mapflags

#define	BFILE_MAGIC	0x20052615
#define	BFILE_BUFPAGES	16
#define	BFILE_FDCH	'*'
#define	BFILE_FDNAMELEN	22
#define	BFILE_MAXNEOF	3		/* maximum EOFs on networks */
#define	BFILE_NMAPS	32		/* number of pages mapped at a time */

/* user commands to 'bcontrol' */
#define	BC_NOOP		0
#define	BC_TELL		1
#define	BC_BUF		2		/* perform buffering */
#define	BC_FULLBUF	BC_BUF
#define	BC_LINEBUF	3		/* do line buffering on output */
#define	BC_UNBUF	4		/* do no buffering */
#define	BC_NOBUF	BC_UNBUF
#define	BC_FD		5		/* get the file description */
#define	BC_STAT		6
#define	BC_TRUNCATE	7
#define	BC_CHMOD	8
#define	BC_SETLK	9		/* perform custom locking */
#define	BC_SETLKW	10
#define	BC_GETLK	11
#define	BC_UNLOCK	12		/* unlock any locks */
#define	BC_LOCKREAD	13		/* lock whole file for reads */
#define	BC_LOCKWRITE	14		/* lock whole file for writes */
#define	BC_LOCK		BC_LOCKWRITE
#define	BC_INPARTLINE	15		/* allow partial input on lines */
#define	BC_GETFL	16		/* get file flags */
#define	BC_SETFL	17		/* set file flags */
#define	BC_GETFDFL	18		/* get file descriptor flags */
#define	BC_SETFDFL	19		/* set file descriptor flags */
#define	BC_GETFD	BC_GETFDFL
#define	BC_SETFD	BC_SETFDFL
#define	BC_CLOSEONEXEC	20		/* set CLOSE-on-EXEC flag */
#define	BC_BUFSIZE	21		/* set buffer size */
#define	BC_DSYNC	22		/* sync data sync */
#define	BC_SYNC		23		/* sync data and attributes */
#define	BC_ISLINEBUF	24		/* get line-buffer status */
#define	BC_ISTERMINAL	25		/* get terminal status */
#define	BC_MINMOD	26		/* ensure minimum file mode */
#define	BC_SETBUFALL	27		/* output buffering */
#define	BC_SETBUFWHOLE	28		/* output buffering */
#define	BC_SETBUFLINE	29		/* output buffering */
#define	BC_SETBUFNONE	30		/* output buffering */
#define	BC_SETBUFDEF	31		/* output buffering */
#define	BC_GETBUFFLAGS	32		/* output buffering */
#define	BC_SETBUFFLAGS	33		/* output buffering */
#define	BC_NONBLOCK	34

/* flags */
#define	BFILE_FINPARTLINE	(1<<0)
#define	BFILE_FTERMINAL 	(1<<1)
#define	BFILE_FBUFWHOLE		(1<<2)
#define	BFILE_FBUFLINE		(1<<3)
#define	BFILE_FBUFNONE		(1<<4)
#define	BFILE_FBUFDEF		(1<<5)

#define	BFILE_DEBUGFNAME	"bfile.deb"


/* buffering modes */
enum bfile_bms {
	bfile_bmall,
	bfile_bmwhole,
	bfile_bmline,
	bfile_bmnone,
	bfile_bmoverlast
} ;

struct bfile_mapflags {
	uint		valid:1 ;
	uint		dirty:1 ;
} ;

struct bfile_map {
	size_t		offset ;	/* file offset for page */
	char		*buf ;
	BFILE_MAPFLAGS	f ;
} ;

struct bfile_bdflags {
	uint		dirty:1 ;	/* needs to be written back */
} ;

struct bfile_bd {
	size_t		boff ;		/* base of buffer */
	BFILE_BDFLAGS	f ;
	char		*bdata ;	/* base of buffer */
	int		bsize ;		/* size of buffer */
	int		blen ;		/* length of data (buffer index) */
} ;

struct bfile_flags {
	uint		created:1 ;
	uint		write:1 ;
	uint		notseek:1 ;
	uint		terminal:1 ;
	uint		network:1 ;
	uint		inpartline:1 ;	/* weirdo mode (should not be used?) */
	uint		cancelled:1 ;
	uint		mappable:1 ;
	uint		mapinit:1 ;
	uint		mapped:1 ;
	uint		nullfile:1 ;
} ;

struct bfile_head {
	uint		magic ;
	BFILE_FLAGS	f ;
	size_t		offset ; 	/* user view */
	BFILE_MAP	*maps ;		/* array of map pages */
	BFILE_BD	*bds ;		/* buffer descriptors */
	char		*bdata ;	/* allocated buffer space */
	char		*bbp ;		/* base buffer pointer */
	char		*bp ;		/* current pointer into buffer */
	size_t		fsize ;		/* current? file size */
	uino_t		ino ;
	dev_t		dev ;
	mode_t		mode ;
	int		fd ;
	int		pagesize ;	/* system page size */
	int		bsize ;		/* allocated buffer size */
	int		oflags ;	/* open flags */
	int		len ;		/* data remaining(r) or filled(w) */
	int		bm ;		/* buffer mode */
} ;

typedef struct bfile_head	bfile ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	bopen(bfile *,cchar *,cchar *,mode_t) noex ;
extern int	bopene(bfile *,cchar *,cchar *,mode_t,int) noex ;
extern int	bopenprog(bfile *,cchar *,cchar *,cchar **,cchar **) noex ;
extern int	bopentmp(bfile *,cchar *,cchar *,mode_t) noex ;
extern int	bopenmod(bfile *,cchar *,cchar *,mode_t) noex ;
extern int	bopenlock(bfile *,cchar *,int,int) noex ;
extern int	bcontrol(bfile *,int,...) noex ;
extern int	bstat(bfile *,USTAT *) noex ;
extern int	bsize(bfile *) noex ;
extern int	bseek(bfile *,offset_t,int) noex ;
extern int	btell(bfile *,offset_t *) noex ;
extern int	brewind(bfile *) noex ;
extern int	breade(bfile *,void *,int,int,int) noex ;
extern int	bread(bfile *,void *,int) noex ;
extern int	breadlnto(bfile *,char *,int,int) noex ;
extern int	breadlns(bfile *,char *,int,int,int *) noex ;
extern int	bgetc(bfile *) noex ;
extern int	bwrite(bfile *,cvoid *,int) noex ;
extern int	bwriteblock(bfile *,bfile *,int) noex ;
extern int	bwritefile(bfile *,cchar *) noex ;
extern int	bputc(bfile *,int) noex ;
extern int	bprintf(bfile *,cchar *,...) noex ;
extern int	bvprintf(bfile *,cchar *,va_list) noex ;
extern int	bprintln(bfile *,cchar *,int) noex ;
extern int	bprint(bfile *,cchar *,int) noex ;
extern int	bprintline(bfile *,cchar *,int) noex ;
extern int	bcopyblock(bfile *,bfile *,int) noex ;
extern int	bcopyfile(bfile *,bfile *,char *,int) noex ;
extern int	btruncate(bfile *,offset_t) noex ;
extern int	breserve(bfile *,int) noex ;
extern int	bisterm(bfile *) noex ;
extern int	bminmod(bfile *,mode_t) noex ;
extern int	bflush(bfile *) noex ;
extern int	bflushn(bfile *,int) noex ;
extern int	bclose(bfile *) noex ;

extern int	bfile_flush(bfile *) noex ;
extern int	bfile_flushn(bfile *,int) noex ;
extern int	bfile_pagein(bfile *,offset_t,int) noex ;

extern int	bfilestat(cchar *,int,USTAT *) noex ;
extern int	bfilefstat(int,USTAT *) noex ;

static inline int breadln(bfile *fp,char *ubuf,int ulen) noex {
	return breadlnto(fp,ubuf,ulen,-1) ;
}

#ifdef	__cplusplus
}
#endif


#endif /* BFILE_INCLUDE */


