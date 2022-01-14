import json
import unittest

from slog_py import slog
from slog_py.slog_pybind import SlogBuffer, SlogCallSite, SlogRecord


class SlogPyTest(unittest.TestCase):
    def test_severities(self):
        with SlogBuffer(slog.SlogContext.get_instance()) as slog_buffer:
            slog.info('Hello World')
            slog_buffer.wait_slog_queue()
            records = slog_buffer.flush().records

            self.assertEqual(1, len(records))
            self.assertEqual(0, int(json.loads(str(records[0]))['severity']))
            self.assertEqual('Hello World', json.loads(
                str(records[0]))['tags'][0]['valueString'])


if __name__ == '__main__':
    unittest.main()
