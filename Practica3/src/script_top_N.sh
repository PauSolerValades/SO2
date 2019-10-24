#!/bin/bash

if [ $# -ne 2 ]
then
	echo "Argument expected (Fitxer de fitxers, top n elements)"
	exit 1
fi

echo "Executant Main... Un moment si us plau."

./main "$1" | sort -nr | head -n $2 > top.txt

echo "Fet! Comprova el fitxer top.txt que ha aparegut al directori!"

exit 0
