
define dso_local i32 @square(i32, i32) #0 {
entry:
	%2 = alloca i32
	store i32 %0, i32* %2
	%3 = alloca i32
	store i32 %1, i32* %3
	%4 = load i32, i32* %2
	%5 = load i32, i32* %2
	%6 = mul i32 %4, %5
	%7 = load i32, i32* %3
	%8 = load i32, i32* %3
	%9 = mul i32 %7, %8
	%10 = add i32 %6, %9
	ret i32 %10
}

define dso_local i32 @main() #0 {
entry:
 %0 = alloca i32
 %1 = alloca i32
	store i32 3, i32* %0
	store i32 4, i32* %1
	%2 = load i32, i32* %0
	%3 = load i32, i32* %1
	%4 = call i32 @square(i32 %2, i32 %3)
	ret i32 %4
}
