from unittest import TestCase as Test
from unittest import main
#import pexpect
from pexpect import replwrap

# Variable prompt REPL
class VPREPLWrapper(replwrap.REPLWrapper):
    def _expect_prompt(self, timeout=-1, async_=False):
        return self.child.expect([self.prompt, self.continuation_prompt],
                timeout=timeout, async_=async_)

class BaseExec(Test):
    def setUp(self):
        super().setUp()
        self.console = VPREPLWrapper("./src/tinylisp", "lissh>", None, continuation_prompt="lissh>")
        self.newline = "\r\n"

    def tearDown(self):
        super().tearDown()


if __name__ == '__main__':
    main()
