TEST_SRC=$(find $1 -regex ".*\.\(c\|cmm\)" | sort)
TEST_LIST=(${TEST_SRC})

IR_PATH="/home/vgalaxy/Desktop/shared/demo.ir"
OUT_PATH="/home/vgalaxy/Desktop/shared/out.s"

make clean
make parser

for i in "${!TEST_LIST[@]}"
do
  echo -e "\e[1;35m./parser ${TEST_LIST[i]}\e[0m"
  bat ${TEST_LIST[i]}
  ./parser ${TEST_LIST[i]} ${OUT_PATH}
  echo -e "\n"
done
