#!/bin/bash
FUNC_SUCCESS=0
FUNC_FAIL=0
COUNTER=0
DIFF=""

declare -a flags=(
"-i"
"-v"
"-c"
"-l"
"-n"
"-h"
"-s"
"-f"
"-o"
)

declare -a tests=(
"print s21_grep.c s21_grep.h Makefile"
"print s21_grep.c noFile"
"while s21_grep.c s21_grep.h Makefile"
"-e ^int -e void s21_grep.c noFile"
"-e ^int -e void s21_grep.c s21_grep.h Makefile"
"= s21_grep.c s21_grep.h Makefile"
"= s21_grep.c noFile"
"-f 1.txt s21_grep.h Makefile"
"-f 1.txt s21_grep.c noFile"
)

declare -a commline=(
"TEST FLAGS"
)

function testing()
{
    str=$(echo "$@" | sed "s/TEST/$i/")
    str=$(echo "$str" | sed "s/FLAGS/$var/")
    (( COUNTER++ ))
    echo "TEST №$COUNTER" >> final_grep_testing.log
    printf "\ns21_grep output:\n" >> final_grep_testing.log
    ./s21_grep $str >> s21_grep_testing.log
    cat s21_grep_testing.log >> final_grep_testing.log
    printf "\ngrep output:\n" >> final_grep_testing.log
    grep $str >> system_grep_testing.log
    cat system_grep_testing.log >> final_grep_testing.log
    printf "\n" >> final_grep_testing.log
    DIFF="$(diff -s s21_grep_testing.log system_grep_testing.log)"
    if [ "$DIFF" == "Files s21_grep_testing.log and system_grep_testing.log are identical" ]
    then
        (( FUNC_SUCCESS++ ))
        echo "grep $str FUNCTIONALITY SUCCESS $COUNTER"
        printf "\n"
        echo "TEST №$COUNTER RESULT: grep $str FUNCTIONALITY SUCCESS $COUNTER" >> final_grep_testing.log
        echo $'\n' >> final_grep_testing.log
    else
        (( FUNC_FAIL++ ))
        echo "grep $str FUNCTIONALITY FAIL $COUNTER"
        printf "\n"
        echo "TEST №$COUNTER RESULT: grep $str FUNCTIONALITY FAIL $COUNTER" >> final_grep_testing.log
        echo $'\n' >> final_grep_testing.log
    fi
    rm s21_grep_testing.log system_grep_testing.log
}


for i in "${tests[@]}"
do
    var=""
    testing "$commline"
done

for item in "${flags[@]}"
do
    for i in "${tests[@]}"
    do
        var="$item"
        testing "$commline"
    done
done

for item in "${flags[@]}"
do
    for item1 in "${flags[@]}"
    do
        if [ $item != $item1 ]
        then
            for i in "${tests[@]}"
            do
                var="$item $item1"
                testing "$commline"
            done
        fi
    done
done

echo "RESULTS:"
echo "RESULTS:" >> final_grep_testing.log
echo "FAIL: $FUNC_FAIL"
echo "FAIL: $FUNC_FAIL" >> final_grep_testing.log
echo "SUCCESS: $FUNC_SUCCESS"
echo "SUCCESS: $FUNC_SUCCESS" >> final_grep_testing.log
echo "ALL: $COUNTER"
echo "ALL: $COUNTER" >> final_grep_testing.log