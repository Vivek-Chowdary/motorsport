#!/bin/bash
tmp="/tmp/tmpIndent$(basename $1)"
tmpa="/tmp/tmpIndentB$(basename $1)"

head -n 1 $1 > $2
max=$(wc -l $1|sed "s/\ .*//g")
let max=$max-1
tail -n $max $1 > $tmpa

cat $tmpa | sed "s/>/>@/g" | tr "@" "\n" |tr "\n" "@" |sed "s/\">/\"@>/g"| sed "s/\"\/>/\"@\/>/g" |sed "s/\(<[^\ ]*\ \)/\1@/g" | sed "s/\ \([^\"]*\"[^\"]*\"\)/@\1/g" | sed "s/\([\ ]*\)\([^<]*[^@]*\)@/\1\2@\1/g" | sed "s/@[\ ]*@/@/g" | tr "@" "\n" > $tmp

#  | sed "s/[\ ]*\([^\ ]\)/\1/g" 

IFS=$'\t\n'

line="0"
ntab="0"
closed="0"
reindent="0"
max=$(wc -l $tmp|sed "s/\ .*//g")
echo -n "[$max]"
for l in $(cat $tmp)
do
    #echo "[$line / $max]"
    echo -n "."
    let line=$line+1
    if [ "$closed" = "1" ]
    then
        # </ -- >
        if [ "W$(echo $l |sed "s/<\/.*>/WEIO/g")O" = "WWEIOO" ]; then
            let ntab=$ntab-4
            reindent="1"
        fi
        # < -- /> 
        if [ "W$(echo $l |sed "s/<.*\/>/WEIO/g")O" = "WWEIOO" ]; then
            let ntab=$ntab-4
            reindent="1"
        fi
    fi
    closed="0"
    # />
    if [ "W$(echo $l |sed "s/\/>/WEIO/g")O" = "WWEIOO" ]; then
        let ntab=$ntab-4
        closed="1"
    fi
    # >
    if [ "W$(echo $l |sed "s/>/WEIO/g")O" = "WWEIOO" ]; then
        closed="1"
    fi
    # text
    perl -e "print ' 'x$ntab" >> $2
    echo "$l" >> $2
    if [ "$reindent" = "1" ]; then
        let ntab=$ntab+4
        reindent="0"
    fi
    # < -- /> 
    if [ "W$(echo $l |sed "s/<.*\/>/WEIO/g")O" = "WWEIOO" ]; then
        let ntab=$ntab-4
    else
    # </ -- >
    if [ "W$(echo $l |sed "s/<\/.*>/WEIO/g")O" = "WWEIOO" ]; then
        let ntab=$ntab-4
    else
    # < --
    if [ "W$(echo $l |sed "s/<[^>]*/WEIO/g")O" = "WWEIOO" ]; then
        let ntab=$ntab+4
    else
    # < -- >
    if [ "W$(echo $l |sed "s/<[^>]*>/WEIO/g")O" = "WWEIOO" ]; then
        let ntab=$ntab+4
    fi
    fi
    fi
    fi
done

echo "[done!]"
