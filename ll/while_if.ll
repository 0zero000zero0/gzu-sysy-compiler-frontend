
define dso_local i32 @main() #0 {
entry:
 %0 = alloca i32
 %1 = alloca i32
 %2 = alloca i32
	store i32 0, i32* %0
	store i32 1, i32* %1
	br label %while_cond1
while_cond1:
	%3 = load i32, i32* %1
	%4 = icmp slt i32 %3, 9527
	br i1 %4, label %while_body2, label %while_after3
while_body2:
	%5 = load i32, i32* %1
	store i32 %5, i32* %2
	br label %while_cond4
while_cond4:
	%6 = load i32, i32* %2
	%7 = icmp sgt i32 %6, 0
	br i1 %7, label %while_body5, label %while_after6
while_body5:
	%8 = load i32, i32* %2
	%9 = sub i32 %8, 2
	store i32 %9, i32* %2
	br label %while_cond4
while_after6:
	%10 = load i32, i32* %2
	%11 = icmp eq i32 %10, 0
	br i1 %11, label %if_then7, label %if_merge9
if_then7:
	%12 = load i32, i32* %0
	%13 = add i32 %12, 1
	store i32 %13, i32* %0
	br label %if_merge9
if_merge9:
	%14 = load i32, i32* %1
	%15 = add i32 %14, 1
	store i32 %15, i32* %1
	br label %while_cond1
while_after3:
	%16 = load i32, i32* %0
	ret i32 %16
}
