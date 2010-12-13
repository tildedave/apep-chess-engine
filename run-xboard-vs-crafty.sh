#!/bin/sh
scriptDir=$PWD/`dirname $0`
craftyLocation=`which crafty`
echo $scriptDir
echo $craftyLocation

xboard -fcp $scriptDir/bin/apep -scp $craftyLocation

