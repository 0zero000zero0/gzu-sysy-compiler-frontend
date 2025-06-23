## Sysy 2022编译器 (前端)
本项目是基于Flex，Bison的Sysy 2022编译器。

1. `sysy.l`是Flex的词法分析文件
2. `sysy.y`是Bison的句法分析文件
4. `irgen`是中间代码生成文件，遍历Bison生成的AST，生成 *LLVM Light IR*
5. `heplers`,`symbol`,`value`,`node`是辅助的文件

## 运行
前置要求:

3. FLex，Bison，GCC和Make软件
运行测试生成编译器
```bash
make build
```

编译器使用
```bash
./exe/sysy_complier <path-to-test-file> <path-to-LLVM-Light-IR> <path-to-AST>
```

例如
```bash
./exe/sysy_complier test/array.c ll/array.ll ast/array.txt
```

### 批量测试
前置要求:
1. 测试用例放在`test`文件夹下，用`*.c`命名
2. 创建`ast`,`ll`,`exe`文件夹，其中`ast`存放生成的AST，`ll`存放生成的*LLVM Light IR*代码，`exe`存放生成的可执行文件

对应文件树如下:
```bash
├── ast
│   ├── (生成的AST)
├── c
│   ├── (批量测试时，GCC编译生成的可执行文件)
├── chatgpt.txt
├── exe
│   └── (SySy 2022编译器可执行文件)
├── helpers.c
├── helpers.h
├── irgen.c
├── ir.h
├── lex.yy.c
├── LICENSE
├── ll
│   ├──(生成的LLVM IR)
├── Makefile
├── node.c
├── node.h
├── README.md
├── symbol.c
├── symbol.h
├── sysy.l
├── sysy.tab.c
├── sysy.tab.h
├── sysy.y
├── test
│   ├── (批量测试用例代码)
├── value.c
├── value.h
```

