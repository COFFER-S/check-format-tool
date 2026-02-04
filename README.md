# check-format pre-commit hook

这是一个用于在提交代码时自动检查代码格式的 [pre-commit](https://pre-commit.com/) hook。

## 支持系统

- 🐧 Linux

---

## 1. 功能说明

此 pre-commit hook 会在每次 `git commit` 时自动运行，检查以下内容：

1. **分支名称格式** (`branch-name`)：检查分支名称是否符合规范
2. **代码风格** (`code-style`)：检查或格式化代码风格，确保代码符合项目规范
3. **提交信息** (``commit-message``)

---

## 2. 快速上手

### 2.1. 安装 pre-commit

```bash
pip install pre-commit
```

### 2.2. 配置 pre-commit

在项目根目录创建 `.pre-commit-config.yaml` 文件，添加以下配置：

```yaml
repos:
  - repo: https://github.com/COFFER-S/check-format-tool
    rev: v1.0.1
    hooks: 
      - id: code-style
        args: ['--local', '-vf', 'code-style']
        types_or: [c, c++]
      - id: branch-name
        args: ['branch-name']
      - id: commit-message
        args: ['--ci', 'commit-message']
```

或编辑 `.pre-commit-config.yaml` 文件，追加以下配置：

```yaml
  - repo: https://github.com/COFFER-S/check-format-tool
    rev: v1.0.1
    hooks:
      - id: code-style
        args: ['--local', '-vf', 'code-style']
        types_or: [c, c++]
      - id: branch-name
        args: ['branch-name']
      - id: commit-message
        args: ['--ci', 'commit-message']
```

### 2.3. 安装 hook

```bash
pre-commit install
```

### 2.4. 使用

#### 2.4.1. 自动运行（推荐）✨

每次执行 `git commit` 时，hook 会自动运行并检查：

- 分支名称格式
- 代码风格（仅检查暂存区的文件）

如果检查失败，commit 会被阻止，需要修复问题后重新提交。

#### 2.4.2. 手动运行

支持手动运行 hook 来检查所有文件：

```bash
# 检查所有文件
pre-commit run --all-files

# 只检查特定文件
pre-commit run --files path/to/file.c

# 只运行 check-format hook
pre-commit run check-format --all-files
```

#### 2.4.3. 跳过 hook（不推荐）
在 git commit -m "xxx" 后方加上 --no-verify 参数，如下：

```bash
git commit -m "feat(gmf): Add coze example" --no-verify
```

> **⚠️ 警告**：跳过 hook 可能导致不符合规范的代码被提交，请谨慎使用。

### 2.5. 参数

id: code-style 的 args
^^^^^^^^^^^^^^^^^^^^^^

- ``--local``：仅在本地环境运行，不存在简写形式
- ``--ci``：仅在 CI 环境运行，不存在简写形式
- ``-v``：打印详细信息，等同于 ``--verbose``
- ``-f``：允许直接格式化已改动的代码段（不询问），等同于 ``--format``
- ``-F``：格式化整个文件，等同于 ``--full``
- ``-vf``：允许直接格式化已改动的代码段（不询问），并打印详细信息，等同于 ``--verbose --format``
- ``-vFf``：允许直接格式化整个文件（不询问），并打印详细信息，等同于 ``--verbose --full --format``

---

## 3. 格式要求

### 3.1. 分支名称格式 (branch-name)

分支名称请遵循以下格式：

```
<prefix>/<description>
```

#### `<prefix>` 可选值

- `ci`
- `dev`
- `docs`
- `test`
- `debug`
- `bugfix`
- `feat`
- `feature`
- `customer`
- `release`

#### `<description>` 要求

- 英文小写
- 使用下划线或连字符分隔单词

**示例**：
- ✅ `feat/add_new_feature`
- ✅ `bugfix/fix_crash_issue`
- ❌ `Feature/AddNewFeature` (不符合规范)

---

### 3.2. 提交信息格式 (commit-message)

提交信息采用 **Conventional Commits** 风格，格式如下：

```
<type>(scope): <subject>
```

#### 3.2.1. type 要求

`<type>` 可选值：

- `ci`
- `dev`
- `change`
- `docs`
- `feat`
- `fix`
- `refactor`
- `remove`
- `customer`
- `release`
- `test`
- `revert`

#### 3.2.2. scope 要求

`<scope>` 必须非空，可以是任意小写字符串，用于标识变更相关的组件、模块或区域。

**常见示例**：

- **组件名**：`audio_stream`、`audio_board`、`esp-idf`、`esp-adf-libs` 等
- **特殊组件**：`ci`、`ut`、`docs`、`tools`、`examples`、`gmf_examples`、`gmf_core`、`packages` 等
- **项目名**：`esp-adf`、`esp-gmf` 等
- **其他自定义 scope**：根据项目需要自定义，如 `build`、`config`、`test` 等

#### 3.2.3. subject 要求

1. commit message 不能为空
2. 以 `Add`/`Fix`/`Remove`/`Change`/`Improve`/`Update` 等动词原型开头
3. 句子首字母大写
4. commit message 标题末尾不能出现 `.`
5. 若存在多行详细的 message，需要用空行将 commit message 标题与正文分隔开
6. 若存在 Close jira 链接的 message，需要用空行将其与上方的 commit message 正文或标题分隔开

#### 3.2.4. 示例

**✅ 正确示例 - 仅有标题**：

```
feat(audio_board): Add esp32_s3_korvo2_v3 board configurations
fix(audio_stream): Fix buffer overflow issue
docs(esp-adf): Update installation guide
```

**✅ 正确示例 - 含有正文**：

```
feat(audio_board): Add esp32_s3_korvo2_v3 board configurations

1. Update cli sdkconfig
2. Update esp-sr

Close https://xxxx
```

**❌ 错误示例**：

```
feat(audio_board): add esp32_s3_korvo2_v3 board configurations  (subject 首字母应大写)
feat: Add esp32_s3_korvo2_v3 board configurations               (缺少 scope)
feat(audio_board):Add esp32_s3_korvo2_v3 board configurations   (缺少空格)
feat(audio_board): Add esp32_s3_korvo2_v3 board configurations. (末尾不能有句号)
```

---

### 3.3. 代码风格格式 (code-style)

代码风格遵循 **OTBS (One True Brace Style)** 风格。

#### 3.3.1. 关于工具

本工具基于 [LLVM](https://llvm.org/) 开源项目的 [clang-format](https://clang.llvm.org/docs/ClangFormat.html) 进行扩展开发，兼容标准的 clang-format 配置文件格式，并在其基础上新增了多项配置选项以满足项目特定的代码风格需求。

#### 3.3.2. 配置文件

工具使用两个配置文件来控制代码格式化：

1. **`.clang-format`**：用于函数定义外的 include、全局变量、结构体、枚举等
2. **`.clang-format-function`**：用于函数定义

#### 3.3.3. 新增配置选项

在标准 clang-format 配置基础上，本工具新增了以下配置选项：

##### MaxLineLength

- **用途**：灵活控制代码行长度，仅在 `ColumnLimit: 0` 时生效
- **特点**：不强制换行，保留用户修改

##### MacroBackslashMinSpaces

- **用途**：设置宏定义中反斜杠（`\`）前的最小空格数
- **配置示例**：`MacroBackslashMinSpaces: 2`

##### StructMemberMinSpaces

- **用途**：确保结构体成员变量与类型之间的最小空格数（默认 2 个空格）
- **配置示例**：`StructMemberMinSpaces: 2`

##### AlignConsecutiveDeclarations 扩展选项

在 `AlignConsecutiveDeclarations` 配置下新增子控制选项：

- `AlignStruct: true/false` - 控制结构体成员声明对齐
- `AlignUnion: true/false` - 控制联合体成员声明对齐
- `AlignVariable: true/false` - 控制变量声明对齐

##### AlignConsecutiveAssignments 扩展选项

在 `AlignConsecutiveAssignments` 配置下新增子控制选项：

- `AlignEnum: true/false` - 控制枚举值赋值对齐（即使 `Enabled: false` 也会生效）
- `AlignMacroSingleLine: true/false` - 控制单行宏定义中的赋值对齐

#### 3.3.4. 主要特点

1. **大括号风格**：左大括号 `{` 与语句在同一行，右大括号 `}` 单独一行
2. **缩进**：使用 4 个空格
3. **对齐**：函数外的相邻的结构体、共用体、枚举、注释、续行符需要对齐，其余不对齐
4. **空格**：结构体成员变量与类型之间、宏定义的 KEY 与 Value 之间、注释与代码之间、续行符与代码之间均需要两个空格

#### 3.3.5. 注意事项

##### a. 结构体成员变量/数组元素中的最后一个逗号的两种用法

**期望多个元素处于同一行**：

```c
// 格式化前
uint32_t array[] = {
    1, 2, 3
};

// 格式化后
uint32_t array[] = {1, 2, 3};
```

**期望每个元素单独一行**：

```c
// 格式化前
uint32_t array[] = {1, 2, 3,};

// 格式化后
uint32_t array[] = {
    1,
    2,
    3,
};
```

##### b. 块注释

块注释格式对 `@note` 当行进行格式化，对 `@note` 下方紧邻部分不进行格式化，用户可以自由发挥。

```c
/**
 * @brief  描述与'brief'之间需两个空格
 *
 * @param[in]  p       参数与'@param'之间需两个空格
 * @param[in]  path    注解与参数之间需至少两个空格
 * @param[in]  enable  紧邻的参数/注解均需要对齐
 *
 * @note  注解与'@note'之间需两个空格
 *        note 下方紧邻部分不进行格式化，用户可以自由发挥
 *        可画图，可写代码，可写表格等
 *        在 note 块下方需要添加一个空行，表示 note 块结束
 *
 * @return
 *       - ESP_CAPTURE_ERR_OK  'return'部分一定也是'Key-Value'的格式，Key必须使用下划线连接
 *       - Others               紧邻的注解需要对齐
 */
```

---

## 4. 配置说明

### 4.1. 配置文件搜索优先级

工具使用两个配置文件来控制代码格式化：

- **`.clang-format`**：用于函数定义外的 include、全局变量、结构体、枚举等
- **`.clang-format-function`**：用于函数定义

配置文件搜索优先级（从高到低）：

1. **项目根目录**（包含 `.git` 的目录）
   - **推荐位置**：将配置文件放在项目根目录（git 仓库根目录）

2. **`$CHECK_TOOLS_PATH` 环境变量指定的路径**
   - 如果设置了 `CHECK_TOOLS_PATH` 环境变量，会在该路径下查找配置文件

3. **可执行文件目录**
   - 打包模式：可执行文件所在目录

4. **PyInstaller 临时目录**
   - 打包环境中的目录

**推荐做法**：

- ✅ 使用包中自带的默认配置
- ✅ **将配置文件放在项目根目录**（git 仓库根目录）

**注意事项**：

- 如果在前三个位置找到配置文件，会优先使用外部文件
- 只有在外部找不到时，才会使用打包的默认配置文件
- 如果使用了打包的配置文件，会显示警告：`Using default configuration files from package`
- 两个配置文件（`.clang-format` 和 `.clang-format-function`）分别按上述顺序搜索，可能来自不同位置

### 4.2. 文件类型过滤

默认情况下，hook 只检查以下文件类型：

- `.c` - C 源文件
- `.h` - C 头文件
- `.cpp` - C++ 源文件
- `.hpp` - C++ 头文件

可以通过修改 `.pre-commit-hooks.yaml` 中的 `files` 字段来调整。

### 4.3. 环境控制参数

工具支持 `--ci` 和 `--local` 参数来控制命令在特定环境下运行：

#### `--ci` 参数

- **用途**：仅在 CI 环境运行命令（当 `CI` 环境变量被设置时）
- **适用场景**：在 CI/CD 流水线中运行检查
- **示例**：
  ```yaml
  - id: commit-message
    args: ['--ci', 'commit-message']
  ```

#### `--local` 参数

- **用途**：仅在本地环境运行命令（当 `CI` 环境变量未设置时）
- **适用场景**：在本地开发环境中运行检查
- **示例**：
  ```yaml
  - id: code-style
    args: ['--local', '-vf', 'code-style']
    types_or: [c, c++]
  ```

#### 参数组合规则

- **同时指定 `--ci` 和 `--local`**：命令总是运行（等同于均不设置）
- **仅指定 `--ci`**：仅在 CI 环境运行
- **仅指定 `--local`**：仅在本地环境运行
- **均不指定**：命令总是运行（默认行为）

#### 推荐配置

- ✅ **本地开发**：使用 `--local` 参数，确保检查只在本地运行
- ✅ **CI 环境**：使用 `--ci` 参数，确保检查只在 CI 环境运行
- ✅ **混合场景**：可以同时配置两个 hook，分别使用 `--local` 和 `--ci` 参数

**配置示例**：

```yaml
repos:
  - repo: https://github.com/COFFER-S/check-format-tool
    rev: v1.0.1
    hooks:
      - id: code-style
        args: ['--local', '-vf', 'code-style']  # 只在本地运行
        types_or: [c, c++]
      - id: branch-name
        args: ['branch-name']  # 总是运行
      - id: commit-message
        args: ['--ci', 'commit-message']  # 只在 CI 运行
```

---

## 5. 工作原理

1. **pre-commit 框架**：当执行 `git commit` 时，pre-commit 框架会拦截提交
2. **hook 执行**：pre-commit 调用 `check_format.py` 脚本
3. **二进制调用**：脚本根据操作系统找到对应的二进制文件并执行
4. **格式检查**：二进制文件检查分支名称、代码风格
5. **结果处理**：
   - ✅ 如果检查通过，提交继续进行
   - ❌ 如果检查失败，提交被阻止，显示错误信息

---

## 6. 故障排除

**错误信息**：

```
Check format........................................................Failed
```

**解决方案**：

1. 查看详细的错误信息，通常会在错误信息中说明具体问题
2. 检查代码格式是否符合项目规范
3. 检查分支名称是否正确

---

## 7. 更新 hook

要更新到新版本的 hook：

1. 更新 `.pre-commit-config.yaml` 中的版本号
2. 运行 `pre-commit autoupdate` 自动更新
3. 或者手动运行 `pre-commit install --install-hooks` 重新安装

---

## 8. 相关文档

- [pre-commit 官方文档](https://pre-commit.com/)
- [代码风格参考](./otbs_style)

---

## 9. Issue

如果发现问题或有改进建议，请在项目中创建 issue。

---

## 10. 许可证

Apache-2.0 License
