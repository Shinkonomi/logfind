echo "Running unit tests:"

for i in test/*_tests
do
    if test -f $i
    then
        if $VALGRIND ./$i 2>> test/tests.log
        then
            echo [$i] "\033[0;32m PASS"
        else
            echo "\033[0;31m ERROR in test [$i]: \033[0m here's test/tests.log"
            echo "----------------------"
            tail test/tests.log
            exit 1
        fi
    fi
done

echo ""
