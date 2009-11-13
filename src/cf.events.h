// cf.events.mc: Definition of logging events for windows.
// This file is used to generate cf.events.h and cf.events.dll as follows:
// mc cf.events.mc
// rc cf.events.rc
// link -dll -noentry cf.events.res
// mc and rc are available from windows sdk, link from visual studio.
// NOTE: Registry entry "TypesSupported" needs to be changed if the
// number of messages below are changed - see EVENT_COUNT in cf.nova.h.
// MessageId: EV's "Event" column
// Severity: Informational / Warning / Error
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: EVMSG_PROMISE_KEPT
//
// MessageText:
//
// A promise by %1 to %2 was already kept. Details follow.%r%r
// %3
//
#define EVMSG_PROMISE_KEPT               0x40000064L

//
// MessageId: EVMSG_PROMISE_REPAIRED
//
// MessageText:
//
// A promise by %1 to %2 has been repaired. Details follow.%r%r
// %3
//
#define EVMSG_PROMISE_REPAIRED           0x40000065L

//
// MessageId: EVMSG_PROMISE_NOT_REPAIRED_POLICY
//
// MessageText:
//
// A promise by %1 to %2 was not kept and has not been repaired due to
// policy reasons (dry-run or action_policy set to warn). Details follow.%r%r
// %3
//
#define EVMSG_PROMISE_NOT_REPAIRED_POLICY 0xC0000066L

//
// MessageId: EVMSG_REPORT
//
// MessageText:
//
// A report promise follows.%r%r
// %1
//
#define EVMSG_REPORT                     0x40000067L

//
// MessageId: EVMSG_PROMISE_NOT_REPAIRED
//
// MessageText:
//
// A promise by %1 to %2 was not kept and has not been repaired. Details follow.%r%r
// %3
//
#define EVMSG_PROMISE_NOT_REPAIRED       0xC0000068L

//
// MessageId: EVMSG_DEFAULT_INFO
//
// MessageText:
//
// %1
//
#define EVMSG_DEFAULT_INFO               0x40000069L

//
// MessageId: EVMSG_DEFAULT_VERBOSE
//
// MessageText:
//
// %1
//
#define EVMSG_DEFAULT_VERBOSE            0x4000006AL

//
// MessageId: EVMSG_DEFAULT_WARN
//
// MessageText:
//
// %1
//
#define EVMSG_DEFAULT_WARN               0x8000006BL

//
// MessageId: EVMSG_DEFAULT_ERROR
//
// MessageText:
//
// %1
//
#define EVMSG_DEFAULT_ERROR              0xC000006CL

