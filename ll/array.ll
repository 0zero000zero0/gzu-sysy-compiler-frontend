
define dso_local i32 @main() #0 {
entry:
 %0 = alloca [10 x [2 x i32]]
	store [10 x [2 x i32]] zeroinitializer, [10 x [2 x i32]]* %0
 %1 = alloca i32
 %2 = alloca i32
 %3 = alloca i32
	store i32 0, i32* %2
	store i32 0, i32* %1
	br label %while_cond1
while_cond1:
	%4 = load i32, i32* %2
	%5 = icmp slt i32 %4, 10
	br i1 %5, label %while_body2, label %while_after3
while_body2:
	store i32 0, i32* %3
	br label %while_cond4
while_cond4:
	%6 = load i32, i32* %3
	%7 = icmp slt i32 %6, 2
	br i1 %7, label %while_body5, label %while_after6
while_body5:
	%8 = load i32, i32* %1
	%9 = load i32, i32* %2
 %10 = getelementptr i32, i32* %0, i32 %9
	%11 = load i32, i32* %3
 %12 = getelementptr i32, i32* %10, i32 %11
	%13 = load i32, i32* %12
	%14 = add i32 %8, %13
	store i32 %14, i32* %1
	%15 = load i32, i32* %3
	%16 = add i32 %15, 1
	store i32 %16, i32* %3
	br label %while_cond4
while_after6:
	%17 = load i32, i32* %2
	%18 = add i32 %17, 2
	store i32 %18, i32* %2
	br label %while_cond1
while_after3:
	%19 = load i32, i32* %1
	ret i32 %19
}
