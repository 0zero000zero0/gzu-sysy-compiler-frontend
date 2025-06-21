
define dso_local i32 @main() #0 {
entry:
 %1 = alloca [10 x [2 x i32]]
  store [10 x [2 x i32]] zeroinitializer, [10 x [2 x i32]]* %1
 %2 = alloca i32
 %3 = alloca i32
 %4 = alloca i32
  store i32 0, i32* %3
  store i32 0, i32* %2
  br label %while_cond1
while_cond1:
  %5 = load i32, i32* %3
  %6 = icmp slt i32 %5, 10
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %while_body2, label %while_after3
while_body2:
  store i32 0, i32* %4
  br label %while_cond4
while_cond4:
  %8 = load i32, i32* %4
  %9 = icmp slt i32 %8, 2
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %while_body5, label %while_after6
while_body5:
  %11 = load i32, i32* %2
  %12 = load i32, i32* %3
 %13 = getelementptr i32, i32* %1, i32 %12
  %14 = load i32, i32* %4
 %15 = getelementptr i32, i32* %13, i32 %14
  %16 = load i32, i32* %15
  %17 = add i32 %11, %16
  store i32 %17, i32* %2
  %18 = load i32, i32* %4
  %19 = add i32 %18, 1
  store i32 %19, i32* %4
  br label %while_cond4
while_after6:
  %20 = load i32, i32* %3
  %21 = add i32 %20, 2
  store i32 %21, i32* %3
  br label %while_cond1
while_after3:
  %22 = load i32, i32* %2
  ret i32 %22
}
