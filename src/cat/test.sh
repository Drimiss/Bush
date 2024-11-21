rm -rf test
mkdir -p test
cp s21_cat test/
gcc -Wall -Wextra -Werror filetest.c -o test/test_gen
cd test/

test_number=0
test_count=64
failed_count=0
files_list="rand1 rand2 rand3 empty lines"
options=(-s -e -n -v -b -t)

while [ $test_count != 0 ]; do
    test_count=$((test_count -1))
		./test_gen $test_number $files_list
    for opt in ${options[@]}
    do
        echo -e "TEST NUMBER $test_number:"
        ./s21_cat $opt $files_list > s21_res
        cat $opt $files_list > cat_res
        echo -e "$(diff -s cat_res s21_res)\n"
        if [[ $(diff -q cat_res s21_res) ]]
        then
            echo "____________________________________________________________"
            echo -e "\nFAIL: test failed at $opt\n"
            failed_count=$((failed_count + 1))
        fi
        test_number=$((test_number + 1))
    done
done