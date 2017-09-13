#!/bin/sh

extractAndRun()
{
    echo "Extracting $1 to $2..."

    case $1 in
        *.zip) unzip $1 -d $2 ;;
        *) echo "'$1' cannot be extracted" ;;
    esac

    sviewgl -r $2
}

if [ -f $1 ] ; then
    tempfoo=`basename $0`
    TMPFILE=`mktemp -d -t ${tempfoo}`
    if [ $? -ne 0 ]; then
        echo "$0: Can't create temp file, exiting..."
        exit 1
    fi

    extractAndRun "$1" "${TMPFILE}"

    rm -fr "${TMPFILE}"
else
    echo "'$1' is not a valid file"
    exit 1
fi
