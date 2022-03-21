file ./parser
set print pretty on
b semantic.c:535
# b print_structure_type
# b variable_declaration
r ../Test/lab2/tmp.c
