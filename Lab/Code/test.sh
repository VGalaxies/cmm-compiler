TEST=$(find $1 -regex ".*\.\(c\|cmm\)" | sort)
TEST_ARR=(${TEST})
IR_PATH="/home/vgalaxy/Desktop/shared/demo.ir"

make clean
make parser

for i in "${!TEST_ARR[@]}"
do
  echo -e "\e[1;35m./parser ${TEST_ARR[i]}\e[0m"
  bat ${TEST_ARR[i]}
  ./parser ${TEST_ARR[i]} ${IR_PATH}
  echo -e "\n"
done
