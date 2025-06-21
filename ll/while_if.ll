
define dso_local i32 @main() #0 {
entry:
  %1 = alloca i32
  %2 = alloca i32
  %3 = alloca i32
  store i32 0, i32* %1
  store i32 1, i32* %2
  br label %while_cond1
while_cond1:
  %4 = load i32, i32* %2
  %5 = icmp i32 %4, 9527
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %while_body2, label %while_after3
while_body2:
  %7 = load i32, i32* %2
  store i32 %7, i32* %3
  br label %while_cond4
while_cond4:
  %8 = load i32, i32* %3
  %9 = icmp i32 %8, 0
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %while_body5, label %while_after6
while_body5:
  %11 = load i32, i32* %3
  %12 = sub i32 %11, 2
  store i32 %12, i32* %3
  br label %while_cond4
while_after6:
  %13 = load i32, i32* %3
  %14 = icmp i32 %13, 0
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %if_then7, label %if_merge9
if_then7:
  %16 = load i32, i32* %1
  %17 = add i32 %16, 1
  store i32 %17, i32* %1
  br label %if_merge9
if_merge9:
  %18 = load i32, i32* %2
  %19 = add i32 %18, 1
  store i32 %19, i32* %2
  br label %while_cond1
while_after3:
  %20 = load i32, i32* %1
  ret i32 %20
}
