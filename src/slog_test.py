# This test is running various scenarios for Slog in Python. It is not easy to
# check what is printed to stdout via underlying C++ code so these tests are not
# checking correctness of the printed/published logs, that should be checked
# manually. But this test may catch typos and missing implementations in the
# code automatically.

import json
import unittest
from src.common.logging.slog import slog
from src.common.logging.slog.buffer.buffer import SlogBuffer

# Below imports are not used in the code of this file directly but they
# are used by methods of SlogBuffer and if we don't import them those
# methods will throw exception that some types are not defined.
from src.common.logging.slog.primitives.primitives import SlogCallSite  # noqa
from src.common.logging.slog.primitives.primitives import SlogRecord  # noqa


class SlogPyTest(unittest.TestCase):
    def test_severities(self):
        with SlogBuffer() as slog_buffer:
            slog.info('Hi INFO')
            slog.warning('Hi WARNING')
            slog.error('Hi ERROR')

            slog_buffer.waitSlogQueue()
            records = slog_buffer.flush().records

            self.assertEqual(3, len(records))
            self.assertEqual(0, int(json.loads(str(records[0]))['severity']))
            self.assertEqual(1, int(json.loads(str(records[1]))['severity']))
            self.assertEqual(2, int(json.loads(str(records[2]))['severity']))

    def test_silent_noisy(self):
        with SlogBuffer() as slog_buffer:
            slog.info('some noise', tags={'is_noisy': 1})
            slog.info(tags={'is_noisy': 0})

            slog_buffer.waitSlogQueue()
            records = slog_buffer.flush().records

            self.assertEqual(2, len(records))
            record_0 = json.loads(str(records[0]))
            self.assertEqual(2, len(record_0['tags']))
            self.assertEqual('_t0_s', record_0['tags'][0]['key'])
            self.assertEqual('some noise', record_0['tags'][0]['valueString'])
            self.assertEqual('is_noisy', record_0['tags'][1]['key'])
            self.assertEqual(1, int(record_0['tags'][1]['valueInt']))
            record_1 = json.loads(str(records[1]))
            self.assertEqual(1, len(record_1['tags']))
            self.assertEqual('is_noisy', record_1['tags'][0]['key'])
            self.assertEqual(0, int(record_1['tags'][0]['valueInt']))

    def test_call_site(self):
        with SlogBuffer() as slog_buffer:
            slog.info(tags={'foo': 'bar'})

            slog_buffer.waitSlogQueue()
            buffer_data = slog_buffer.flush()
            records = buffer_data.records
            call_sites = buffer_data.call_sites

            self.assertEqual(1, len(records))
            record_0 = json.loads(str(records[0]))
            call_site = call_sites[int(record_0['call_site_id'])]
            self.assertTrue(call_site.file().endswith('src/slog_test.py'))
            self.assertEqual(56, call_site.line())
            self.assertEqual('test_call_site', call_site.function())

    def test_scope(self):
        with SlogBuffer() as slog_buffer:
            with slog.scope('foo_scope', tags={'foo': 'bar'}):
                slog.info(tags={'foo': 1.5})

            slog_buffer.waitSlogQueue()
            buffer_data = slog_buffer.flush()
            records = buffer_data.records
            call_sites = buffer_data.call_sites

            self.assertEqual(3, len(records))

            record_0 = json.loads(str(records[0]))
            call_site = call_sites[int(record_0['call_site_id'])]
            self.assertTrue(call_site.file().endswith('src/slog_test.py'))
            self.assertEqual(72, call_site.line())
            self.assertEqual('test_scope', call_site.function())
            self.assertEqual(5, len(record_0['tags']))
            self.assertEqual('.scope_name', record_0['tags'][0]['key'])
            self.assertEqual('foo_scope', record_0['tags'][0]['valueString'])

            record_1 = json.loads(str(records[1]))
            self.assertEqual(1, len(record_1['tags']))
            self.assertEqual('foo', record_1['tags'][0]['key'])
            self.assertEqual(1.5, float(record_1['tags'][0]['valueDouble']))

            record_2 = json.loads(str(records[2]))
            self.assertEqual(0, int(record_2['call_site_id']))
            self.assertEqual(2, len(record_2['tags']))

    def test_scope_decorator(self):
        with SlogBuffer() as slog_buffer:
            @slog.scope_decorator()
            def f(msg):
                slog.info('msg: {}'.format(msg))
            f('msg1')

            slog_buffer.waitSlogQueue()
            buffer_data = slog_buffer.flush()
            records = buffer_data.records
            call_sites = buffer_data.call_sites

            self.assertEqual(3, len(records))

            record_0 = json.loads(str(records[0]))
            call_site = call_sites[int(record_0['call_site_id'])]
            self.assertTrue(call_site.file().endswith('src/slog_test.py'))
            self.assertEqual(105, call_site.line())
            self.assertEqual('test_scope_decorator', call_site.function())
            self.assertEqual(4, len(record_0['tags']))
            self.assertEqual('.scope_name', record_0['tags'][0]['key'])
            self.assertEqual('decorated_f', record_0['tags'][0]['valueString'])

            record_1 = json.loads(str(records[1]))
            self.assertEqual(1, len(record_1['tags']))
            self.assertEqual('_t0_s', record_1['tags'][0]['key'])
            self.assertEqual('msg: msg1', record_1['tags'][0]['valueString'])

            record_2 = json.loads(str(records[2]))
            self.assertEqual(0, int(record_2['call_site_id']))
            self.assertEqual(2, len(record_2['tags']))

    def test_scope_with_exception(self):
        with SlogBuffer() as slog_buffer:
            try:
                with slog.scope('fooz_scope'):
                    raise Exception('foo-error')
                    slog.info(tags={'foo_tag': 'bar'})
            except Exception as e:
                print('Exception could be handled here: {}'.format(e))
                pass

            slog_buffer.waitSlogQueue()
            buffer_data = slog_buffer.flush()
            records = buffer_data.records
            call_sites = buffer_data.call_sites

            self.assertEqual(2, len(records))

            record_0 = json.loads(str(records[0]))
            call_site = call_sites[int(record_0['call_site_id'])]
            self.assertTrue(call_site.file().endswith('src/slog_test.py'))
            self.assertEqual(135, call_site.line())
            self.assertEqual('test_scope_with_exception', call_site.function())
            self.assertEqual(4, len(record_0['tags']))
            self.assertEqual('.scope_name', record_0['tags'][0]['key'])
            self.assertEqual('fooz_scope', record_0['tags'][0]['valueString'])

            record_1 = json.loads(str(records[1]))
            self.assertEqual(0, int(record_1['call_site_id']))
            self.assertEqual(2, len(record_1['tags']))


if __name__ == '__main__':
    unittest.main()
