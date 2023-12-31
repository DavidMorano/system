.RP
....TM 76-1274-16 39199 39199-11
....ND October 8, 1976
.nr CW 2.85i
.nr GW .3i
.TL
Typing Documents on the UNIX System:
.br
\!.br
Using the \-ms Macros with Troff and Nroff
.AU "MH 2C-572" 6377
M.E. Lesk
.AI
.MH
.OK
Text Formatting
Phototypesetting
.AB
This document describes a set of easy-to-use macros
for preparing documents on the UNIX system.
Documents may be produced on either the
phototypesetter or a on a computer terminal,
without changing the input.
.PP
The macros provide facilities for paragraphs, sections (optionally
with automatic numbering), page titles, footnotes,
equations,
tables, two-column format, and
cover pages for papers.
.PP
This memo includes, as an appendix,
the text of the ``Guide to Preparing
Documents with \-ms''
which contains additional examples
of features of \-ms.
.PP
This manual is a revision of, and replaces,
``Typing Documents on UNIX,''
dated November 22, 1974.
.AE
.CS 6 6 12 1 0 8
.bd I 3
.PP
.I
Introduction.
.R
This memorandum describes a package of commands to produce
papers
using the
.bd I
.I
troff
.R
and
.I nroff
formatting programs on the
.SM
UNIX
.NL
system.
As with other
.I roff -derived
programs,
text is prepared interspersed with formatting commands.
However, this package,
which itself is written in
.I troff
commands,
provides higher-level commands
than those provided with the basic
.I troff
program.
The commands available in this package are listed in
Appendix A.
.bd I 3
.PP
.I
Text.
.R
Type normally, except that instead of indenting for paragraphs,
place a line reading ``.PP'' before each paragraph.
This will produce indenting and extra space.
.LP
Alternatively, the command .LP that was used here will produce
a left-aligned (block) paragraph.
The paragraph spacing can be changed: see below under ``Registers.''
.PP
.I
Beginning.
.R
For a document with a paper-type cover sheet, the input should start as follows:
.DS L
	[optional overall format .RP \- see below]
	.TL
	Title of document (one or more lines)
	.AU
	Author(s) (may also be several lines)
	.AI
	Author's institution(s)
	.AB
	Abstract; to be placed on the cover sheet of a paper.
	Line length is 5/6 of normal; use .ll here to change.
	.AE  (abstract end)
	text ... (begins with .PP, which see)
