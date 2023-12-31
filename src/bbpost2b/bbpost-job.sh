#!/bin/ksh
# BBPOST-JOB

# 9/30/84 JIS
# the delete feature has been removed, and all posting is now done by using
# the new smail. Only the user interface is provided by this shell script.

# 7/24/89 LEEPER
# all posting now done via PDU

# 11/13/89 JIS
# use NFS to do posting from Suns.

# 94/11/29 DAM
#	fixed some "unknown command" bugs and got rid of special treatment
#	for machine 'mtsol'


VERSION="1/dam"


if [ -d /usr/sbin ] ; then

  UNAME=`uname -n`

else

  UNAME=`hostname`

fi

P=`basename ${0}`

getit() {

	    ypmatch $1 passwd
}

BBDIR=${BBDIR-/usr/add-on/pcs/lib/boards} # location on SUN
ABBDIR=/usr/ncmp/pcs/lib/boards # location on AMDAHL

if test -z "${BBDIR}" ; then 	

	/usr/5bin/echo "${P}: BBDIR not defined" >&2
	exit 1
fi

# get user long name

# remember current directory
DIR=`pwd`

ANSWER=""

# check for "immediate edit option"
if test "$1" = "+e" -o "$1" = "+ed" -o "$1" = "+edi" -o "$1" = "+edit"
then 	
	ANSWER=edit
	shift
fi

if test "$1" = "+m" -o "$1" = "+ma" -o "$1" = "+mai" -o "$1" = "+mail"
then	
	MAILTO=yes
	shift
fi

if test "$2" = "+m" -o "$2" = "+ma" -o "$2" = "+mai" -o "$2" = "+mail"
then	
	MAILTO=yes
fi

# get board name and bulletin title

BOARD=$1
while
	test -z "$BOARD" -a -z "$ANSWER"
do

	/usr/5bin/echo "BOARD: \c"
	read BOARD

	if test -z "${BOARD}" ; then	

		/usr/5bin/echo "enter the name of a board"
		/usr/5bin/echo "enter \"?\" to obtain names of existing boards"
		continue
	fi

	if test "${BOARD}" = "?" ; then 	

		/usr/5bin/echo "enter the name of a board"
		/usr/5bin/echo "the following are \c"
		bb +names +every_board
		BOARD=""
		continue
	fi

	if test "${BOARD}" = . ; then 	

		ANSWER=edit
		BOARD=""
		continue
	fi

	cd $BBDIR
	if test ! -d ${BOARD} ; then 	

		/usr/5bin/echo "board \"${BOARD}\" does not exist"
		/usr/5bin/echo "enter \"?\" to obtain names of existing boards"
		BOARD=""
	fi
	cd $DIR

	if [ "x`/usr/5bin/echo ${BOARD} | grep general`" != "x" ] ; then

	/usr/5bin/echo "The $BOARD board is for work-related items only."
	/usr/5bin/echo "For-sale and other non-work-related items should go"
	/usr/5bin/echo "on the misc board.  Is this item work-related [y/n]? "
		read foo
		test "x${foo}" = xy || BOARD=""
	fi

done


while test -z "${TITLE}" -a -z "${ANSWER}" ; do

	/usr/5bin/echo "TITLE: \c"
	TITLE=`line`

	if test -z "${TITLE}" ; then	

	/usr/5bin/echo "enter the title for the bulletin"
	/usr/5bin/echo "enter \"?\" to obtain names of existing bulletins"
		continue
	fi

	# check for ? (help)

if test "${TITLE}" = "?" ; then 	

	/usr/5bin/echo "enter a title for the bulletin"
	/usr/5bin/echo "the title may contain spaces "
	/usr/5bin/echo "the following titles already exist on \c"
		bb -subject -a "${BOARD}"
		TITLE=""
		continue
	fi

	if test "${TITLE}" = . ; then	

		ANSWER=edit
		TITLE=""
		continue
	fi

	# create file name
	# check for existing name

	FILE=`bbfile ${TITLE}`
	OWNER=""

	if test -z "${BBDIR}/${BOARD}/${FILE}" ; then 	

		exit
	fi

	if test ! "${TITLE}" = "${FILE}" ; then	:

#	/usr/5bin/echo "note: \"${FILE}\" will be used for name of bulletin"
	fi

	if test -f "${BBDIR}/${BOARD}/${FILE}" ; then 	

/usr/5bin/echo "title \"${FILE}\" already exists, please choose another"
			TITLE=""
		fi
done

TMP=/tmp/bbpost$$
trap "
cat <<!!
You must have hit break.  That kills $0
Your recovery file is in $TMP.
Please try to post again and pull $TMP into the editor.
!!
exit
" 2

trap "rm -f /tmp/bbpost${$} ; exit 1" 1 15

	/usr/5bin/echo "BOARD: ${BOARD}\nTITLE: ${TITLE}\n" > $TMP
 	if test ! "${ANSWER}" = "edit" ; then 	

		/usr/5bin/echo "enter text for bulletin terminated by ."
		while
			#read TEXT
			TEXT=`line`
			test ! "${TEXT}" = "."
		do
			/usr/5bin/echo "${TEXT}" >> $TMP
		done
	fi


