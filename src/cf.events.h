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
// A promise by %1 to %2 was already kept. Details follow.
// %3
//
#define EVMSG_PROMISE_KEPT               0x40000064L

//
// MessageId: EVMSG_PROMISE_REPAIRED
//
// MessageText:
//
// A promise by %1 to %2 has been repaired. Details follow.
// %3
//
#define EVMSG_PROMISE_REPAIRED           0x40000065L

//
// MessageId: EVMSG_PROMISE_NOT_REPAIRED_POLICY
//
// MessageText:
//
// A promise by %1 to %2 was not kept and has not been repaired due to
// policy reasons (dry-run or action_policy set to warn). Details follow.
// %3
//
#define EVMSG_PROMISE_NOT_REPAIRED_POLICY 0xC0000066L

//
// MessageId: EVMSG_PROMISE_NOT_REPAIRED
//
// MessageText:
//
// A promise by %1 to %2 was not kept and has not been repaired. Details follow.
// %3
//
#define EVMSG_PROMISE_NOT_REPAIRED       0xC0000067L

//
// MessageId: EVMSG_DEFAULT_INFO
//
// MessageText:
//
// %1
//
#define EVMSG_DEFAULT_INFO               0x40000068L

//
// MessageId: EVMSG_DEFAULT_WARN
//
// MessageText:
//
// %1
//
#define EVMSG_DEFAULT_WARN               0x80000069L

//
// MessageId: EVMSG_DEFAULT_ERROR
//
// MessageText:
//
// %1
//
#define EVMSG_DEFAULT_ERROR              0xC000006AL

