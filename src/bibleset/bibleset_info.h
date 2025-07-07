/* bibleset_info HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* clean up the KJV bible as shipped from xxxx */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */


#ifndef	BIBLESETINFO_INCLUDE
#define	BIBLESETINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>

#include	"linecenter.h"
#include	"biblebook.h"
#include	"biblemeta.h"


#ifndef	nelem
#ifdef	nelements
#define	nelem		nelements
#else
#define	nelem(n)	(sizeof(n) / sizeof((n)[0]))
#endif
#endif

#ifndef	FD_STDIN
#define	FD_STDIN	0
#define	FD_STDOUT	1
#define	FD_STDERR	2
#endif

#ifndef	MAXPATHLEN
#define	MAXPATHLEN	2048
#endif

#ifndef	MAXNAMELEN
#define	MAXNAMELEN	256
#endif

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	2048
#endif

/* service name */
#ifndef	SVCNAMELEN
#define	SVCNAMELEN	32
#endif

#ifndef	PASSWDLEN
#define	PASSWDLEN	32
#endif

#ifndef	USERNAMELEN
#ifndef	LOGNAME_MAX
#define	USERNAMELEN	LOGNAME_MAX
#else
#define	USERNAMELEN	32
#endif
#endif

#ifndef	GROUPNAMELEN
#ifndef	LOGNAME_MAX
#define	GROUPNAMELEN	LOGNAME_MAX
#else
#define	GROUPNAMELEN	32
#endif
#endif

#ifndef	LOGNAMELEN
#ifndef	LOGNAME_MAX
#define	LOGNAMELEN	LOGNAME_MAX
#else
#define	LOGNAMELEN	32
#endif
#endif

#ifndef	PROJNAMELEN
#ifndef	LOGNAME_MAX
#define	PROJNAMELEN	LOGNAME_MAX
#else
#define	PROJNAMELEN	32
#endif
#endif

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

/* timezone (zoneinfo) name */
#ifndef	TZLEN
#define	TZLEN		60
#endif

#ifndef	LOGIDLEN
#define	LOGIDLEN	15
#endif

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
#endif

#ifndef	NYEARS_CENTURY
#define	NYEARS_CENTURY	100
#endif

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

#ifndef	DEBUGLEVEL
#define	DEBUGLEVEL(n)	(pip->debuglevel >= (n))
#endif

#ifndef	STDINFNAME
#define	STDINFNAME	"*STDIN*"
#endif

#ifndef	STDOUTFNAME
#define	STDOUTFNAME	"*STDOUT*"
#endif

