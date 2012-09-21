#!/usr/bin/python

import sys

def make_entry(uid, password, cn, sn, o):
    return """
dn: uid=%s,ou=people,dc=localhost
objectClass: inetOrgPerson
objectClass: organizationalPerson
objectClass: person
objectClass: top
cn: %s
sn: %s
o: %s
ou: people
uid: %s
userPassword:: %s""" % (uid, cn, sn, o, uid, password.encode('base64'))

def print_usage():
    print 'Usage: generate-ldap-data.py <num-entries> <file>'

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print_usage()
        sys.exit(1)

    num_entries = int(sys.argv[1])
    file_out = open(sys.argv[2], 'w')

    print 'Writing %d entries to %s' % (num_entries, sys.argv[2])

    for i in range(0, num_entries):
        cn = 'FirstName%d' % i
        sn = 'LastName%d' % i
        o = 'Jersey'
        uid = 'user%d' % i
        password = 'pass%d' % i
        file_out.write(make_entry(uid, password, cn, sn, o))

    file_out.close()
    print 'Done!'
    sys.exit(0)
