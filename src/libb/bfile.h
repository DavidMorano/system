/* bfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* header file for the Basic I-O library package */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This file was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BFILE_INCLUDE
#define	BFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* |USTAT| */
#include	<unistd.h>
#include	<stdarg.h>		/* |va_list(3c)| */
#include	<usystem.h>
#include	<stdfnames.h>


/* objects */
#define	BFILE		struct bfile_head
#define	BFILE_BD	struct bfile_bufdesc
#define	BFILE_BDFL	struct bfile_bdflags
#define	BFILE_MAP	struct bfile_mapper
#define	BFILE_FL	struct bfile_flags
#define	BFILE_MAPFL	struct bfile_mapflags
/* values */
#define	BFILE_MAGIC	0x20052615
#define	BFILE_MINFD	10
#define	BFILE_BUFPAGES	16
#define	BFILE_FDCH	'*'
#define	BFILE_FDNAMELEN	22
#define	BFILE_MAXNEOF	3		/* maximum EOFs on networks */
#define	BFILE_NMAPS	32		/* number of pages mapped at a time */
/* files */
#define	BFILE_STDIN	STDFNIN		/* from STDFNAMES */
#define	BFILE_STDOUT	STDFNOUT	/* from STDFNAMES */
#define	BFILE_STDERR	STDFNERR	/* from STDFNAMES */
#define	BFILE_STDLOG	STDFNLOG	/* from STDFNAMES */
#define	BFILE_STDNULL	STDFNNULL	/* from STDFNAMES */
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
#define	BFILE_FBUFWHOLE		(1<<2)	/* aka "atomic" */
#define	BFILE_FBUFLINE		(1<<3)
#define	BFILE_FBUFNONE		(1<<4)
#define	BFILE_FBUFDEF		(1<<5)
#define	BFILE_FBUFATOMIC	BFILE_FBUFWHOLE


/* buffering modes */
enum bfilebms {
	bfilebm_reg,			/* buffering reg (default) */
	bfilebm_atomic,			/* buffering atomic */
	bfilebm_line,			/* buffering line */
	bfilebm_none,			/* buffering none */
	bfilebm_overlast
} ;

struct bfile_mapflags {
	uint		valid:1 ;
	uint		dirty:1 ;
} ;

struct bfile_mapper {
	char		*bdata ;	/* buffer data */
	size_t		bsize ;		/* buffer size */
	size_t		offset ;	/* file offset for page */
	BFILE_MAPFL	fl ;
} ;

struct bfile_bdflags {
	uint		dirty:1 ;	/* needs to be written back */
} ;

struct bfile_bufdesc {
	char		*bdata ;	/* base of buffer */
	size_t		boff ;		/* base of buffer */
	BFILE_BDFL	fl ;
	int		bsize ;		/* size of buffer */
	int		blen ;		/* length of data (buffer index) */
} ;

struct bfile_flags {
	uint		created:1 ;
	uint		writing:1 ;	/* 0=reading, 1=writing */
	uint		rd:1 ;		/* reading allowed */
	uint		wr:1 ;		/* writing allowed */
	uint		append:1 ;	/* append-mode */
	uint		filedesc:1 ;	/* file-descriptor type of filename */
	uint		notseek:1 ;
	uint		terminal:1 ;
	uint		network:1 ;
	uint		inpartline:1 ;	/* weirdo mode (should not be used?) */
	uint		cancelled:1 ;
	uint		mappable:1 ;
	uint		mapinit:1 ;
	uint		mapped:1 ;
	uint		nullfile:1 ;
	uint		program:1 ;
} ;

