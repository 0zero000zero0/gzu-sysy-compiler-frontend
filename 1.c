// ================================================================== //
//                                                                    //
//      修正所有辅助函数以正确处理 Value* //
//                                                                    //
// ================================================================== //

// generate_lval_address: 为左值(LVal)生成地址
static char *generate_lval_address(Node *node)
{
    // 基准情形：一个简单的变量
    if (strcmp(node->name, "LVal") == 0)
    {
        Node *ident_node = node->children[0];
        Symbol *sym = lookup_symbol(ident_node->name);
        if (sym)
        {
            return strdup(sym->llvm_reg);
        }
        fprintf(stderr, "FATAL: Undefined symbol '%s' used as LVal.\n", ident_node->name);
        exit(1);
    }

    // 递归情形：数组访问
    if (strcmp(node->name, "ArrayAccess") == 0)
    {
        char *base_ptr_reg = generate_lval_address(node->children[0]);
        Value *index_val = generate_code(node->children[1]);

        // 获取基地址指针指向的类型
        Node *base_var_node = node->children[0];
        char *aggregate_type = NULL;
        if (strcmp(base_var_node->name, "LVal") == 0)
        {
            Symbol *sym = lookup_symbol(base_var_node->children[0]->name);
            aggregate_type = strdup(sym->type);
        }
        else
        {
            // 简化处理多维数组的中间类型推断
            Node *root_lval_node = base_var_node;
            while (strcmp(root_lval_node->name, "ArrayAccess") == 0)
            {
                root_lval_node = root_lval_node->children[0];
            }
            Symbol *sym = lookup_symbol(root_lval_node->children[0]->name);
            aggregate_type = strdup(sym->type);
        }

        char *res_ptr_reg = new_reg();
        // 修正GEP指令，使其对多维数组更健壮
        emit("\t%s = getelementptr inbounds %s, %s* %s, i32 0, %s %s",
             res_ptr_reg, aggregate_type, aggregate_type, base_ptr_reg, index_val->type, index_val->reg);

        free(aggregate_type);
        free(base_ptr_reg);
        freeValue(index_val);
        return res_ptr_reg;
    }

    fprintf(stderr, "FATAL: generate_lval_address called with unexpected node type '%s'.\n", node->name);
    exit(1);
}

static char *build_llvm_type_str(Node *dim_node, const char *base_type)
{
    if (!dim_node || dim_node->num_children == 0)
    {
        return strdup(base_type);
    }
    char *inner_type = build_llvm_type_str(dim_node->children[1], base_type);

    Value *dim_size_val = generate_code(dim_node->children[0]);
    if (strcmp(dim_size_val->type, "i32") != 0)
    {
        fprintf(stderr, "FATAL: Array dimension must be an integer.\n");
        exit(1);
    }

    char result_buf[512];
    sprintf(result_buf, "[%s x %s]", dim_size_val->reg, inner_type);

    free(inner_type);
    freeValue(dim_size_val);

    return strdup(result_buf);
}

// process_var_def_list: 处理变量定义列表
static void process_var_def_list(Node *n, const char *base_type)
{
    if (!n || n->num_children == 0)
        return;

    Node *var_def_node = n->children[0];
    char *var_name = var_def_node->children[0]->name;
    Node *dim_list_node = var_def_node->children[1];
    char *llvm_type = build_llvm_type_str(dim_list_node, base_type);
    char *ptr_reg = new_reg();

    emit("\t%s = alloca %s", ptr_reg, llvm_type);
    add_symbol(var_name, llvm_type, ptr_reg, 0);

    if (dim_list_node->num_children > 0)
    {
        emit("\tstore %s zeroinitializer, %s* %s", llvm_type, llvm_type, ptr_reg);
    }

    if (strcmp(var_def_node->name, "VarDef_Init") == 0)
    {
        Node *init_val_node = var_def_node->children[2];
        if (dim_list_node->num_children > 0)
        {
            int dims[10] = {0}, num_dims = 0;
            get_dimensions(dim_list_node, dims, &num_dims);
            int flat_index = 0;
            if (init_val_node->children[0] && init_val_node->children[0]->num_children > 0)
            {
                generate_initializer_stores(init_val_node->children[0], ptr_reg, llvm_type, dims, num_dims, &flat_index);
            }
        }
        else
        {
            Value *init_val = generate_code(init_val_node);
            char *final_init_reg = init_val->reg;

            if (strcmp(base_type, init_val->type) != 0)
            {
                final_init_reg = new_reg();
                if (strcmp(base_type, "float") == 0)
                {
                    emit("\t%s = sitofp %s %s to float", final_init_reg, init_val->type, init_val->reg);
                }
                else
                {
                    emit("\t%s = fptosi %s %s to i32", final_init_reg, init_val->type, init_val->reg);
                }
            }
            emit("\tstore %s %s, %s* %s", base_type, final_init_reg, base_type, ptr_reg);

            if (final_init_reg != init_val->reg)
                free(final_init_reg);
            freeValue(init_val);
        }
    }

    free(llvm_type);
    free(ptr_reg);

    if (n->num_children > 1)
    {
        process_var_def_list(n->children[n->num_children - 1], base_type);
    }
}

