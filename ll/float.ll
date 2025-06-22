
define dso_local i32 @main() #0 {
entry:
 %0 = alloca float
 %1 = alloca float
	store float 2.000000, float* %0
	%2 = load i32, i32* %0
	%3 = mul i32 %2, 2
	%4 = sub i32 %3, 3
	store float %4, float* %1
	%5 = load i32, i32* %0
	%6 = load i32, i32* %1
	%7 = icmp sgt i32 %5, %6
	%8 = load i32, i32* %1
	%9 = icmp sgt i32 %8, 0
	%10 = icmp  i32 %7, %9
	br i1 %10, label %if_then1, label %if_merge3
if_then1:
	ret i32 10
if_merge3:
	%11 = sub i32 0, 1
	ret i32 %11
}
