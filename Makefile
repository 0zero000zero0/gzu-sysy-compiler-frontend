TEST_SRC_DIR := test
LL_DIR := ll
CST_DIR := ast

TEST_C_FILES := $(wildcard $(TEST_SRC_DIR)/*.c)
LL_FILES := $(patsubst $(TEST_SRC_DIR)/%.c,$(LL_DIR)/%.ll,$(TEST_C_FILES))
CST_FILES := $(patsubst $(TEST_SRC_DIR)/%.c,$(CST_DIR)/%.txt,$(TEST_C_FILES))
LL_FILES := $(patsubst $(TEST_SRC_DIR)/%.c,$(LL_DIR)/%.ll,$(TEST_C_FILES))

test: build
	@for file in $(TEST_C_FILES); do \
    	echo "Compiling $$file..."; \
        base="$${file##*/}"; \
        noext="$${base%.*}"; \
        ./exe/sysy_complier "$$file" "$(LL_DIR)/$$noext.ll" "$(CST_DIR)/$$noext.txt"; \
    done

build: flex bison
	gcc -g node.c sysy.tab.c lex.yy.c ir.c -o exe/sysy_complier
flex: sysy.l
	flex -o lex.yy.c sysy.l
bison: sysy.y
	bison -d  sysy.y
clean:
	rm -rf  sysy.tab.c sysy.tab.h lex.yy.c exe/*  $(LL_DIR)/* $(CST_DIR)/*