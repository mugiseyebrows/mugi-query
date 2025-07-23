import argparse
import pandas as pd
import datetime
import re
from pandas.api.types import is_string_dtype

from sqlalchemy import create_engine

parser = argparse.ArgumentParser(add_help=False)

parser.add_argument("--type", choices=['mysql'], required=True)
parser.add_argument("--user", required=True)
parser.add_argument("--password", required=True)
parser.add_argument("--host", required=True)
parser.add_argument("--database", required=True)
parser.add_argument("--port", default=3306)
parser.add_argument("--name", required=True, help='table name')
parser.add_argument("--fail", action='store_true')
parser.add_argument("--replace", action='store_true')
parser.add_argument("--append", action='store_true')
parser.add_argument("path")

args = parser.parse_args()

con = create_engine(f"mysql+mysqlconnector://{args.user}:{args.password}@{args.host}:{args.port}/{args.database}", echo=True)

kwargs = {'index': False}
if args.fail:
    kwargs['if_exists'] = 'fail'
elif args.replace:
    kwargs['if_exists'] = 'replace'
elif args.append:
    kwargs['if_exists'] = 'append'

df = pd.read_csv(args.path)

def parse_date(df, col, rs, fmts, N = 1000):
    if not is_string_dtype(df[col].dtype):
        return
    sample = df[col][:N].fillna('')
    n = len(sample)
    for r, fmt in zip(rs, fmts):
        count_empty = sum(sample.map(lambda x: x == ''))
        count_matched = sum(sample.map(lambda x: r.match(x) is not None))
        if count_matched > 0 and (count_matched + count_empty) == n:
            df[col] = pd.to_datetime(df[col], format=fmt)
            if '%H' not in fmt:
                df[col] = df[col].apply(lambda x: x.date())
            return

rs = [
    re.compile('^[0-3][0-9]\\.[0-1][0-9]\\.[0-9][0-9]$'),
    re.compile('^[0-9]{4}-[0-1][0-9]-[0-3][0-9]$'),
]

fmts = [
    '%d.%m.%y',
    '%Y-%m-%d'
]

cols = list(df.columns)
for col in cols:
    parse_date(df, col, rs, fmts)

df.to_sql(args.name, con, **kwargs)