# Smart Locker Business Flow Refactor Plan

> For Hermes: Use subagent-driven-development skill to implement this plan task-by-task.

Goal: 先把 smart_locker 现有“存件/取件/调试/UI 事件”职责划分清楚，形成单一可信业务流，再在此基础上继续补密码取件、UI 反馈、蜂鸣器提示等功能。

Architecture: 保留现有 ESP-IDF + FreeRTOS + LVGL + XST + locker/NVS 分层，不重做 UI 页面结构。重构重点不是页面美化，而是把“UI 只发请求、serve 只管业务编排、locker 只管柜体与绑定数据、main 只做初始化与状态刷新”这一职责边界落到代码里。所有业务结果通过明确的服务接口和状态更新通道回传，避免 UI 直接驱动底层流程或多个模块重复调用同一业务。

Tech Stack: ESP-IDF 5.3.3, FreeRTOS, LVGL, ESP32-S3, NVS, XST palm-vein module, C, Python unittest, compile_commands-based syntax verification.

---

## Current Problems Summary

1. UI 与业务层职责混乱
- `bsp/ui/generated/events_init.c` 直接 `xSemaphoreGive(ready_save/ready_take/verify_debug)`。
- `serve/serve.c` 再次直接做 XST enroll/verify 与 locker/NVS 操作。
- UI 没有正式的“请求 API”，只有直接碰内部同步原语。

2. 取件存在两套并行但未打通的流程
- `serve.c` 有掌静脉取件主链路。
- `events_init.c` 有密码取件 UI 逻辑，但密码校验仍是写死 `"0000"`。
- 二者没有统一业务状态与结果回传。

3. 页面状态没有与真实业务状态联动
- `save_page_label_1/2`、`save_page_bar_1` 没接入真实进度。
- 取件页面切换了密码 UI，但没有正式的“按密码开柜”服务接口。

4. 语义混乱
- `locker_t.is_locked` 实际更像“电磁锁触发/门当前开闭过程状态”，语义容易误用。
- 首页颜色刷新逻辑与变量命名容易导致错误理解。

5. 调试入口与正式业务入口未隔离
- `verify_debug_task` 与正式任务共享同一服务层文件，但未形成明确调试 API。

---

## Target Responsibility Split

### Layer 1: UI event layer (`bsp/ui/generated/events_init.c`)
Only do:
- 页面切换
- 收集用户输入
- 调用 `serve_*` 请求接口
- 根据服务层暴露的状态/结果刷新 label、bar、提示文案

Must NOT do:
- 直接 `xSemaphoreGive(...)`
- 直接决定柜号
- 直接操作 NVS
- 直接硬编码业务密码判断

### Layer 2: Business service layer (`serve/serve.c`, `serve/serve.h`)
Own:
- 存件业务状态机
- 取件业务状态机
- 调试请求入口
- 请求受理、防重入、状态回传
- 调用 XST、locker DB、locker GPIO、buzzer

Must NOT do:
- 创建 LVGL 控件
- 依赖具体页面控件名称

### Layer 3: Locker domain layer (`bsp/locker/locker.c`, `bsp/locker/locker.h`)
Own:
- 柜门物理控制
- 柜门开关检测
- 绑定关系数据库
- 密码生成与密码查询接口

### Layer 4: System bootstrap (`main/main.c`)
Own:
- 初始化硬件与服务层
- 周期性刷新主页柜体显示
- 不直接参与存/取业务判断

---

## Unified Business Flow After Refactor

### Save Flow
1. UI 进入 `save_page`
2. UI 调用 `serve_request_save()`
3. serve 判断当前是否忙碌；若忙则拒绝并更新状态
4. serve 执行：找空柜 -> palm enroll -> 生成密码 -> 入库 -> 开柜 -> 回传结果
5. UI 只订阅并展示：处理中/成功/失败、柜号、密码、提示文案、进度条

### Take Flow (phase 1 keep current palm path clear)
1. UI 进入 `take_page`
2. 若用户选择掌静脉取件：调用 `serve_request_take_by_palm()`
3. serve 执行：verify -> 查绑定 -> 开柜 -> 删除 XST 用户 -> 清绑定 -> 回传结果
4. UI 只展示处理中/成功/失败

### Take Flow (phase 2 later add password path)
1. UI 输入 4 位密码
2. UI 调用 `serve_request_take_by_password(password)`
3. serve 调 `locker_db_get_entry_by_password()` 查绑定
4. serve 开柜并清绑定
5. UI 展示成功/失败

### Debug Flow
- 设置页按钮只调 `serve_request_debug_verify()` 或明确的调试 API
- 调试状态与正式业务状态分离，避免污染正式流程

---

## Proposed Service API

### Task 1: Add explicit request/status API in `serve.h`

Objective: 用明确函数替代 UI 对信号量的直接访问，并定义统一业务状态结构。

Files:
- Modify: `serve/serve.h`
- Modify: `serve/serve.c`
- Test: `tests/test_business_flow_refactor_source.py`

