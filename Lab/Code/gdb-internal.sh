file ./parser
set print pretty on
# b semantic.c:535
b statement_list
r ../Test/lab2/tmp.c
