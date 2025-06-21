
define dso_local i32 @main() #0 {
entry:
  %1 = alloca i32
  %2 = alloca i32
  store i32 0, i32* (null)
  store i32 0, i32* (null)
  br label %while_cond1
while_cond1:
  %3 = load i32, i32* (null)
  %4 = icmp i32 %3, 10
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %while_body2, label %while_after3
while_body2:
  %6 = load i32, i32* (null)
  %7 = add i32 %6, 1
  store i32 %7, i32* (null)
  %8 = load i32, i32* (null)
  %9 = icmp i32 %8, 5
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %if_then4, label %if_merge6
if_then4:
  br label %if_merge6
if_merge6:
  %11 = load i32, i32* (null)
  %12 = load i32, i32* (null)
  %13 = add i32 %11, %12
  store i32 %13, i32* (null)
  br label %while_cond1
while_after3:
  %14 = load i32, i32* (null)
  ret i32 %14
}
