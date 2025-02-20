# Zilog

## Zilog: A Simple C type language and interpritated

Zilog is a straightforward interpreted language designed for ease of use and readability. It provides basic data types and commands to allow users to write simple programs.

## Supported Data Types

| Type           | Description                                                                             |
|----------------|-----------------------------------------------------------------------------------------|
| `number`       | Defines an integer variable. Range: `-2,147,483,648 - 2,147,483,647`                      |
| `swim`         | Defines a floating-point (double) variable. Precision up to 18 decimal places           |
| `raw`          | Defines a string variable `"String"`                                                    |
| `con`          | Defines a boolean variable. Accepts `true`, `false`, `1`, or `0`.   |

The variable is defined by the C type. Base example: `number `name` = 1`

## Supported auxiliary commands

| Type           | Description                                                                             |
|----------------|-----------------------------------------------------------------------------------------|
| `display`      | Output variable or data                                                                 |
| `end`          | End line command                                                                        |

Simple command example: `display "hi" end` (or) `display name end` (or) `display 1 end` and more other cases
