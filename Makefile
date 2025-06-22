TEST_SRC_DIR := test
LL_DIR := ll
CST_DIR := ast
GCC_dir :=c
TEST_C_FILES := $(wildcard $(TEST_SRC_DIR)/*.c)
LL_FILES := $(patsubst $(TEST_SRC_DIR)/%.c,$(LL_DIR)/%.ll,$(TEST_C_FILES))
CST_FILES := $(patsubst $(TEST_SRC_DIR)/%.c,$(CST_DIR)/%.txt,$(TEST_C_FILES))
GCC_FILES := $(patsubst $(TEST_SRC_DIR)/%.c,$(GCC_dir)/%,$(TEST_C_FILES))

test: build
	@for file in $(TEST_C_FILES); do \
		echo "--- Testing $$file ---"; \
		base="$${file##*/}"; \
		noext="$${base%.*}"; \
		./exe/sysy_complier "$$file" "$(LL_DIR)/$$noext.ll" "$(CST_DIR)/$$noext.txt"; \
		gcc "$$file" -o "$(GCC_dir)/$$noext"; \
		if [ $$? -ne 0 ]; then \
			echo "Complier failed for $$file"; \
			continue; \
		fi; \
		lli "$(LL_DIR)/$$noext.ll"; \
		echo "Test finished with exit code: $$?."; \
		./$(GCC_dir)/$$noext; \
		echo "GCC execution finished with exit code: $$?."; \
		echo ""; \
	done

build: flex bison
	gcc -g node.c sysy.tab.c lex.yy.c ir.c -o exe/sysy_complier
flex: sysy.l
	flex -o lex.yy.c sysy.l
bison: sysy.y
	bison -d  sysy.y
clean:
	rm -rf  sysy.tab.c sysy.tab.h lex.yy.c exe/*  $(LL_DIR)/* $(CST_DIR)/*