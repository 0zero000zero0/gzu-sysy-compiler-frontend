
define dso_local i32 @main() #0 {
entry:
  %1 = alloca float
  %2 = alloca float
  store float 1.732, float* (null)
  %3 = load i32, i32* (null)
  %4 = mul i32 %3, 2
  %5 = sub i32 %4, 3
  store float %5, float* (null)
  %6 = load i32, i32* (null)
  %7 = load i32, i32* (null)
  %8 = icmp i32 %6, %7
  %9 = load i32, i32* (null)
  %10 = icmp i32 %9, 0
  %11 = icmp i32 %8, %10
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %if_then1, label %if_merge3
if_then1:
  ret i32 10
if_merge3:
  ret i32 (null)
}