.DE
To omit some of the standard headings
(e.g. no abstract, or no author's institution) just
omit the corresponding fields and command lines.
The word
.SM
ABSTRACT
.NL
can be suppressed by writing ``.AB no'' for ``.AB''.
Several interspersed .AU and .AI lines can be used for multiple authors.
The headings are not compulsory: beginning
with a .PP command is perfectly OK and will just
start printing an ordinary paragraph.
.I Warning:
You can't just begin a document with a line of text.
Some \-ms command must
precede any text input.  When in doubt, use .LP
to get proper initialization, although any of
the commands .PP, .LP, .TL, .SH, .NH is good enough.
Figure 1 shows the legal arrangement of commands at the
start of a document.
.PP
.I
Cover Sheets and First Pages.
.R
The first line
of a document signals the general format of the first page.
In particular, if it is ".RP" a cover sheet with title and
abstract is prepared.
The default format
is useful for scanning drafts.
.PP
In general \-ms is arranged so that only one form
of a document need be stored, containing all
information;  the first command gives the format,
and unnecessary items for that format are ignored.
.PP
Warning: don't put extraneous material
between the .TL and .AE commands.  Processing
of the titling items is
special, and other data placed in them may not behave
as you expect.
Don't forget that some \-ms command must precede any input text.
.PP
.I
Page headings.
.R
The \-ms macros, by default, will print a page heading containing
a page number (if greater than 1).
A default page footer is provided only in
.I nroff ,
where the date is used.
The user can make minor adjustments to the page headings/footings
by redefining the
strings
LH, CH, and RH
which are the left, center and right portions of the page headings,
respectively; and the
strings
LF, CF, and RF,
which are the left, center and right portions of the page footer.
For more complex formats, the user can redefine
the macros PT and BT, which are invoked respectively at the top
and bottom of each page.
The margins (taken from registers HM and FM for the top and bottom
margin respectively) are normally 1 inch; the page header/footer are
in the middle of that space.
The user who redefines these macros should be careful
not to change parameters such as point size or font
without resetting them to default values.
.PP
.2C
.I
Multi-column formats.
.R
If you place the command ``.2C'' in your document, the document will
be printed in double column format beginning
at that point.  This feature is not too useful in computer
terminal output, but is often desirable on the typesetter.
The command ``.1C'' will go
back to one-column format and also skip to a new page.
The ``.2C'' command is actually a special case of the command
.DS L
	.MC [column width [gutter width]]
.DE
which makes multiple columns with the specified column
and gutter width; as many columns as will fit across the page
are used.
Thus triple, quadruple, ... column pages can be printed.
Whenever the number of columns is changed (except going from
full width to some larger number of columns)
a new page is started.
.PP
.I
Headings.
.R
To produce a special heading, there are two commands.
If you type
.DS L
	.NH
	type section heading here
	may be several lines
.DE
you will get automatically numbered section headings (1, 2, 3, ...),
in boldface.
For example,
.DS L
    .NH
    Care and Feeding of Department Heads
.DE
produces
.NH
Care and Feeding of Department Heads
.PP
Alternatively,
.DS L
	.SH
	Care and Feeding of Directors
.DE
will print the heading with no number added:
.SH
Care and Feeding of Directors
.PP
Every section heading, of either type, should be followed
by a paragraph beginning with .PP or .LP, indicating
the end of the heading.
Headings may contain more than one line
of text.
.PP
The .NH command also supports more complex numbering schemes.
If a numerical argument is given, it is taken to be a
``level'' number and an appropriate sub-section
number is generated.
Larger level numbers indicate deeper
sub-sections, as in this example:
.DS L
	.NH
	Erie-Lackawanna
	.NH 2
	Morris and Essex Division
	.NH 3
	Gladstone Branch
	.NH 3
	Montclair Branch
	.NH 2
	Boonton Line
.DE
generates:
.NH
Erie-Lackawanna
.NH 2
Morris and Essex Division
.NH 3
Gladstone Branch
.NH 3
Montclair Branch
.NH 2
Boonton Line
.PP
An explicit ``.NH 0'' will reset the numbering of level 1
to one, as here:
.DS L
	.NH 0
	Penn Central
.DE
.ft 3
.if n .ul 1
.sp 1
1.  Penn Central
.PP
.I
Indented paragraphs.
.R
(Paragraphs with hanging numbers, e.g. references.)
The sequence
.DS L
	.IP [1]
	Text for first paragraph, typed
	normally for as long as you would
	like on as many lines as needed.
	.IP [2]
	Text for second paragraph, ...
.DE
produces
.IP [1]
Text for first paragraph, typed normally for as long
as you would like on as many lines as
needed.
.IP [2]
Text for second paragraph, ...
.LP
A series of indented paragraphs may be followed by an ordinary paragraph
beginning with .PP or .LP,
depending on whether you wish indenting or not.
The command .LP was used here.
.PP
More sophisticated uses of .IP are also possible.
If the label is omitted, for example, a plain block indent
is produced.
.DS L
	.IP
	This material will
	just be turned into a
	block indent suitable for quotations or
	such matter.
	.LP
.DE
will produce
.IP
This material
will just be turned
into a block indent
suitable for
quotations or such matter.
.LP
If a non-standard amount of indenting is required,
it may be specified after the label (in character positions)
and will remain in effect until the next .PP or .LP.
Thus, the general form of the .IP command
contains two additional fields: the label and the indenting
length.  For example,
.DS L
	.IP first: 9
	Notice the longer label, requiring larger
	indenting for these paragraphs.
	.IP second:
	And so forth.
	.LP
.DE
produces this:
.IP first: 9
Notice the longer label, requiring larger
indenting for these paragraphs.
.IP second:
And so forth.
.LP
It is also possible to produce multiple nested indents;
the command .RS indicates that the next .IP starts from the
current indentation level.
Each .RE will eat up one level of indenting
so you should balance .RS and .RE commands.
The .RS command should be thought of as ``move right'' and
the .RE command as ``move left''.
As an example
.DS L
	.IP 1.
	Bell Laboratories
	.RS
	.IP 1.1
	Murray Hill
	.IP 1.2
	Holmdel
	.IP 1.3
	Whippany
	.RS
	.IP 1.3.1
	Madison
	.RE
	.IP 1.4
	Chester
	.RE
	.LP
.DE
will result in
.IP 1.
Bell Laboratories
.RS
.IP 1.1
Murray Hill
.IP 1.2
Holmdel
.IP 1.3
Whippany
.RS
.IP 1.3.1
Madison
.RE
.IP 1.4
Chester
.RE
.LP
All of these variations on .LP leave the right
margin untouched.  Sometimes, for purposes
such as setting off a quotation, a paragraph indented
on both right and left is required.
.QP
A single paragraph
like this is obtained
by preceding it with .QP.
More complicated material (several paragraphs) should be
bracketed with .QS and .QE.
.LP
.I
Emphasis.
.R
To get
italics
(on the typesetter) or underlining (on the terminal)
say
.DS L
	.I
	as much text as you want
	can be typed here
	.R
.DE
.bd I
.br
as was done for
.I
these three words.
.R
The .R command restores the normal (usually Roman) font.
If only one word is to be italicized, it
may be just given on the line with the .I command,
.br
.bd I 3
.DS
	.I word
.DE
and in this case no .R is needed to restore
the previous font.
.B
Boldface
.R
can be produced by
.DS L
	.B
	Text to be set in boldface
	goes here
	.R
.DE
and also will be underlined on the terminal or line printer.
As with .I, a single word can be placed in boldface
by placing it on the same line as the .B command.
.PP
A few size changes
can be specified similarly with
the commands .LG (make larger), .SM (make smaller), and .NL
(return to normal size).
The size change
is two points; the commands may be repeated for
.SM
increased
.SM
effect
.NL
(here one .NL canceled two .SM commands).
.PP
If actual
.UL underlining
as opposed to italicizing is required on the typesetter,
the command
.DS
	.UL word
.DE
will underline a word.  There is no way to underline
multiple words on the typesetter.
.PP
.I
Footnotes.
.R
Material placed between lines with the commands .FS
(footnote) and .FE (footnote end) will
be collected, remembered, and finally placed
at the bottom of the current page*.
By default, footnotes are 11/12th the
length of normal text,
but this can be changed using the FL register (see below).
.FS
* Like this.
.FE
.PP
.I
Displays and Tables.
.R
To prepare displays of lines, such as tables, in which
the lines should not be re-arranged,
enclose them in the commands .DS and .DE
.DS L
	.DS
	table lines, like the
	examples here, are placed
	between .DS and .DE
	.DE
.DE
By default, lines between .DS and .DE are indented and left-adjusted.
You can also center lines, or retain the left margin.
Lines bracketed by .DS C and .DE commands are
centered (and not re-arranged); lines bracketed
by .DS L and .DE are left-adjusted, not indented, and
not re-arranged.
A plain .DS is equivalent
to .DS I, which indents and left-adjusts.  Thus,
.DS C
these lines were preceded
by .DS C and followed by
a .DE command;
.DE
whereas
.DS L
these lines were preceded
by .DS L and followed by
a .DE command.
.DE
Note that .DS C centers each line; there is a variant .DS B
that makes the display into a left-adjusted block of text, and
then centers that entire block.
Normally a display is kept together, on one page.
If you wish to have a long display which
may be split across page
boundaries,
use .CD, .LD, or .ID in place of
the commands .DS C, .DS L, or .DS I respectively.
An extra argument to the .DS I or .DS command is taken
as an amount to indent.
Note: it is tempting to assume that .DS R will right adjust
lines, but it doesn't work.
.PP
.I
Boxing words or lines.
.R
To draw rectangular boxes around words the command
.DS L
	.BX word
.DE
will print
.BX word
as shown.
The boxes will not be neat on a terminal, and this
should not be used as a substitute for italics.
.B1
Longer pieces of text may be boxed
by enclosing them with .B1 and .B2:
.DS L
	.B1
	text...
	.B2
.DE
as has been done here.
.B2
.PP
.I
Keeping blocks together.
.R
If you wish to keep a table or other block of lines
together on a page, there are ``keep - release'' commands.
If a block of lines preceded by .KS and followed by .KE does
not fit on the remainder of the current page, it will begin
on a new page.
Lines bracketed by .DS and .DE commands are automatically
kept together this way.
There is also a ``keep floating'' command: if the
block to be kept together is preceded by .KF instead of .KS
and does not fit
on the current page, it will be moved down through the text
until the top of the next page.  Thus, no large blank space
will be introduced in the document.  
.PP
.I
Nroff/Troff commands.
.R
Among the useful commands from the basic formatting programs
are the following.  They all work with both typesetter and
computer terminal output:
.DS L
	.bp - begin new page.
	.br - ``break'', stop running text
	          from line to line.
	.sp n - insert n blank lines.
	.na - don't adjust right margins.
.DE
.PP
.I
Date.
.R
By default, documents produced on computer terminals have the
date at the bottom of each page; documents produced on
the typesetter don't.
To force the date, say ``.DA''.  To force no date, say ``.ND''.
To lie about the date, say ``.DA July 4, 1776''
which puts the specified date at the bottom of each page.
The command
.DS L
	.ND May 8, 1945
.DE
in ".RP" format
places the specified date on the cover sheet and nowhere else.
Place this line before the title.
.PP
.I
Signature line.
.R
You can obtain a signature line by placing
the command .SG in the document.
The authors' names will
be output in place of the .SG line.
An argument to .SG
is used
as a typing identification line, and
placed after the signatures.
The .SG command is ignored
in released paper format.
.PP
.I
Registers.
.R
Certain of the registers used by \-ms can
be altered to change default
settings.
They should be changed with .nr commands,
as with
.DS
	.nr PS 9
.DE
.bd I
to make the default point size 9 point.
If the effect is needed immediately, the
normal
.I
troff
.R
command should be used
in addition to changing the number register.
.br
.ps 9
.vs 10p
.TS
c0 c c c
c c c c
a l l l.
Register	Defines	Takes	Default
		effect
PS	point size	next para.	10
VS	line spacing	next para.	12 pts
LL	line length	next para.	6\(fm\(fm
LT	title length	next para.	6\(fm\(fm
PD	para. spacing	next para.	0.3 VS
PI	para. indent	next para.	5 ens
FL	footnote length	next FS	11/12 LL
CW	column width	next 2C	7/15 LL
GW	intercolumn gap	next 2C	1/15 LL
PO	page offset	next page	26/27\(fm\(fm
HM	top margin	next page	1\(fm\(fm
FM	bottom margin	next page	1\(fm\(fm
.TE
.ps \n(PS
.vs \n(VS
You may also alter
the strings
LH, CH, and RH which are the left, center, and right headings
respectively; and similarly LF, CF, and RF which are strings in the
page footer.
The page number on
.I
output
.R
is taken from register PN, to permit
changing its output style.
For more complicated headers and footers
the macros PT and BT can be redefined, as
explained earlier.
.bd I 3
.PP
.I
Accents.
.R
To simplify typing certain foreign words,
strings representing common accent marks are defined.
They precede the letter over which the mark
is to appear.
Here are the strings:
.TS
center;
c c6 c c.
Input	Output	Input	Output
\e*\(fme	\*'e	\e*~a	\*~a
\e*\(gae	\*`e	\e*Ce	\h'0.15m'\v'-0.6m'\s6\zv\s0\v'0.6m'\h'-0.15m'e
\e*:u	\*:u	\e*,c	\*,c
\e*^e	\o'^e'
.TE
.PP
.I
Use.
.R
After your document is prepared and stored on a file,
you can print it on a terminal with the command*
.bd I
.FS
* If .2C was used, pipe the
.I nroff
output
through
.I col;
make the first line of the input
``.pi /usr/bin/col.''
.br
.FE
.DS L
.I
   nroff \-ms file
.R
.DE
and you can print it on the typesetter with the
command
.DS L
.I
   troff \-ms file
.R
.DE
(many options are possible).
In each case, if your document is stored in several files,
just list all the filenames
where we have used ``file''.
If equations or tables are used,
.I
eqn
.R
and/or
.I
tbl
.R
must be invoked as preprocessors.
.br
.bd I 3
.PP
.I
References and further study.
.R
If you have to do Greek or mathematics, see
.I eqn
[1]
for equation setting.
To aid
.I eqn
users,
.I \-ms
provides definitions of .EQ and .EN
which normally center the equation and set it off slightly.
An argument on .EQ is taken to be an equation
number and placed in the right margin near the equation.
In addition, there are three special arguments to EQ:
the letters C, I, and L indicate centered (default),
indented, and left adjusted equations, respectively. 
If there is both a format argument
and an equation number,
give the format argument first, as in
.bd I
.DS
	.EQ L (1.3a)
.DE
for a left-adjusted equation numbered (1.3a).
.PP
Similarly,
the macros .TS and .TE
are defined
to separate tables (see [2]) from text with a little space.
A very long table with a heading may be broken
across pages by beginning it with .TS H
instead of .TS,
and placing the line .TH in the table data
after the heading.  If the table
has no heading repeated from page to page,
just use the ordinary .TS and .TE macros.
.PP
To learn more about
.I troff
see
[3] for a general introduction, and [4]
for the full details (experts only).
Information on related UNIX commands
is in [5].
For jobs that do not seem well-adapted
to \-ms, consider other macro packages.
It is often far easier to write a specific macro packages
for such tasks as imitating particular journals than
to try to adapt \-ms.
.PP
.bd I 3
.I
Acknowledgment.
.R
Many thanks are due to Brian Kernighan for
his help in the design and implementation of this package,
and for his assistance in preparing this manual.
.bd I
.SH
.ce
References
.PP
.IP [1]
B. W. Kernighan and L. L. Cherry,
.I
Typesetting Mathematics \(em Users Guide (2nd edition),
.R
Bell Laboratories Computing Science Report no. 17.
.IP [2]
M. E. Lesk,
.I
Tbl \(em A Program to Format Tables,
.R
Bell Laboratories Computing Science Report no. 45.
.IP [3]
B. W. Kernighan,
.I
A Troff Tutorial,
.R
Bell Laboratories, 1976.
.IP [4]
J. F. Ossanna,
.I
Nroff\|/Troff Reference Manual,
.R
Bell Laboratories Computing Science Report no. 51.
.IP [5]
K. Thompson and D. M. Ritchie,
.I
UNIX Programmer's Manual,
.R
Bell Laboratories, 1978.
.1C
.SH
.ce
Appendix A
.ce
List of Commands
.ft R
.TS
expand;
l2 l5 l2 l.
1C	Return to single column format.	LG	Increase type size.
2C	Start double column format.	LP	Left aligned block paragraph.
AB	Begin abstract.	
AE	End abstract.	
AI	Specify author's institution.	
AU	Specify author.	ND	Change or cancel date.
B	Begin boldface.	NH	Specify numbered heading.
DA	Provide the date on each page.	NL	Return to normal type size.
DE	End display.	PP	Begin paragraph.
DS	Start display (also CD, LD, ID).	
EN	End equation.	R	Return to regular font (usually Roman).
EQ	Begin equation.	RE	End one level of relative indenting.
FE	End footnote.	RP	Use released paper format.
FS	Begin footnote.	RS	Relative indent increased one level.
		SG	Insert signature line.
I	Begin italics.	SH	Specify section heading.
		SM	Change to smaller type size.
IP	Begin indented paragraph.	TL	Specify title.
KE	Release keep.	
KF	Begin floating keep.	UL	Underline one word.
KS	Start keep.	
.TE
.sp
.ce
.ft B
Register Names
.ft R
.PP
The following register names are used by \-ms internally.
Independent use of these names in one's own macros may
produce incorrect output.
Note that no lower case letters are used in any \-ms internal name.
.TS
 expand;
c s s  s s s  s s s  s s
l l l  l l l  l l l  l l.
Number registers used in \-ms
:	DW	GW	HM	IQ	LL	NA	OJ	PO	T.	TV
#T	EF	H1	HT	IR	LT	NC	PD	PQ	TB	VS
.T	FC	H2	IF	IT	MF	ND	PE	PS	TC	WF
1T	FL	H3	IK	KI	MM	NF	PF	PX	TD	YE
AV	FM	H4	IM	L1	MN	NS	PI	RO	TN	YY
CW	FP	H5	IP	LE	MO	OI	PN	ST	TQ	ZN
.TE
.sp
.TS
expand;
c s s  s s s  s s s  s s
l l l  l l l  l l l  l l.
String registers used in \-ms
\(fm	A5	CB	DW	EZ	I	KF	MR	R1	RT	TL
\(ga	AB	CC	DY	FA	I1	KQ	ND	R2	S0	TM
^	AE	CD	E1	FE	I2	KS	NH	R3	S1	TQ
~	AI	CF	E2	FJ	I3	LB	NL	R4	S2	TS
:	AU	CH	E3	FK	I4	LD	NP	R5	SG	TT
,	B	CM	E4	FN	I5	LG	OD	RC	SH	UL
1C	BG	CS	E5	FO	ID	LP	OK	RE	SM	WB
2C	BT	CT	EE	FQ	IE	ME	PP	RF	SN	WH
A1	C	D	EL	FS	IM	MF	PT	RH	SY	WT
A2	C1	DA	EM	FV	IP	MH	PY	RP	TA	XD
A3	C2	DE	EN	FY	IZ	MN	QF	RQ	TE	XF
A4	CA	DS	EQ	HO	KE	MO	R	RS	TH	XK
.TE
.ne 4i
.br
.ne 5i
.ta 1i 2i 3i 4i
.vs .6i
.nf
.in 1i
.ll 4.2i
.ce
Order of Commands in Input

	RP
TL
AU
AI
	AB
	AE

		NH, SH
			PP, LP
			text ...
.br
.ce
Figure 1
.fi
.in 0
