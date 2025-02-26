raw operand = "" // add new variable
raw plus = "+"
raw minus = "-"
number i = 0
number j = 0
number result = 0

in operand // input
in i
in j

if operand == plus // conditional operator if
result = opr i + j end // operator + with opr
display "Result of operand " plus " : " result end
break

if operand == minus
result = opr i - j end
display "Result of operand " minus " : " result end
break