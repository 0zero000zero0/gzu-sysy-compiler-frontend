
define dso_local i32 @main() #0 {
entry:
 %1 = alloca i32
 %2 = alloca i32
 %3 = alloca i32
 %4 = alloca i32
  store i32 1, i32* %1
  store i32 2, i32* %2
  %5 = load i32, i32* %2
  %6 = load i32, i32* %1
  %7 = mul i32 %6, 2
  %8 = add i32 %7, 4
  %9 = icmp slt i32 %5, %8
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %if_then1, label %if_else2
if_then1:
  store i32 0, i32* %3
  br label %while_cond4
while_cond4:
  %11 = load i32, i32* %3
  %12 = icmp slt i32 %11, 10
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %while_body5, label %while_after6
while_body5:
  %14 = load i32, i32* %4
  %15 = load i32, i32* %3
  %16 = mul i32 %15, 2
  %17 = add i32 %14, %16
  store i32 %17, i32* %4
  %18 = load i32, i32* %3
  %19 = add i32 %18, 1
  store i32 %19, i32* %3
  br label %while_cond4
while_after6:
  br label %if_merge3
if_else2:
  store i32 100, i32* %4
  br label %if_merge3
if_merge3:
  %20 = load i32, i32* %4
  ret i32 %20
}
