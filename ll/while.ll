
define dso_local i32 @main() #0 {
entry:
  %1 = alloca i32
  %2 = alloca i32
  store i32 100, i32* (null)
  store i32 0, i32* (null)
  br label %while_cond1
while_cond1:
  %3 = load i32, i32* (null)
  %4 = icmp i32 %3, 10
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %while_body2, label %while_after3
while_body2:
  %6 = load i32, i32* (null)
  %7 = load i32, i32* (null)
  %8 = add i32 %6, %7
  store i32 %8, i32* (null)
  %9 = load i32, i32* (null)
  %10 = sub i32 %9, 1
  store i32 %10, i32* (null)
  br label %while_cond1
while_after3:
  %11 = load i32, i32* (null)
  ret i32 %11
}
