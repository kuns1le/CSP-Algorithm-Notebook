# 输入格式模板

先看题目的“输入”部分决定循环结构，不靠样例猜多组。以下片段放在 main 或 solve 中，按需包含 iostream、vector、string、sstream、limits 等头文件，并使用 std 命名空间。

## 基础骨架

完整骨架见 [main.cpp](../templates/main.cpp)。常用设置：

~~~cpp
ios::sync_with_stdio(false);
cin.tie(nullptr);
~~~

关闭同步后统一使用 cin/cout。需要交互的题目按协议刷新输出。

## 1. 单个数、固定数量、长度已知的数组

~~~cpp
int n;
cin >> n;
vector<long long> a(n);
for (auto& x : a) cin >> x;
~~~

cin >> 会跳过空格和换行，数组是否写在一行通常没有影响。

## 2. 明确给出测试组数 T

~~~cpp
int T;
cin >> T;
while (T--) {
    int n;
    cin >> n;
    vector<int> a(n); // 每组重新创建，避免上一组数据残留。
    for (int& x : a) cin >> x;
    // 处理当前测试组。
}
~~~

只有输入明确存在 T 才使用此结构。

## 3. 读到 EOF

~~~cpp
long long a, b;
while (cin >> a >> b) {
    cout << a + b << '\n';
}
~~~

不要使用 while (!cin.eof())，因为读取失败后才会设置 EOF 状态。

## 4. 特定结束标记

~~~cpp
int n;
while (cin >> n && n != 0) {
    // 处理这一组；0 是结束标记，不是数据。
}
~~~

结束条件必须按题意修改；有的题目以一整组全零结束。

## 5. 单词、整行，以及混合读取

~~~cpp
string word;
cin >> word; // 不能包含空格。
string line;
getline(cin, line); // 从当前位置读到换行，可以得到空行。
~~~

读完一个整数，再开始读取完整的一行：

~~~cpp
int n;
cin >> n;
cin.ignore(numeric_limits<streamsize>::max(), '\n');
string line;
getline(cin, line);
~~~

如果空行和前导空白无意义，也可 getline(cin >> ws, line)。ws 会吃掉所有前导空白，不能用于必须保留空行或缩进的输入。

一行中整数个数不定：

~~~cpp
string line;
while (getline(cin, line)) {
    istringstream in(line);
    long long x;
    while (in >> x) {
        // 处理 x。
    }
}
~~~

## 6. 矩阵和字符网格

~~~cpp
int n, m;
cin >> n >> m;
vector<vector<int>> a(n, vector<int>(m));
for (auto& row : a) for (int& x : row) cin >> x;

vector<string> grid(n);
for (auto& row : grid) cin >> row; // 每行无空格，长度为 m。
~~~

如果矩阵元素后紧跟网格，只有题目确实同时包含两部分时才把这两段连用。

## 7. 边表转邻接表

~~~cpp
int n, m;
cin >> n >> m;
vector<vector<int>> g(n);
for (int i = 0; i < m; ++i) {
    int u, v;
    cin >> u >> v;
    --u; --v; // 题目编号 1..n，模板统一成 0..n-1。
    g[u].push_back(v);
    g[v].push_back(u); // 无向图；有向图删除这一行。
}
~~~

有边权时使用 vector<vector<pair<int,long long>>>，并同时读取 w。邻接矩阵、重边、自环和连通性要分别根据题意处理。

## 输出检查

- 每个答案是否需要换行、固定小数位、大小写或 Case 编号？
- 不输出调试提示、中文说明或多余符号。
- 浮点输出按题意使用 fixed 和 setprecision，需包含 iomanip。
