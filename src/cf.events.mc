;// cf.events.mc: Definition of logging events for windows.
;// This file is used to generate cf.events.h and cf.events.dll as follows:
;// mc cf.events.mc
;// rc cf.events.rc
;// link -dll -noentry cf.events.res /MACHINE:X86  # or X64
;// Save to cf.events.i686.dll or cf.events.x86_64.dll respectively
;// mc and rc are available from windows sdk, link from visual studio.
;// NOTE: Registry entry "TypesSupported" needs to be changed if the
;// number of messages below are changed - see EVENT_COUNT in cf.nova.h.

;// MessageId: EV's "Event" column
;// Severity: Informational / Warning / Error

MessageId    = 100
Severity     = Informational
Facility     = Application
SymbolicName = EVMSG_PROMISE_KEPT
Language     = English
%1A promise by %2 to %3 was already kept. Details follow.%r%r
%4
.

MessageId    = 101
Severity     = Informational
Facility     = Application
SymbolicName = EVMSG_PROMISE_REPAIRED
Language     = English
%1A promise by %2 to %3 has been repaired. Details follow.%r%r
%4
.

MessageId    = 102              
Severity     = Error
Facility     = Application
SymbolicName = EVMSG_PROMISE_NOT_REPAIRED_POLICY
Language     = English
%1A promise by %2 to %3 was not kept and has not been repaired due to
policy reasons (dry-run or action_policy set to warn). Details follow.%r%r
%4
.

MessageId    = 103
Severity     = Error
Facility     = Application
SymbolicName = EVMSG_PROMISE_NOT_REPAIRED
Language     = English
%1A promise by %2 to %3 was not kept and has not been repaired. Details follow.%r%r
%4
.

MessageId    = 104
Severity     = Informational
Facility     = Application
SymbolicName = EVMSG_REPORT
Language     = English
%1A report-promise follows.%r%r
%2
.

MessageId    = 105
Severity     = Informational
Facility     = Application
SymbolicName = EVMSG_DEFAULT_INFO
Language     = English
%1%2
.

MessageId    = 106
Severity     = Informational
Facility     = Application
SymbolicName = EVMSG_DEFAULT_VERBOSE
Language     = English
%1%2
.

MessageId    = 107
Severity     = Warning
Facility     = Application
SymbolicName = EVMSG_DEFAULT_WARN
Language     = English
%1%2
.

MessageId    = 108
Severity     = Error
Facility     = Application
SymbolicName = EVMSG_DEFAULT_ERROR
Language     = English
%1%2
.
