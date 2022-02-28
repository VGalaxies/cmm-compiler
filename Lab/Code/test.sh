TEST=$(find ../Test -name "*.c")
TESTARR=(${TEST})

make clean
make parser

for i in "${!TESTARR[@]}"
do
  echo -e "\e[1;35m./parser ${TESTARR[i]}\e[0m"
  bat ${TESTARR[i]}
  ./parser ${TESTARR[i]}
  echo -e "\n"
done
