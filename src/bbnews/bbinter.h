/* bbinter HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* the user bbinterface (command bbinterpreter) for VMAIL */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This is a complete rewrite of the trash that performed this
	function previously.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	BBINTER_INCLUDE
#define	BBINTER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<signal.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uterm.h>
#include	<mbcache.h>
#include	<mailbox.h>
#include	<mailmsgfile.h>
#include	<mailmsgviewer.h>
#include	<keysymer.h>
#include	<kbdinfo.h>
#include	<localmisc.h>

#include	"cmdmap.h"
#include	"config.h"
#include	"defs.h"
#include	"display.h"


#define	BBINTER		struct bbinter_head
#define	BBINTER_NUMLEN	10


struct bbinter_flags {
	uint		welcome:1 ;		/* welcome message issued */
	uint		cmdprompt:1 ;		/* prompt issued */
	uint		cmdnumeric:1 ;		/* cmd-numeric in progress */
	uint		mailnew:1 ;		/* new mail arrived */
	uint		mailinfo:1 ;		/* displaying mailinfo */
	uint		uterminit:1 ;
	uint		cminit:1 ;		/* CMDMAP */
	uint		ksinit:1 ;		/* KEYSYMER */
	uint		kiinit:1 ;		/* KBDINFO */
	uint		mbopen:1 ;		/* mailbox is open */
	uint		mbreadonly:1 ;		/* mailbox opened read-only */
	uint		mcinit:1 ;
	uint		mfinit:1 ;
	uint		mvinit:1 ;
	uint		setmbname:1 ;
	uint		exit:1 ;		/* exit bbinteractive mode */
	uint		viewchange:1 ;
	uint		info_msg ;		/* info message */
	uint		info_err ;		/* error-info message */
} ; /* end struct */

struct bbinter_head {
	proginfo	*pip ;
	SIGACTION	*sao ;
	sigset_t	oldsigmask ;
	UTERM		ut ;
	CMDMAP		cm ;
	KEYSYMER	ks ;
	KBDINFO		ki ;
	DISPLAY		di ;
	MAILBOX		mb ;
	MBCACHE		mc ;
	MAILMSGFILE	msgfiles ;	/* mail-msg file-name DB */
	MAILMSGVIEWER	mv ;
	cchar		*mbname ;
	cchar		*pathprefix ;
	cchar		*vmdname ;
	time_t		ti_start ;	/* startup */
	time_t		ti_poll ;
	time_t		ti_clock ;
	time_t		ti_mailcheck ;
	time_t		ti_mailinfo ;
	time_t		ti_info ;
	bbinter_flags	fl ;
	uint		magic ;
	int		displines ;	/* total lines */
	int		scanlines ;	/* scan lines */
	int		viewlines ;	/* view lines */
	int		jumplines ;
	int		tag_info ;
	int		nmsgs ;
	int		nmsgdels ;	/* number mail deletions */
	int		miscantop ;	/* msg-index at top of scan display */
	int		miscanpoint ;	/* msg-index pointed to */
	int		miviewpoint ;	/* msg-index being viewed */
	int		lnviewtop ;	/* top line being viewed */
	int		nviewlines ;
	int		plen ;		/* prompt length */
	int		runcount ;	/* testing */
	int		numlen ;
	int		mfd ;		/* mailbox file descriptor */
	int		delmark ;	/* deletion mark character for scans */
	int		tfd ;
	char		numbuf[BBINTER_NUMLEN + 1] ;
} ; /* end struct */

EXTERNC_begin

extern int	bbinter_start(BBINTER *,proginfo *) noex ;
extern int	bbinter_cmd(BBINTER *) noex ;
extern int	bbinter_finish(BBINTER *) noex ;

EXTERNC_end


#endif /* BBINTER_INCLUDE */



