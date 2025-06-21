
define dso_local i32 @main() #0 {
entry:
 %1 = alloca i32
 %2 = alloca i32
 %3 = alloca i32
 %4 = alloca i32
  store i32 3, i32* %1
  store i32 5, i32* %2
  store i32 7, i32* %3
  %5 = load i32, i32* %1
  %6 = load i32, i32* %2
  %7 = icmp sgt i32 %5, %6
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %if_then1, label %if_merge3
if_then1:
  %9 = load i32, i32* %1
  store i32 %9, i32* %4
  %10 = load i32, i32* %3
  %11 = load i32, i32* %4
  %12 = icmp sgt i32 %10, %11
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %if_then4, label %if_merge6
if_then4:
  %14 = load i32, i32* %3
  store i32 %14, i32* %4
  br label %if_merge6
if_merge6:
  br label %if_merge3
if_merge3:
  %15 = load i32, i32* %1
  %16 = load i32, i32* %2
  %17 = icmp sle i32 %15, %16
  %18 = icmp ne i32 %17, 0
  br i1 %18, label %if_then7, label %if_merge9
if_then7:
  %19 = load i32, i32* %2
  store i32 %19, i32* %4
  %20 = load i32, i32* %3
  %21 = load i32, i32* %4
  %22 = icmp sgt i32 %20, %21
  %23 = icmp ne i32 %22, 0
  br i1 %23, label %if_then10, label %if_merge12
if_then10:
  %24 = load i32, i32* %3
  store i32 %24, i32* %4
  br label %if_merge12
if_merge12:
  br label %if_merge9
if_merge9:
  %25 = load i32, i32* %4
  ret i32 %25
}
