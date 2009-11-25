;// cf.events.mc: Definition of logging events for windows.
;// This file is used to generate cf.events.h and cf.events.dll as follows:
;// mc cf.events.mc
;// rc cf.events.rc
;// link -dll -noentry cf.events.res
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
A promise by %1 to %2 was already kept. Details follow.%r%r
%3
.

MessageId    = 101
Severity     = Informational
Facility     = Application
SymbolicName = EVMSG_PROMISE_REPAIRED
Language     = English
A promise by %1 to %2 has been repaired. Details follow.%r%r
%3
.

MessageId    = 102              
Severity     = Error
Facility     = Application
SymbolicName = EVMSG_PROMISE_NOT_REPAIRED_POLICY
Language     = English
A promise by %1 to %2 was not kept and has not been repaired due to
policy reasons (dry-run or action_policy set to warn). Details follow.%r%r
%3
.

MessageId    = 103
Severity     = Error
Facility     = Application
SymbolicName = EVMSG_PROMISE_NOT_REPAIRED
Language     = English
A promise by %1 to %2 was not kept and has not been repaired. Details follow.%r%r
%3
.

MessageId    = 104
Severity     = Informational
Facility     = Application
SymbolicName = EVMSG_REPORT
Language     = English
A report promise follows.%r%r
%1
.

MessageId    = 105
Severity     = Informational
Facility     = Application
SymbolicName = EVMSG_DEFAULT_INFO
Language     = English
%1
.

MessageId    = 106
Severity     = Informational
Facility     = Application
SymbolicName = EVMSG_DEFAULT_VERBOSE
Language     = English
%1
.

MessageId    = 107
Severity     = Warning
Facility     = Application
SymbolicName = EVMSG_DEFAULT_WARN
Language     = English
%1
.

MessageId    = 108
Severity     = Error
Facility     = Application
SymbolicName = EVMSG_DEFAULT_ERROR
Language     = English
%1
.