Step 1: Write failing source-level tests

```python
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(rel):
    return (ROOT / rel).read_text(encoding="utf-8", errors="ignore")

class BusinessFlowRefactorSourceTests(unittest.TestCase):
    def test_serve_header_exposes_request_api(self):
        src = read("serve/serve.h")
        self.assertIn("typedef enum", src)
        self.assertIn("serve_request_save", src)
        self.assertIn("serve_request_take_by_palm", src)
        self.assertIn("serve_request_debug_verify", src)
        self.assertIn("serve_get_save_status", src)

if __name__ == "__main__":
    unittest.main()
```

Step 2: Run test to verify failure

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: FAIL — request API not present yet.

Step 3: Add minimal API definitions

Recommended shape:

```c
typedef enum {
    SERVE_FLOW_IDLE = 0,
    SERVE_FLOW_PENDING,
    SERVE_FLOW_RUNNING,
    SERVE_FLOW_SUCCESS,
    SERVE_FLOW_FAILED,
} serve_flow_state_t;

typedef struct {
    serve_flow_state_t state;
    uint8_t locker_id;
    uint8_t password[4];
    bool has_password;
    uint16_t user_id;
    esp_err_t err;
    char message[64];
} serve_save_status_t;

bool serve_request_save(void);
bool serve_request_take_by_palm(void);
bool serve_request_debug_verify(void);
void serve_get_save_status(serve_save_status_t *out);
```

Step 4: Run test to verify pass

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: PASS

Step 5: Commit

```bash
git add serve/serve.h serve/serve.c tests/test_business_flow_refactor_source.py
git commit -m "refactor: expose explicit business flow request api"
```

### Task 2: Move UI entrypoints from semaphores to service request API

Objective: 让 UI 通过 `serve_request_*` 调用业务，而不是直接碰内部同步原语。

Files:
- Modify: `bsp/ui/generated/events_init.c`
- Modify: `serve/serve.h`
- Test: `tests/test_business_flow_refactor_source.py`

Step 1: Extend failing test

```python
    def test_ui_events_use_serve_request_api_instead_of_direct_semaphore_give(self):
        src = read("bsp/ui/generated/events_init.c")
        self.assertIn("serve_request_save()", src)
        self.assertIn("serve_request_take_by_palm()", src)
        self.assertNotIn("xSemaphoreGive(ready_save)", src)
        self.assertNotIn("xSemaphoreGive(ready_take)", src)
```

Step 2: Run test to verify failure

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: FAIL — UI still uses raw semaphores.

Step 3: Implement minimal change
- `main_btn_1_event_handler()` 调 `serve_request_save()`
- `main_btn_2_event_handler()` 调 `serve_request_take_by_palm()`
- 设置页调试按钮调 `serve_request_debug_verify()`
- UI 层根据返回 bool 更新最基本提示，若拒绝请求则显示“当前忙碌，请稍后”

Step 4: Run tests

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: PASS

Step 5: Commit

```bash
git add bsp/ui/generated/events_init.c serve/serve.h tests/test_business_flow_refactor_source.py
git commit -m "refactor: route ui events through service api"
```

### Task 3: Centralize save-flow status updates in `serve.c`

Objective: 明确 save flow 的开始、处理中、成功、失败四类状态，并为 UI 提供读取接口。

Files:
- Modify: `serve/serve.c`
- Modify: `serve/serve.h`
- Modify: `bsp/ui/generated/events_init.c`
- Test: `tests/test_business_flow_refactor_source.py`

Step 1: Add failing test

```python
    def test_save_flow_has_explicit_status_update_points(self):
        src = read("serve/serve.c")
        self.assertIn("serve_get_save_status", src)
        self.assertIn("SERVE_FLOW_RUNNING", src)
        self.assertIn("SERVE_FLOW_SUCCESS", src)
        self.assertIn("SERVE_FLOW_FAILED", src)
```

Step 2: Run test to verify failure

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: FAIL

Step 3: Implement minimal status handling
- 在 `ready_save_task()` 开始时写入 RUNNING
- 无空柜时写入 FAILED + 文案
- enroll 成功后记录 user_id
- DB 保存成功后记录 locker_id/password
- 开柜完成后写入 SUCCESS
- 任一步失败都写入 FAILED
- `events_init.c` 在页面加载和适当时机读取状态并刷新：
  - `save_page_label_1`：主提示
  - `save_page_label_2`：简短状态/倒计时位
  - `save_page_bar_1`：进度

Step 4: Run tests

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: PASS

Step 5: Commit

```bash
git add serve/serve.c serve/serve.h bsp/ui/generated/events_init.c tests/test_business_flow_refactor_source.py
git commit -m "refactor: centralize save flow status reporting"
```

### Task 4: Extract password lookup into locker domain API

Objective: 为后续“密码取件”铺路，但此任务只做领域接口清晰化，不立即完成整套取件新功能。

