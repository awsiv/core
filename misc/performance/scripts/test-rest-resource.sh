#!/usr/bin/env bash 

# Default parameters:
USER="admin";
PASS="admin";
CURL="/usr/bin/curl";
RESOURCE=
PORT="8888"
URL="localhost"
COUNT=50
PAGE=1


# need curl
if [ ! -x $CURL ]; then
    echo "ERROR: script require curl ($CURL)";
    exit 1;
fi

# options
VERBOSE=""
HELP="Usage: $(basename $0) [OPTION] -r resource_name
Options:
\t-h\t--help\t\t\tprint help
\t-v\t--verbose\t\tverbose mode
\t--curl\t\t\tvalue\tcurl path
\t-r\t--resource\tvalue\tresource name
\t--port\t\t\tvalue\tport
\t--username\t\tvalue\tusername for api
\t--pass\t\t\tvalue\tpass for api
\t--url\t\t\tvalue\tbase url for REST server
\t--hostkey\t\tvalue\t hostkey
\t--page\t\tvalue\t (default 1) page number
\t--count\t\tvalue\t (default 50) results per page

Resources:
\tstatus
\tcontext 
\thost
\thost-info (require hostkey) 
\thost-seen (require hostkey)
\thost-seen-by (require hostkey)
\tpromise-log-repaired-summary
\tpromise-log-repaired
\tpromise-log-notkept-summary
\tpromise-log-notkept
\tsetuid
\tsoftware
\tvariable
\tfile
"

while true; do
  case "$1" in
    -h | --help ) 
        echo -e "$HELP"; 
        exit 0 ;;
    --curl ) 
        CURL="$2"; 
        shift 2 ;;
    -r | --resource ) 
        RESOURCE="$2"; 
        shift 2 ;;
    --port ) 
        PORT="$2"; 
        shift 2 ;;
    --user ) 
        USER="$2"; 
        shift 2 ;;
    --pass ) 
        PASS="$2"; 
        shift 2 ;;
    --url ) 
        URL="$2"; 
        shift 2 ;;
    --hostkey ) 
        HOSTKEY="$2"; 
        shift 2 ;;
    -v | --verbose ) 
        VERBOSE=true; 
        shift ;;
    --page ) 
        if [[ $2 != *[!0-9]* ]]; then
            PAGE="$2"; 
        else
            echo "error: page value must be a number";
            exit 1;
        fi
        shift 2 ;;
    --count ) 
        if [[ $2 != *[!0-9]* ]]; then
            COUNT="$2"; 
        else
            echo "error: count value must be a number";
            exit 1;
        fi
        shift 2 ;;

    -- ) 
        shift; 
        break ;;
    * ) 
        break ;;
  esac
done

if [ -z $RESOURCE ]; then 
    echo "error: missing parameter (required): --resource status/contexts/hosts/promises/setuid/software/variables/filechanges"; 
    exit 1; 
fi

RESLIST=(
    "status" 
    "context" 
    "host" 
    "setuid" 
    "software" 
    "promise-compliance" 
    "promise-log-repaired-summary" 
    "promise-log-repaired" 
    "promise-log-notkept-summary"
    "promise-log-notkept"
    "variable"
    "file"
);

RESLIST_HK=(
    "host-info" 
    "host-seen" 
    "host-seen-by"
);

for i in "${RESLIST[@]}"
do
    if [ "$i" == $RESOURCE ]; then
        ISR=true;
    fi
done

# need hostkey param
for i in "${RESLIST_HK[@]}"
do
    if [ "$i" == $RESOURCE ]; then
        ISR=true;
        HK=true;
    fi
done

if [ ! $ISR ]; then 
    echo "error: invalid resource parameter"; 
    exit 1; 
fi

if [ $HK ]; then 
    if [ -z $HOSTKEY ]; then 
        echo "error: missing parameter (required): --hostkey"; 
        exit 1; 
    fi
fi

case "$RESOURCE" in
    host-info )
        RESOURCE="host/$HOSTKEY/";;
    host-seen )
        RESOURCE="host/$HOSTKEY/seen";;
    host-seen-by )
        RESOURCE="host/$HOSTKEY/seen-by";;
    promise-compliance )
        RESOURCE="promise/compliance";;
    promise-log-repaired )
        RESOURCE="promise/log/repaired";;
    promise-log-notkept )
        RESOURCE="promise/log/notkept";;
    promise-log-repaired-summary )
        RESOURCE="promise/log/repaired/summary";;
    promise-log-notkept-summary )
        RESOURCE="promise/log/notkept/summary";;

esac

if [ $RESOURCE = "status" ]; then 
    RESOURCE=""; 
fi

URL=$(echo $URL | sed 's~http[s]*://~~g')

CALL="curl --user $USER:$PASS -P $PORT  http://$URL/rest/$RESOURCE/?count=$COUNT&page=$PAGE";

if [ $VERBOSE ]; then
    echo "server address = $URL";
    echo "resource = $RESOURCE";
    echo "port = $PORT";
    echo "user = $USER";
    echo "password = $PASS";
    echo "page = $PAGE"
    echo "results per page = $COUNT";
    echo -e "\n";
    echo $CALL;
fi

if [ ! -z $VERBOSE ]; then 
    $CALL; 
    echo -e "\n";
else 
    $CALL &> /dev/null; 
fi

if [ $? = 0 ]; then 
    echo "<status>success</status>"; 
    exit 0; 
else 
    echo "<status>fail</status>"; 
    exit 1; 
fi
