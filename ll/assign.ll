
define dso_local i32 @main() #0 {
entry:
 %0 = alloca i32
 %1 = alloca i32
 %2 = alloca i32
 %3 = alloca i32
 %4 = alloca i32
	store i32 10, i32* %0
	store i32 20, i32* %2
	store i32 5, i32* %1
	store i32 15, i32* %3
	%5 = load i32, i32* %0
	%6 = load i32, i32* %1
	%7 = sub i32 %5, %6
	%8 = load i32, i32* %0
	%9 = load i32, i32* %1
	%10 = sub i32 %8, %9
	%11 = mul i32 %7, %10
	%12 = load i32, i32* %2
	%13 = load i32, i32* %3
	%14 = sub i32 %12, %13
	%15 = load i32, i32* %2
	%16 = load i32, i32* %3
	%17 = sub i32 %15, %16
	%18 = mul i32 %14, %17
	%19 = add i32 %11, %18
	store i32 %19, i32* %4
	%20 = load i32, i32* %4
	ret i32 %20
}