while true ; do

	if test -z "${ANSWER}" ; then	

	/usr/5bin/echo "\npost, review, edit, or quit ? [post] \c"
		read ANSWER
	fi

	if test -z "${ANSWER}" ; then 
		ANSWER=post
	fi

	case "${ANSWER}" in

	e | ed | edi | edit)

		ANSWER=""
		/usr/5bin/echo "editing bulletin ..."
		${ED-ed} $TMP
		# check for title & board change
		TITLE=`grep '^TITLE' ${TMP} | sed "s/.*:[ 	]*//"`

BOARD=`grep '^BOARD' ${TMP} | sed "s/.*:[ 	]*//" | sed "s/[	 ]*$//"`

		if test -z "${BOARD}" ; then 	

			/usr/5bin/echo warning: bulletin board not defined.
			/usr/5bin/echo "Please edit the name of the board."
			ERROR=board
			continue
		fi

		if test ! -d "${BBDIR}/${BOARD}" ; then 	

	/usr/5bin/echo "warning: bulletin board \"${BOARD}\" does not exist"
			/usr/5bin/echo "please edit the name of the board"
			/usr/5bin/echo "the following are available boards\n"
			bb +names +every_board
			ERROR=board
			continue
		fi

	TITLE=`grep TITLE ${TMP} | sed "s/.*:[ 	]*//" | sed "s/[	 ]*$//"`
		if test -z "${TITLE}" ; then 	

			/usr/5bin/echo "warning: title not defined"
			/usr/5bin/echo "please edit the title of the bulletin"
			ERROR=title
			continue
		fi

		FILE=`bbfile ${TITLE}`
#		echo PLACE HERE
		if test ! "${TITLE}" = "${FILE}" ; then	

	/usr/5bin/echo "Note: \"${FILE}\" will be used for name of bulletin"
		fi
	AFILE="${BBDIR}/${BOARD}/${FILE}"
	if test -f "${AFILE}" -a ! "${AFILE}" = "${OLDFILE}" ; then

	/usr/5bin/echo \"${FILE}\" already exists on \"${BOARD}\" board
			/usr/5bin/echo "please edit the title of the bulletin"
			/usr/5bin/echo "the following are existing titles on \c"
			bb +old_titles  $BOARD
			ERROR=title
			continue
		fi

		ERROR=""
		;;

	r | re | rev |revi | revie | review)
		ANSWER=""
		trap "continue" 1 2 3 15
		if test -s $TMP ; then 

			/usr/5bin/echo ""
	   	     cat $TMP
		else /usr/5bin/echo "bulletin is empty"
		fi
		;;

	p | po | pos | post)

		ANSWER=""

		if test "${ERROR}" = board ; then 	

			if test -z "${BOARD}" ; then 	

		/usr/5bin/echo "warning: bulletin board not defined"
			else 	
	/usr/5bin/echo "warning: bulletin board \"${BOARD}\" does not exist"
			fi
			/usr/5bin/echo "please edit the name of the board"
			continue
		fi

		if test "${ERROR}" = title ; then 	

			if test -z "${TITLE}" ; then 	

		/usr/5bin/echo "warning: title not defined"
			else 	
		/usr/5bin/echo "article title \"${FILE}\" already exists"
			fi
			/usr/5bin/echo "please edit the title of the bulletin"
			continue
		fi
		ANSWER=""
		# post on local system
		if test ! -s "${TMP}" ; then

		/usr/5bin/echo "cannot post, bulletin is empty"
		     continue
		fi

	    	NTITLE=`/usr/5bin/echo ${TITLE} | sed -e 's/[^A-Za-z0-9_]/_/g'`
	    	USERNAME=`logname`
		BBFILE=${BBDIR}/${BOARD}/${NTITLE}
	    	{ cat <<!!
x-mailer: AT&T PCS BBPOST (version ${VERSION})
FROM:       `getit ${USERNAME} | cut -d- -f2 | cut -d"(" -f1` <!${USERNAME}>
DATE:       `date "+%d %h 19%y  %H:%M"` `date|cut -c21-23`
!!
	   	newform -i-8 -o-0 ${TMP} ; } | pcscl > $BBFILE
# DAM change 10/17/94
#		cp $HOME/.bb$$ $BBDIR/$BOARD/$NTITLE
#		pcscl $HOME/.bb$$ $BBDIR/$BOARD/$NTITLE
# end of change by DAM
		chmod 644 ${BBFILE}
		#rcp ${BBFILE} mtgzy:${BBFILE}
		#rsh mtgzy chmod 644 ${BBFILE}

		rm -f ${HOME}/.bb${$}
		exit
		;;

	q | qu | qui | quit)	rm $TMP > /dev/null 2>/dev/null
		exit 0
		;;

	\?)
		ANSWER=""
		/usr/5bin/echo "\n	post	post bulletin"
		/usr/5bin/echo "	review	print bulletin for review"
		/usr/5bin/echo "	edit	edit bulletin"
		/usr/5bin/echo "	quit	quit without posting bulletin\n"
		;;

	*)	
	/usr/5bin/echo "unknown command \"${ANSWER}\", please try again"
		ANSWER=""
		;;

	esac
done


