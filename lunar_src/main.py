from lunar_python import Lunar
import sys
import sxtwl
import argparse
parser = argparse.ArgumentParser(description='lunar')
# 这是输入日期（阳历）
parser.add_argument('arg1', help='year')
parser.add_argument('arg2', help='month')
parser.add_argument('arg3', help='day')
parser.add_argument('arg4', help='hours')
parser.add_argument('arg5', help='minutes')
parser.add_argument('arg6', help='seconds')

# 解析命令行参数

args = parser.parse_args()

if __name__ =='__main__':
    sys.stdout.reconfigure(encoding='utf-8')
    day = sxtwl.fromSolar(int(args.arg1), int(args.arg2), int(args.arg3))
    lunar = Lunar.fromYmdHms(day.getLunarYear(),day.getLunarMonth(), day.getLunarDay(),int(args.arg4), int(args.arg5), int(args.arg6))
    print(lunar.toFullString())