// get_dimensions: 从维度AST节点中提取维度信息
static void get_dimensions(Node *dim_node, int *dims, int *num_dims)
{
    if (!dim_node || dim_node->num_children == 0)
        return;

    Value *dim_val = generate_code(dim_node->children[0]);
    dims[(*num_dims)++] = atoi(dim_val->reg);
    freeValue(dim_val);

    get_dimensions(dim_node->children[1], dims, num_dims);
}

// generate_initializer_stores: 生成初始化存储指令
static void generate_initializer_stores(Node *init_node, char *base_ptr, char *llvm_type, int *dims, int num_dims, int *flat_index)
{
    if (!init_node)
        return;

    if (strcmp(init_node->name, "ConstExp") == 0 || strcmp(init_node->name, "Exp") == 0)
    {
        Value *val = generate_code(init_node);

        char gep_indices[256] = "i32 0";
        char temp[32];
        int temp_index = *flat_index;
        for (int i = 0; i < num_dims; i++)
        {
            int dim_prod = 1;
            for (int j = i + 1; j < num_dims; j++)
                dim_prod *= dims[j];
            int index_i = temp_index / dim_prod;
            sprintf(temp, ", i32 %d", index_i);
            strcat(gep_indices, temp);
            temp_index %= dim_prod;
        }

        char *element_ptr = new_reg();
        char element_type[64];
        strcpy(element_type, (strstr(llvm_type, "float") ? "float" : "i32"));

        char *final_val_reg = val->reg;
        if (strcmp(element_type, val->type) != 0)
        {
            final_val_reg = new_reg();
            if (strcmp(element_type, "float") == 0)
                emit("\t%s = sitofp %s %s to float", final_val_reg, val->type, val->reg);
            else
                emit("\t%s = fptosi %s %s to i32", final_val_reg, val->type, val->reg);
        }

        emit("\t%s = getelementptr inbounds %s, %s* %s, %s", element_ptr, llvm_type, llvm_type, base_ptr, gep_indices);
        emit("\tstore %s %s, %s* %s", element_type, final_val_reg, element_type, element_ptr);

        if (final_val_reg != val->reg)
            free(final_val_reg);
        freeValue(val);
        free(element_ptr);
        (*flat_index)++;
    }
    else if (strcmp(init_node->name, "InitVal_Aggregate") == 0)
    {
        generate_initializer_stores(init_node->children[0], base_ptr, llvm_type, dims, num_dims, flat_index);
    }
    else if (strcmp(init_node->name, "ConstInitVal_List") == 0 || strcmp(init_node->name, "InitVal_List") == 0)
    {
        generate_initializer_stores(init_node->children[0], base_ptr, llvm_type, dims, num_dims, flat_index);
        if (init_node->num_children > 1)
        {
            generate_initializer_stores(init_node->children[1], base_ptr, llvm_type, dims, num_dims, flat_index);
        }
    }
}

// process_block_item_list: 递归处理语句块内条目
static void process_block_item_list(Node *n)
{
    if (!n || n->num_children == 0)
        return;

    freeValue(generate_code(n->children[0]));

    if (n->num_children > 1)
    {
        process_block_item_list(n->children[1]);
    }
}