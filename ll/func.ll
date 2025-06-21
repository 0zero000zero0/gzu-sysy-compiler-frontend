
define dso_local i32 @square(i32, i32) #0 {
entry:
  %1 = alloca i32
  store i32 %0, i32* %1
  %2 = alloca i32
  store i32 %1, i32* %2
  %3 = load i32, i32* %1
  %4 = load i32, i32* %1
  %5 = mul i32 %3, %4
  %6 = load i32, i32* %2
  %7 = load i32, i32* %2
  %8 = mul i32 %6, %7
  %9 = add i32 %5, %8
  ret i32 %9
}

define dso_local i32 @main() #0 {
entry:
  %10 = alloca i32
  %11 = alloca i32
  store i32 3, i32* %10
  store i32 4, i32* %11
  %12 = load i32, i32* %10
  %13 = load i32, i32* %11
  %14 = call i32 @square(i32 %12, i32 %13)
  ret i32 %14
}
