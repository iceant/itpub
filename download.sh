#!/bin/sh
if [ x$1!=x ]
then
./itpub_forum "http://www.itpub.net/forum.php?mod=forumdisplay&fid=$1&filter=author&orderby=dateline"
else
./itpub_forum "http://www.itpub.net/forum.php?mod=forumdisplay&fid=61&filter=author&orderby=dateline"
fi
