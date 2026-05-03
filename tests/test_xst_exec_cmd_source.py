import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def read(rel):
    return (ROOT / rel).read_text(encoding="utf-8", errors="ignore")


class XstExecCmdSourceTests(unittest.TestCase):
    def test_exec_cmd_ignores_progress_or_stale_replies_until_expected_mid_arrives(self):
        src = read("bsp/XST/xst.c")
        start = src.index("static xst_result_t xst_exec_cmd")
        end = src.index("// ============== 其余向外暴露的API保持原样不变", start)
        func = src[start:end]

        self.assertIn("TickType_t start_tick", func)
        self.assertIn("remaining_ticks", func)
        self.assertIn("body->mid != cmd", func)
        self.assertIn("continue", func)
        self.assertNotIn("return MR_FAILED4_UNKNOWN_REASON;\n        }\n\n        xst_result_t res", func)


if __name__ == "__main__":
    unittest.main()
