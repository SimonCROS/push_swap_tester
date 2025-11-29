> [!NOTE]
> [November 29, 2025]
> 
> A new beta version is available on https://github.com/SimonCROS/push_swap_tester/tree/v2, with performances up to **10x faster** 🚀 !

# complexity

## Build

Compile with the following command

`make`

## Usage

> [!WARNING]  
> This version is a beta version. Checker is not available.\
> If you find a bug, please report it to this pull request: https://github.com/SimonCROS/push_swap_tester/pull/6.

**./complexity** \[**-vh**\] \[**-s** <ins>seed</ins>\] \[**-o** <ins>output</ins>\] \[**-f** <ins>push_swap</ins>\] \[**-t** <ins>threads</ins>\] <ins>numbers</ins> <ins>iterations</ins> \[<ins>goal</ins>\] \[<ins>checker</ins>\]

Following options are available :

**-v**, **--version**\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Show version

**-h**, **--help**\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Show help

**--json**, **--no-json**\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Toggle JSON output on or off. By default, JSON output is enabled for non-TTY environments

**-s** <ins>seed</ins>, **--seed**=<ins>seed</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Generates the numbers based on the seed

**-o** <ins>output</ins>, **--output**=<ins>output</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Specifies an output file for logs (errors, timeouts, excessive instructions)

**--output-buffer**=<ins>buffer-size</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Used along with **--output**, sets the maximum size of the output file (default: 10 MiB)

**-f** <ins>push_swap</ins>, **--file**=<ins>push_swap</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Use the specified <ins>push_swap</ins> executable

**-t** <ins>threads</ins>, **--threads**=<ins>threads</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Specifies the number of threads to use (default: 6)

**-k** <ins>timeout</ins>, **--timeout**=<ins>timeout</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Timeout for a single push_swap execution in milliseconds (default: 500)

### Demo :

![Demo Animation for 100 elements](../assets/v1.6.0-100-100-checker.gif)
![Demo Animation for 500 elements](../assets/v1.6.0-500-100.gif)

---

Merci à [@mhouppin](https://github.com/mhouppin) pour l'inspiration visuelle du testeur.\
Thanks to [@42cursus-youkim](https://github.com/42cursus-youkim) for the translations.
