# Copyright 2022 Woven Planet Holdings
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
            self.assertEqual(2, int(json.loads(str(records[0]))['severity']))
            self.assertEqual('Hello World', json.loads(
                str(records[0]))['tags'][0]['valueString'])


if __name__ == '__main__':
    unittest.main()
