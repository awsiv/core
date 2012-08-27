#!/usr/bin/python
###############################################################################
#
# mongodb-migration.py
#
# This script operates on a migration log. The log is an a sequence of
# migration steps, each a tuple (version, description, migration_function).
# The version is the database version prerequisite for applying the step
# After the step is applied, the version is incremented.
#
###############################################################################

import sys
import pymongo


def get_migration_object(conn):
    info = conn.cfreport.migration.find_one()
    if info is None:
        info = {'version': 0}
        conn.cfreport.migration.insert(info)
    return info


def get_version(conn):
    return get_migration_object(conn)['version']


def set_version(conn, version):
    info = get_migration_object(conn)
    info['version'] = version
    conn.cfreport.migration.save(info, safe=True)


def migrate_vr_to_namespaces(conn):
    """
        Transform the host.vr record to store
        a 'default' namespace for the top level, rather
        than the old 'scope' (bundle) top level.
    """
    for host in conn.cfreport.hosts.find():
        host['vr'] = {'default': host['vr']}
        conn.cfreport.hosts.save(host)


def migrate(conn, migration_log):
    version = get_version(conn)
    print 'Detected database version', version
    for (step, name, migration_fn) in migration_log:
        version = get_version(conn)
        if step < version:
            print 'Skipping step:', step, '(', name, ')'
            continue
        elif step > version:
            print 'Attempted to apply step ', step,
            'to a database of version', version
            print 'Aborting'
            sys.exit(1)

        set_version(conn, step + 1)
        print 'Applying migration step', step, '(', name, '):',
        migration_fn(conn)
        print 'OK'
    print 'Migration complete!'


def accept_warning():
    warning = """
        WARNING: As MongoDB does not provide transactional support,
        it is recommended that you back up the database before proceeding.
    """
    print warning
    answer = raw_input("Please enter Y to proceed: ")
    return answer[0] == 'Y'


def print_usage():
    print 'Usage: mongodb-migration <host> <port>'
    sys.exit()


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print_usage()
        sys.exit()

    host = sys.argv[1]
    port = 0
    try:
        port = int(sys.argv[2])
    except ValueError:
        print 'Invalid port number'
        sys.exit()

    conn = None
    try:
        conn = pymongo.Connection(host, port)
    except Exception as e:
        print 'Could not connect to mongod, host:', host, ' port:', port
        print 'Error: ', e
        sys.exit()

    if not accept_warning():
        sys.exit()

    migrate(conn, [
        (0, 'Put existing variables in default namespace',
         migrate_vr_to_namespaces),
    ])
