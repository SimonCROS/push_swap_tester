# complexity

## Build

Compile with the following command

`make`

## Usage

> [!WARNING]  
> This version is a beta version. Checker and `-o` are currently not available.\
> If you find a bug, please report it to this pull request: https://github.com/SimonCROS/push_swap_tester/pull/6.

**./complexity** \[**-vh**\] \[**-s** <ins>seed</ins>\] \[**-f** <ins>push_swap</ins>\] \[**-t** <ins>threads</ins>\] <ins>numbers</ins> <ins>iterations</ins> \[<ins>goal</ins>\] \[<ins>checker</ins>\]

Following options are available :

**-v**, **--version**\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Show version of tester

**-h**, **--help**\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Show help

**-s** <ins>seed</ins>, **--seed**=<ins>seed</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Generates the numbers based on the seed

**-o** <ins>output</ins>, **--output**=<ins>output</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Specifies an output file for logs

**-f** <ins>push_swap</ins>, **--file**=<ins>push_swap</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Use <ins>push_swap</ins> for push_swap executable

**-t** <ins>threads</ins>, **--threads**=<ins>threads</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Specifies the amount of threads to use (default: 6)

### Demo :

![Demo Animation for 100 elements](../assets/v1.6.0-100-100-checker.gif)
![Demo Animation for 500 elements](../assets/v1.6.0-500-100.gif)

---

Merci à [@mhouppin](https://github.com/mhouppin) pour l'inspiration visuelle du testeur.\
Thanks to [@42cursus-youkim](https://github.com/42cursus-youkim) for the translations.
