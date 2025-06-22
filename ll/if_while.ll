
define dso_local i32 @main() #0 {
entry:
 %0 = alloca i32
 %1 = alloca i32
 %2 = alloca i32
 %3 = alloca i32
	store i32 1, i32* %0
	store i32 2, i32* %1
	%4 = load i32, i32* %1
	%5 = load i32, i32* %0
	%6 = mul i32 %5, 2
	%7 = add i32 %6, 4
	%8 = icmp slt i32 %4, %7
	br i1 %8, label %if_then1, label %if_else2
if_then1:
	store i32 0, i32* %2
	br label %while_cond4
while_cond4:
	%9 = load i32, i32* %2
	%10 = icmp slt i32 %9, 10
	br i1 %10, label %while_body5, label %while_after6
while_body5:
	%11 = load i32, i32* %3
	%12 = load i32, i32* %2
	%13 = mul i32 %12, 2
	%14 = add i32 %11, %13
	store i32 %14, i32* %3
	%15 = load i32, i32* %2
	%16 = add i32 %15, 1
	store i32 %16, i32* %2
	br label %while_cond4
while_after6:
	br label %if_merge3
if_else2:
	store i32 100, i32* %3
	br label %if_merge3
if_merge3:
	%17 = load i32, i32* %3
	ret i32 %17
}
