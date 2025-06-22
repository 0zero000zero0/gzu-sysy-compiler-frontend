
define dso_local i32 @main() #0 {
entry:
 %0 = alloca i32
 %1 = alloca i32
	store i32 100, i32* %0
	store i32 0, i32* %1
	br label %while_cond1
while_cond1:
	%2 = load i32, i32* %0
	%3 = icmp sgt i32 %2, 10
	br i1 %3, label %while_body2, label %while_after3
while_body2:
	%4 = load i32, i32* %1
	%5 = load i32, i32* %0
	%6 = add i32 %4, %5
	store i32 %6, i32* %1
	%7 = load i32, i32* %0
	%8 = sub i32 %7, 1
	store i32 %8, i32* %0
	br label %while_cond1
while_after3:
	%9 = load i32, i32* %1
	ret i32 %9
}
