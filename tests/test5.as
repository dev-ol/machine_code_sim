       lw 0 1 x
       lw 0 2 y
start  beq 1 2 done 
       lw 0 1 y
       beq 1 2 start
done   halt
x     .fill 10
y     .fill 3