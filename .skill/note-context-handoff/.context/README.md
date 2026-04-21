这个目录用于保存 context-handoff-summary skill 生成的续聊上下文。

文件约定：
- current.md：当前最新、下次新对话优先读取的精简上下文
- history/：每次更新时保存的历史快照

推荐流程：
1. 新对话开始前先读取 current.md
2. 继续完成任务
3. 对话结束、阶段结束或用户要求“保存上下文”时，重新生成并覆盖 current.md
4. 同时写入 history/ 下的时间戳快照
