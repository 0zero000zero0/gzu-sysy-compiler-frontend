
define dso_local i32 @main() #0 {
entry:
 %1 = alloca i32
 %2 = alloca i32
  store i32 0, i32* %1
  store i32 0, i32* %2
  br label %while_cond1
while_cond1:
  %3 = load i32, i32* %1
  %4 = icmp slt i32 %3, 10
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %while_body2, label %while_after3
while_body2:
  %6 = load i32, i32* %2
  %7 = icmp sgt i32 %6, 10
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %if_then4, label %if_merge6
if_then4:
  br label %if_merge6
if_merge6:
  %9 = load i32, i32* %2
  %10 = load i32, i32* %1
  %11 = add i32 %9, %10
  store i32 %11, i32* %2
  %12 = load i32, i32* %1
  %13 = add i32 %12, 1
  store i32 %13, i32* %1
  br label %while_cond1
while_after3:
  %14 = load i32, i32* %2
  ret i32 %14
}
