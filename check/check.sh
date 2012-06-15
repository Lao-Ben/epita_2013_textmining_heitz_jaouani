#!/bin/sh

CHEMINDICO=misc/words.txt
NBPICK=100

launch ()
{
    DISTANCE=${1}

    echo "\033[1;33mTest ${NBPICK} words, distance ${DISTANCE}\033[37m"

    rm tmp

if [ $DISTANCE = 'mixed' ]; then
    for WORD in `cat words`; do
	RAND=`tr -cd 0-9 </dev/urandom | head -c 1`
	echo "approx "$(($RAND % 3))" ${WORD}" >> tmp
    done
else
    for WORD in `cat words`; do
	echo "approx ${DISTANCE} ${WORD}" >> tmp
    done
fi

# REF
    TIMEBASE=`date +%s%N | cut -b1-13 | tr --delete "\n"`
    echo "" | ../ref/linux32/TextMiningApp ../dicoref.bin 2>/dev/null > /dev/null
    TIMEVOID=$((`date +%s%N | cut -b1-13 | tr --delete "\n"` - TIMEBASE))

    TIMEBASE=`date +%s%N | cut -b1-13 | tr --delete "\n"`
    cat tmp | ../ref/linux32/TextMiningApp ../dicoref.bin > ./resultref 2> /dev/null
    TIME=$((`date +%s%N | cut -b1-13 | tr --delete "\n"` - TIMEBASE))

    REALTIME=$((${TIME} - ${TIMEVOID}))

    if [ ${REALTIME} -lt 2 ]; then
	REALTIME=2
    fi;


    echo "  Ref total time    : ${TIME} msec"
    echo "  Ref browsing time : ${REALTIME} msec"
    echo -n "  Ref speed rate :  (~"
    echo "scale=0
    ${NBPICK} * 1000 / ${REALTIME}" | bc -l  | tr --delete "\n"
    echo " words / sec)"

echo =====================

# APP
    TIMEBASE=`date +%s%N | cut -b1-13 | tr --delete "\n"`
    echo "" | ../TextMiningApp ../dico.bin 2>/dev/null > /dev/null
    TIMEVOID=$((`date +%s%N | cut -b1-13 | tr --delete "\n"` - TIMEBASE))

    TIMEBASE=`date +%s%N | cut -b1-13 | tr --delete "\n"`
    cat tmp | ../TextMiningApp ../dico.bin > ./result 2> /dev/null
    TIME=$((`date +%s%N | cut -b1-13 | tr --delete "\n"` - TIMEBASE))

    REALTIME=$((${TIME} - ${TIMEVOID}))

    echo "  App total time    : ${TIME} msec"
    echo "  App browsing time : ${REALTIME} msec"
    echo -n "  App speed rate :  (~"
    echo "scale=0
    ${NBPICK} * 1000 / ${REALTIME}" | bc -l  | tr --delete "\n"
    echo " words / sec)"




    diff -q ./result ./resultref
    if [ ${?} = 0 ]; then
	echo "\033[1;32mDiff Success\033[37m"
    else
	echo -n "\033[1;31mDiff Fail: "
	diff result resultref >> difflog
	echo "\n\n\n===============================================\n\n\n" >> difflog
	echo -n $((`grep "\[" difflog | wc -l` / 2))
	echo " error(s) / ${NBPICK} tests (see ./check/difflog)\033[37m"
    fi;

    echo
}

echo "\033[1;36m********* Moulinette Textmining *********\033[37m"

REALCHEMINDICO=../${CHEMINDICO}

if [ ! -f $REALCHEMINDICO ]; then
    echo "\n\033[1;31mErreur: Dictionnaire introuvable\033[37m"
    echo "\033[1;31m        Chemin recherche : ${CHEMINDICO}\033[37m"
    echo "\033[1;31m        Vous pouvez specifier un autre chemin dans check/check.sh\033[37m\n"
    exit 1
fi;


if [ ${#} = 1 ]; then
    NBPICK=${1}
fi;

if [ ! -f ../dico.bin ]; then
    echo -n "Dictionnary does not exist. I'm creating it..."
    ../TextMiningCompiler $REALCHEMINDICO ../dico.bin 2>&1  > /dev/null
    echo "Done."
fi

if [ ! -f ../dicoref.bin ]; then
    echo -n "Reference dictionnary does not exist. I'm creating it..."
    ../ref/linux32/TextMiningCompiler $REALCHEMINDICO ../dicoref.bin 2>&1  > /dev/null
    echo "Done."
fi


echo -n "\nRandomly picking ${NBPICK} words from dictionnary..."
shuf -n ${NBPICK} $REALCHEMINDICO > tmp
cut -f 1 tmp > ./words
echo "Done.\n\n"


rm -f ./difflog

launch 0

launch 1

launch 2

launch mixed


#rm -f ./words ./tmp ./result ./resultref ./time ./timeref
