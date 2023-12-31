.\"_
.\" GBCS NGO Broadband
.\"_
.PF "'GBCS''Starbase Technology'"
.\"_
.\"_
.ds HF 3 3 3 3 3
.ds HP 12 12 10 10 10 10
.nr Hb 7
.nr Cl 3
.\"_
.PM P
.\"_
.EQ
delim $$
define z_odd % Z sub odd %
define z_even % Z sub even %
define ddl % D sup 2 L %
.EN
.\"_
.\"_ .ND "January 31, 1995"
.TL
Notes on Starbase Project Processor Selection,
Issue 0 (COMPAS ID 41984)
.AU "D. A. Morano" DAM HO 13F3C2000 5532 4G-433
.\"_
.AS 2 5
This document discusses some ideas concerning the selection of a 
project-wide
microprocessor (processor family architecture) decision for Starbase.
It is expected that various Starbase designs will require the
selection of a microprocessor for running firmware or software
and there are obvious advantages by trying to select a common
processor family architecture for all (or most all) of the designs
reqiring microprocessors.  This document discusses some of the
various advantages and disadvantages of the three leading processor
architecures being considered.  It will be shown (or at least suggested)
the the PowerPC process architecure may be the best choice after considering
the stage that the Starbase project is at and the nature of the
various processor requirements needed for Starbase designs.
.AE
.MT 0
.\"_
.BG - stars.gif 2.0i 4.0i l 0.0i alo
.S +8
.DS I


GBCS

STARBASE

PROJECT
.DE
.S -8
.SP 4
.\"_
.H 1 "Introduction"
.P
The use of differential buses has not been very popular to date in
the low and medium performance computer industry.
The principle factor discouraging the use of differential buses
is the fact that twice as many backplane traces
(and possibly backplane/board connector pins) must be provided
as compared with a backplane bus with only single ended signals.
.P
Backplane buses which have ventured into using differentially driven
signals have usually chosen to use convention ECL voltage swings.
Buses driven differentially with ECL do not enjoy a
particular power savings advantage over TTL flavored buses since
the total voltage swing of a differentially driven ECL bus is
1.4 to 1.8 Volts or more and thus not very much smaller than some single ended
bus applications.
.P
The bus driver used in this integrated circuit is the
first CMOS only technology to implement the new
logic family called \fIDave's Differential
Logic\fP (or $ ddl $).  This $ ddl $ interface logic is
described more fully in "A New Differential Bus Scheme."\*(Rf
.RS
.BK differential scheme morano gbcs
.RF
This new interface logic solves a number of
problems related to high speed buses for telecommunications applications.
The initial application for use of this IC will be as bus transceivers in
GBCS Adaptive Standard Interface (ASI) \*(Rf
.RS
Morano, D.A., Pitio, W.M., Shugard, D.D.,
"Adaptive Standard Interface Specification,
Draft Issue 1.4,"
11340-940401-01TM.
.RF
based switching systems.
The ASI is designed to solve many of the limitations of previous
telecommunications buses while achieving a new level of bandwidth
performance.  Prior work on implementing $ ddl $ in a BiCMOS technology
can be found in the document
"Balanced Logic And Synchronous Transceiver (BLAST) I Chip"
by Morano, et all \*(Rf
.RS
Balanced Logic And Synchronous Transceiver (BLAST) I Chip
Design Specification,
Issue 0 (COMPAS ID 37165)
D. A. Morano,
W. M. Pitio,
D. D. Shugard.
.RF
.\"_
.H 1 "The $ ddl $ Driver Interface Circuit"
.P
The $ ddl $ interface circuit must satisfy one principle requirement.
The interface circuit must either be isolated from the bus in one
logic state or actively driving both leads of a differential bus signal
in the other logic state.  Many circuit designs are possible depending
on system level requirements.
The authors are primarily interested in applications using a lower
total signal voltage swing than conventional ECL differential buses.
.P
A target loaded bus odd mode impedance of 25 Ohms matches the system
requirements of ASI.
This is a reasonable value
when a bus is designed for a nominal odd mode characteristic
impedance of 50 Ohms.  
Due to bus loading, the characteristic impedance
is assumed to be lowered to half of the geometric impedance resulting
in a 25 Ohms odd mode impedance for a realistically loaded bus.
A desired total voltage swing of 0.25 Volts has also been selected
as a reasonable experimental value.
For this total voltage swing of 0.25 Volts,
an odd mode wave current of 5 milliAmperes is required.
For a bus application, a current of 10 milliAmperes would be required
to be sourced by the driver since two waves are always propagated in 
both directions on the bus.
.P
Since differentially driven signals can have 
a factor of up to ten times better signal to noise ratio\*(Rf
.RS
Rainal, A.J.,
"Balanced Interconnections For High Performance 
and Reduced Power Dissipation,"
AT&T document 11538-921013-29TM.
.RF
\&, it is reasoned that we can afford to compromise this
ratio substantially by not
swinging the signal voltage level as great as conventional 
differential ECL systems.
By using a differential receiver,
one can still receive the signals with a signal to noise ratio of
perhaps three to five times better than a single-ended receiver
but with drivers that are using less power.
.P
An ideal representation of what the new driver circuit must emulate
is shown in Figure \_FIGIDEAL
.DS
.BP ideal.ps 5.0i 8.0i c 0.0i a
.FG "Ideal Driver Circuit"
.TAG FIGIDEAL
.DE
.\"_
.H 2 "Balanced Driver Isolation"
.P
The driver must either drive a logic one state or do nothing.
Isolation of the driver bus outputs is handled 
lead that is part of the input logic, or the isolated condition 
can be obtained
by asserting a logic zero at the input of the driver.
Having a differential state when the bus is idle solves the
unbalanced signal propagation problem inherent in conventional ECL 
buses utilizing tri-state isolation.
.\"_
.H 2 "Wired Logic Function"
.P
A wired differential "or" signal is now attainable.  If every driver
asserts a logic zero, then no drivers are driving the bus (logic zero).
If one
or more drivers assert a logic one then the signal reads back as a logic one.
.\"_
