
define dso_local i32 @main() #0 {
entry:
 %0 = alloca i32
 %1 = alloca i32
	store i32 0, i32* %0
	store i32 0, i32* %1
	br label %while_cond1
while_cond1:
	%2 = load i32, i32* %0
	%3 = icmp slt i32 %2, 10
	br i1 %3, label %while_body2, label %while_after3
while_body2:
	%4 = load i32, i32* %1
	%5 = icmp sgt i32 %4, 10
	br i1 %5, label %if_then4, label %if_merge6
if_then4:
	br label %if_merge6
if_merge6:
	%6 = load i32, i32* %1
	%7 = load i32, i32* %0
	%8 = add i32 %6, %7
	store i32 %8, i32* %1
	%9 = load i32, i32* %0
	%10 = add i32 %9, 1
	store i32 %10, i32* %0
	br label %while_cond1
while_after3:
	%11 = load i32, i32* %1
	ret i32 %11
}