Files:
- Modify: `bsp/locker/locker.h`
- Modify: `bsp/locker/locker.c`
- Test: `tests/test_business_flow_refactor_source.py`

Step 1: Add failing test

```python
    def test_locker_domain_exposes_password_lookup_api(self):
        header = read("bsp/locker/locker.h")
        src = read("bsp/locker/locker.c")
        self.assertIn("locker_db_get_entry_by_password", header)
        self.assertIn("locker_db_get_entry_by_password", src)
```

Step 2: Run test to verify failure

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: FAIL

Step 3: Implement minimal API

```c
esp_err_t locker_db_get_entry_by_password(const uint8_t password[4], user_locker_entry_t *entry);
```

Implementation rule:
- 只比较 4 位数字密码
- 只返回 `is_valid == true` 的条目
- 不在此任务中做 UI 或取件流程改动

Step 4: Run tests

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: PASS

Step 5: Commit

```bash
git add bsp/locker/locker.h bsp/locker/locker.c tests/test_business_flow_refactor_source.py
git commit -m "refactor: add locker password lookup domain api"
```

### Task 5: Clarify locker state naming/usage without large rename blast radius

Objective: 先把语义解释清楚并在主页刷新逻辑中统一使用，不做高风险大规模重命名。

Files:
- Modify: `main/main.c`
- Modify: `bsp/locker/locker.h`
- Modify: `bsp/locker/locker.c`
- Test: `tests/test_business_flow_refactor_source.py`

Step 1: Add failing test

```python
    def test_main_uses_detection_function_for_homepage_status_refresh(self):
        src = read("main/main.c")
        self.assertIn("Detection_locker_on_off", src)
```

Step 2: Run test to verify failure or weak coverage

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: either FAIL or reveal current refresh still relies on ambiguous state.

Step 3: Implement minimal clarification
- 在 `locker.h`/`locker.c` 注释中明确：
  - `have_saved` = 是否有存物绑定
  - `Detection_locker_on_off()` = 门磁/检测引脚的实时门状态
  - `is_locked` 仅保留为当前兼容字段，不再作为首页真实门态来源
- `main_serve()` 用 `Detection_locker_on_off()` 刷主页颜色，而不是依赖可能过时的 `is_locked`

Step 4: Run tests

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: PASS

Step 5: Commit

```bash
git add main/main.c bsp/locker/locker.h bsp/locker/locker.c tests/test_business_flow_refactor_source.py
git commit -m "refactor: clarify locker state usage for homepage refresh"
```

### Task 6: Verify with source tests and syntax checks

Objective: 在当前 shell 可能无法完整 `idf.py build` 的情况下，完成最低可信度验证。

Files:
- Modify: `tests/test_business_flow_refactor_source.py`
- Verify: `build/compile_commands.json`

Step 1: Run source-level regression tests

Run: `python -m unittest tests/test_business_flow_refactor_source.py -v`
Expected: PASS

Step 2: Run existing regression tests

Run: `python -m unittest tests/test_save_flow_source.py -v`
Expected: PASS

Step 3: Run syntax-level verification using compile database

Run a Python helper that extracts compile commands from `build/compile_commands.json` and checks modified C files with `-fsyntax-only`.

Target files:
- `main/main.c`
- `serve/serve.c`
- `bsp/locker/locker.c`
- `bsp/ui/generated/events_init.c`

Expected: all exit code 0

Step 4: Final commit

```bash
git add -A
git commit -m "refactor: clarify smart locker business flow boundaries"
```

---

## Rules for This Refactor

1. Keep generated screen layout files untouched unless absolutely necessary
- Prefer touching `events_init.c`, not `setup_scr_*.c`
- Existing widget names should be reused

2. Avoid speculative abstractions
- One save status struct, one take status struct is enough
- Do not invent generic event buses unless needed by current requirements

3. UI reads service state; service never writes LVGL objects
- This is the key boundary

4. Do not bundle password takeout feature into the refactor prematurely
- First make flow boundaries clear
- Then add feature on top

5. Preserve working save-flow persistence behavior already added
- Existing `locker_db_add_entry()` integration must not regress

---

## Verification Checklist

- [ ] UI no longer gives internal business semaphores directly
- [ ] `serve.h` exposes explicit request functions
- [ ] save flow has explicit status output for UI
- [ ] take flow main entrance is singular and clear
- [ ] locker domain has password lookup API for next phase
- [ ] homepage status refresh no longer depends on ambiguous state semantics alone
- [ ] `python -m unittest tests/test_business_flow_refactor_source.py -v` passes
- [ ] `python -m unittest tests/test_save_flow_source.py -v` passes
- [ ] syntax checks for modified C files pass via compile database

## Expected Result After This Plan

After completing this refactor, the codebase should have:
- 一个清晰的 UI -> serve -> locker/XST 业务入口链路
- 一个可读的 save flow 状态输出机制
- 一个明确可扩展的 take flow 入口
- 一个可直接继续实现“密码取件、UI 提示、蜂鸣器反馈”的稳定基础
