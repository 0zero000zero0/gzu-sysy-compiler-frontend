
define dso_local i32 @main() #0 {
entry:
 %0 = alloca i32
 %1 = alloca i32
	store i32 1, i32* %0
	store i32 2, i32* %1
	%2 = load i32, i32* %0
	%3 = load i32, i32* %1
	%4 = add i32 %2, %3
	ret i32 %4
}