#ifndef	STDERRFNAME
#define	STDERRFNAME	"*STDERR*"
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(2 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif

#define	BUFLEN		MAX((MAXPATHLEN + MAXNAMELEN),MAXHOSTNAMELEN)

#undef	HDRBUFLEN
#define	HDRBUFLEN	200

#define	PROGINFO	struct proginfo
#define	PROGINFO_FL	struct proginfo_flags
#define	PROGINFO_POUT	struct proginfo_pout
#define	PROGINFO_POFF	struct proginfo_poff
#define	PROGINFO_COUNTS	struct proginfo_counts

#define	PIVARS		struct pivars

#define	ARGINFO		struct arginfo

#define	TROFFSTRS	struct troffstrs 


struct troffstrs {
	cchar	*slash3 ;	/* three slashes */
	cchar	*int2 ;		/* interpolate two-char string-name */
	cchar	*linecomment ;	/* introduce a line comment */
	cchar	*infont_r ;	/* in-line font switching */
	cchar	*infont_i ;
	cchar	*infont_b ;
	cchar	*infont_x ;
	cchar	*infont_cw ;
	cchar	*infont_p ;
} ;

enum words {
	word_chapter,
	word_psalm,
	word_bookindex,
	word_page,
	word_booktitle,
	word_thebookof,
	word_book,
	word_overlast
} ;

struct proginfo_flags {
	uint		progdash:1 ;
	uint		akopts:1 ;
	uint		aparams:1 ;
	uint		quiet:1 ;
	uint		outfile:1 ;
	uint		errfile:1 ;
	uint		biblebook:1 ;
	uint		biblemeta:1 ;
	uint		cover:1 ;	/* cover EPS file */
	uint		frontmatter:1 ;
	uint		backmatter:1 ;
	uint		pagenums:1 ;	/* provide page numbering */
	uint		ff:1 ;		/* font-family */
	uint		ibz:1 ;		/* ignore-book-zero */
	uint		tc:1 ;		/* table-of-contents */
	uint		pagetitle:1 ;
	uint		hyphenate:1 ;
	uint		ha:1 ;
	uint		ps:1 ;		/* point-size */
	uint		vs:1 ;		/* vertical-size (leading) */
	uint		vzlw:1 ;	/* verse-zero line-width */
	uint		vzlb:1 ;	/* verse-zero line-blocking */
	uint		inbook:1 ;	/* saw a book */
	uint		inchapter:1 ;
	uint		inverse:1 ;
	uint		inversezero:1 ;
	uint		inkeep:1 ;
	uint		setchapter:1 ; /* set the chapter TROFF-string */
	uint		setverse:1 ;	/* set the verse TROFF-string */
	uint		chapterzero:1 ; /* saw a zero-chapter */
	uint		versezerohalf:1 ;
	uint		chapterbegin:1 ; /* at beginning of chapter */
	uint		octetbegin:1 ;	/* psalm-octet begin */
	uint		reduced:1 ;	/* font-size reduced */
	uint		quoteblock:1 ;	/* quoted-block */
	uint		tmpshortcol:1 ;	/* temporary short output */
	uint		preverseone:1 ;	/* previous to verse=1 */
	uint		maintextheader:1 ;	/* main-text header */
	uint		maintextfooter:1 ;	/* main-text footer */
} ;

struct proginfo_pout {
	char		pageinfo[TIMEBUFLEN + 1] ;
	char		hf_pageinfo[HDRBUFLEN + 1] ;
	char		hf_pagetitle[HDRBUFLEN + 1] ;
	char		hf_pagelocation[HDRBUFLEN + 1] ;
	char		hf_pagenum[HDRBUFLEN + 1] ;
} ;

struct proginfo_poff {
	cchar	**chartrans ;
} ;

struct proginfo_counts {
	int		book ;
	int		chapter ;
	int		verse ;
	int		vmissing ;	/* "missing" words in a verse */
} ;

struct proginfo {
	vecstr		stores ;
	cchar	**envv ;
	cchar	*pwd ;
	cchar	*progename ;	/* execution filename */
	cchar	*progdname ;	/* dirname of arg[0] */
	cchar	*progname ;	/* basename of arg[0] */
	cchar	*pr ;		/* program root */
	cchar	*searchname ;
	cchar	*rootname ;	/* distribution name */
	cchar	*version ;
	cchar	*banner ;
	cchar	*nodename ;
	cchar	*domainname ;
	cchar	*username ;
	cchar	*groupname ;
	cchar	*tmpdname ;
	cchar	*helpfname ;
	cchar	*bibledb ;
	cchar	*pagetitle ;
	cchar	*pageinfo ;
	cchar	*frontfname ;
	cchar	*coverfname ;	/* cover EPS filename */
	cchar	*ff ;		/* font-family */
	char		*word[word_overlast + 1] ;
	void		*efp ;
	PROGINFO_FL	have, f, changed, final ;
	PROGINFO_FL	open ;
	PROGINFO_COUNTS	c ;
	PROGINFO_POUT	pout ;
	PROGINFO_POFF	poff ;
	TROFFSTRS	troff ;
	linecenter	cv ;		/* center-verse */
	BIBLEBOOK	bb ;
	BIBLEMETA	bm ;
	time_t		daytime ;
	pid_t		pid ;
	uint		columns ;	/* code columns */
	uint		linewidth ;	/* page linewidth */
	uint		vzlw ;		/* verse-zero line-width */
	uint		vzlb ;		/* verse-zero line-break */
	uint		ps ;		/* point-size */
	uint		vs ;		/* vertical-spacing */
	int		pwdlen ;
	int		debuglevel ;
	int		verboselevel ;
	int		ffi ;		/* font-family index */
	int		cckeeps ;	/* count chapter keeps */
} ;

struct pivars {
	cchar	*vpr1 ;
	cchar	*vpr2 ;
	cchar	*vpr3 ;
	cchar	*pr ;
	cchar	*vprname ;
} ;

struct arginfo {
	cchar		**argv ;
	int		argc ;
	int		ai, ai_max, ai_pos ;
	int		ai_continue ;
} ;


EXTERNC_begin

extern int proginfo_start(struct proginfo *,cchar **,cchar *,cchar *) ;
extern int proginfo_setentry(struct proginfo *,cchar **,cchar *,int) ;
extern int proginfo_setversion(struct proginfo *,cchar *) ;
extern int proginfo_setbanner(struct proginfo *,cchar *) ;
extern int proginfo_setsearchname(struct proginfo *,cchar *,cchar *) ;
extern int proginfo_setprogname(struct proginfo *,cchar *) ;
extern int proginfo_setexecname(struct proginfo *,cchar *) ;
extern int proginfo_setprogroot(struct proginfo *,cchar *,int) ;
extern int proginfo_pwd(struct proginfo *) ;
extern int proginfo_rootname(struct proginfo *) ;
extern int proginfo_progdname(struct proginfo *) ;
extern int proginfo_progename(struct proginfo *) ;
extern int proginfo_nodename(struct proginfo *) ;
extern int proginfo_getpwd(struct proginfo *,char *,int) ;
extern int proginfo_getename(struct proginfo *,char *,int) ;
extern int proginfo_getenv(struct proginfo *,cchar *,int,cchar **) ;
extern int proginfo_finish(struct proginfo *) ;

EXTERNC_end


#endif /* BIBLESETINFO_INCLUDE */


