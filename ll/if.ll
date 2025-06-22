
define dso_local i32 @main() #0 {
entry:
 %0 = alloca i32
 %1 = alloca i32
 %2 = alloca i32
 %3 = alloca i32
	store i32 3, i32* %0
	store i32 5, i32* %1
	store i32 7, i32* %2
	%4 = load i32, i32* %0
	%5 = load i32, i32* %1
	%6 = icmp sgt i32 %4, %5
	br i1 %6, label %if_then1, label %if_merge3
if_then1:
	%7 = load i32, i32* %0
	store i32 %7, i32* %3
	%8 = load i32, i32* %2
	%9 = load i32, i32* %3
	%10 = icmp sgt i32 %8, %9
	br i1 %10, label %if_then4, label %if_merge6
if_then4:
	%11 = load i32, i32* %2
	store i32 %11, i32* %3
	br label %if_merge6
if_merge6:
	br label %if_merge3
if_merge3:
	%12 = load i32, i32* %0
	%13 = load i32, i32* %1
	%14 = icmp sle i32 %12, %13
	br i1 %14, label %if_then7, label %if_merge9
if_then7:
	%15 = load i32, i32* %1
	store i32 %15, i32* %3
	%16 = load i32, i32* %2
	%17 = load i32, i32* %3
	%18 = icmp sgt i32 %16, %17
	br i1 %18, label %if_then10, label %if_merge12
if_then10:
	%19 = load i32, i32* %2
	store i32 %19, i32* %3
	br label %if_merge12
if_merge12:
	br label %if_merge9
if_merge9:
	%20 = load i32, i32* %3
	ret i32 %20
}
