
define dso_local i32 @main() #0 {
entry:
  %1 = alloca i32
  %2 = alloca i32
  %3 = alloca i32
  %4 = alloca i32
  %5 = alloca i32
  store i32 10, i32* (null)
  store i32 20, i32* (null)
  store i32 5, i32* (null)
  store i32 15, i32* (null)
  %6 = load i32, i32* (null)
  %7 = load i32, i32* (null)
  %8 = sub i32 %6, %7
  %9 = load i32, i32* (null)
  %10 = load i32, i32* (null)
  %11 = sub i32 %9, %10
  %12 = mul i32 %8, %11
  %13 = load i32, i32* (null)
  %14 = load i32, i32* (null)
  %15 = sub i32 %13, %14
  %16 = load i32, i32* (null)
  %17 = load i32, i32* (null)
  %18 = sub i32 %16, %17
  %19 = mul i32 %15, %18
  %20 = add i32 %12, %19
  store i32 %20, i32* (null)
  %21 = load i32, i32* (null)
  ret i32 %21
}
