# Emulation of single-precision floating-point addition
This code tries to emulate IEEE-754 single-precision floating-point addition by representing each
bit in the numbers as integer arrays. I wrote this code to improve my understanding of how floating point
numbers are represented and how hardware operates on them. The code currently fails on some edge
cases, and it doesn't handle negative numbers, inf, nan and overflow cases. 
