
import sys, os
sys.path.append(os.path.dirname(__file__))
from unittest import main
from lisp_setup import BaseExec


class Number(BaseExec):
    def test_add_two_numbers(self):
        output = self.console.run_command("(+ 255 1)")
        self.assertEqual(output.strip(), "256")

    def test_add_number_and_nil(self):
        output = self.console.run_command("(+ 255 ())")
        self.assertEqual(output.strip(), "ERR")

    def test_add_number_and_atom(self):
        output = self.console.run_command("(+ 't 255)")
        self.assertEqual(output.strip(), "ERR")

    def test_add_number_and_true(self):
        output = self.console.run_command("(+ 255 #t)")
        self.assertEqual(output.strip(), "ERR")

if __name__ == '__main__':
    main()