struct bfile_head {
	BFILE_MAP	*maps ;		/* array of map pages */
	BFILE_BD	*bds ;		/* buffer descriptors */
	char		*bdata ;	/* allocated buffer space */
	char		*bbp ;		/* base buffer pointer */
	char		*bp ;		/* current pointer into buffer */
	void		*escp ;		/* escape-pointer */
	ino_t		ino ;
	size_t		offset ; 	/* user view */
	size_t		fsize ;		/* current? file size */
	dev_t		dev ;
	BFILE_FL	fl ;
	uint		magic ;
	int		fd ;
	int		pagesize ;	/* system page size */
	int		bsize ;		/* allocated buffer size */
	int		of ;		/* open flags */
	int		len ;		/* data remaining(r) or filled(w) */
	int		bm ;		/* buffer mode */
	mode_t		fm ;		/* file mode */
	mode_t		om ;		/* open-mode (permissions) */
} ;

typedef BFILE_FL	bfile_fl ;
typedef BFILE_MAP	bfile_map ;
typedef	BFILE_BD	bfile_bd ;
typedef	BFILE_BDFL	bfile_bdfl ;
typedef	BFILE_MAPFL	bfile_mapfl ;

#ifdef	__cplusplus
enum bfilemems {
	bfilemem_size,
	bfilemem_getc,
	bfilemem_isterm,
	bfilemem_flush,
	bfilemem_rewind,
	bfilemem_close,
	bfilemem_overlast
} ;
struct bfile ;
struct bfile_pr {
	bfile		*op = nullptr ;
	int		w = -1 ;
	void operator () (bfile *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar * = nullptr,int = -1) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (bfile_pr) */
struct bfile_co {
	bfile		*op = nullptr ;
	int		w = -1 ;
	void operator () (bfile *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (bfile_co) */
struct bfile : bfile_head {
    	typedef nullptr_t	null ;
	bfile_pr	println ;
	bfile_pr	print ;
	bfile_co	size ;
	bfile_co	getc ;
	bfile_co	isterm ;
	bfile_co	flush ;
	bfile_co	rewind ;
	bfile_co	close ;
	bfile() noex {
	    println(this,0) ;
	    print(this,0) ;
	    size(this,bfilemem_size) ;
	    getc(this,bfilemem_getc) ;
	    isterm(this,bfilemem_isterm) ;
	    flush(this,bfilemem_flush) ;
	    rewind(this,bfilemem_rewind) ;
	    close(this,bfilemem_close) ;
	} ;
	bfile(const bfile &) = delete ;
	bfile &operator = (const bfile &) = delete ;
	int open(cc *,cc * = nullptr,mode_t = 0) noex ;
	int opene(cc *,cc * = nullptr,mode_t = 0,int = -1) noex ;
	int openmod(cc *,cc * = nullptr,mode_t = 0) noex ;
	int wasteln(char *,int) noex ;
	int reade(void *,int,int,int) noex ;
	int read(void *,int) noex ;
	int readlnto(char *,int,int = -1) noex ;
	int readln(char *lp,int ll) noex {
	    return readlnto(lp,ll,-1) ;
	} ;
	int readlns(char *,int,int,int *) noex ;
	int seek(off_t,int) noex ;
	int tell(off_t *) noex ;
	int write(cvoid *,int) noex ;
	int writeblock(bfile *,int) noex ;
	int writefile(cchar *) noex ;
	int writeblanks(int) noex ;
	int writechrs(int,int) noex ;
	int putc(int) noex ;
	int printf(cchar *,...) noex ;
	int vprintf(cchar *,va_list) noex ;
	int printlns(int,cchar *,int) noex ;
	int printcleanln(cchar *,int) noex ;
	int printcleanlns(int,cchar *,int) noex ;
	int copyblock(bfile *,int) noex ;
	int copyfile(bfile *,char *,int) noex ;
	int truncate(off_t) noex ;
	int reserve(int) noex ;
	int minmod(mode_t) noex ;
	int flushn(int) noex ;
	int control(int,...) noex ;
	int controlv(int,va_list) noex ;
	int stat(USTAT *) noex ;
	int dup(bfile *) noex ;
	void dtor() noex ;
	~bfile() {
	    dtor() ;
	} ;
} ; /* end struct (bfile) */
#else	/* __cplusplus */
typedef BFILE		bfile ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	bopen(bfile *,cchar *,cchar *,mode_t) noex ;
extern int	bopene(bfile *,cchar *,cchar *,mode_t,int) noex ;
extern int	bopenprog(bfile *,cchar *,cchar *,mainv,mainv) noex ;
extern int	bopentmp(bfile *,cchar *,cchar *,mode_t) noex ;
extern int	bopenmod(bfile *,cchar *,cchar *,mode_t) noex ;
extern int	bopenlock(bfile *,cchar *,int,int) noex ;
extern int	bcontrol(bfile *,int,...) noex ;
extern int	bcontrolv(bfile *,int,va_list) noex ;
extern int	bstat(bfile *,USTAT *) noex ;
extern int	bsize(bfile *) noex ;
extern int	bseek(bfile *,off_t,int) noex ;
extern int	btell(bfile *,off_t *) noex ;
extern int	brewind(bfile *) noex ;
extern int	bwasteln(bfile *,char *,int) noex ;
extern int	breade(bfile *,void *,int,int,int) noex ;
extern int	bread(bfile *,void *,int) noex ;
extern int	breadlnto(bfile *,char *,int,int) noex ;
extern int	breadlns(bfile *,char *,int,int,int *) noex ;
extern int	bgetc(bfile *) noex ;
extern int	bwrite(bfile *,cvoid *,int) noex ;
extern int	bwriteblock(bfile *,bfile *,int) noex ;
extern int	bwritefile(bfile *,cchar *) noex ;
extern int	bwriteblanks(bfile *,int) noex ;
extern int	bwritechrs(bfile *,int,int) noex ;
extern int	bputc(bfile *,int) noex ;
extern int	bprintf(bfile *,cchar *,...) noex ;
extern int	bvprintf(bfile *,cchar *,va_list) noex ;
extern int	bprintln(bfile *,cchar *,int) noex ;
extern int	bprintlns(bfile *,int,cchar *,int) noex ;
extern int	bprintcleanln(bfile *,cchar *,int) noex ;
extern int	bprintcleanlns(bfile *,int,cchar *,int) noex ;
extern int	bcopyblock(bfile *,bfile *,int) noex ;
extern int	bcopyfile(bfile *,bfile *,char *,int) noex ;
extern int	btruncate(bfile *,off_t) noex ;
extern int	breserve(bfile *,int) noex ;
extern int	bisterm(bfile *) noex ;
extern int	bminmod(bfile *,mode_t) noex ;
extern int	bflush(bfile *) noex ;
extern int	bflushn(bfile *,int) noex ;
extern int	bdup(bfile *,bfile *) noex ;
extern int	bclose(bfile *) noex ;

extern int	bfile_bufreset(bfile *) noex ;
extern int	bfile_acc(bfile *,bool) noex ;
extern int	bfile_ckrd(bfile *) noex ;
extern int	bfile_ckwr(bfile *) noex ;
extern int	bfile_flush(bfile *) noex ;
extern int	bfile_flushn(bfile *,int) noex ;
extern int	bfile_pagein(bfile *,off_t,int) noex ;

static inline int breadln(bfile *fp,char *ubuf,int ulen) noex {
	return breadlnto(fp,ubuf,ulen,-1) ;
}
static inline int bprint(bfile *fp,cchar *lbuf,int llen) noex {
	return bprintln(fp,lbuf,llen) ;
}
static inline int bprintline(bfile *fp,cchar *lbuf,int llen) noex {
	return bprintln(fp,lbuf,llen) ;
}
static inline int bprintlines(bfile *fp,int flen,cchar *lbuf,int llen) noex {
	return bprintlns(fp,flen,lbuf,llen) ;
}

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
static inline int bfile_magic(bfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == BFILE_MAGIC) {
		rs = (op->fl.nullfile) ? 0 : 1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bfile_magic) */

#endif /* __cplusplus */


#endif /* BFILE_INCLUDE */


