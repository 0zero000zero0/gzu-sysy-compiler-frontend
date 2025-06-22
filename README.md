## Sysy 2022编译器 (前端)
本项目是基于Flex，Bison的Sysy 2022编译器。

1. `sysy.l`是Flex的词法分析文件
2. `sysy.y`是Bison的句法分析文件
3. `node`是AST的结点类
4. `ir`是中间代码生成文件，把Bison生成的AST转换成 *LLVM Light IR*

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
.
├── ast
│   ├──(生成的AST)
├── exe
│   └── sysy_complier
├── ir.c
├── ir.h
├── lex.yy.c
├── ll
│   ├──(生成的LLVM Light IR)
├── Makefile
├── node.c
├── node.h
├── README.md
├── sysy.l
├── sysy.tab.c
├── sysy.tab.h
├── sysy.y
└── test
    ├──(测试用例)
```

