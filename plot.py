from argparse import ArgumentParser
from functools import cmp_to_key
from itertools import chain
from os import walk, path, getcwd
from pathlib import Path
from typing import List

RESULTS_DIR = './results'
SIZES = ["MINI", "SMALL", "MEDIUM", "LARGE", "EXTRALARGE", 'STANDARD']


def read_file(file_path):
    with open(file_path, 'r') as fp:
        return fp.readlines()


def write_file(file_path, lines):
    with open(file_path, 'w') as fp:
        fp.write(lines)


def parse_results(result_dir):
    def rf(file_in, parser):
        return parser(read_file(path.join(result_dir, file_in))) \
            if file_in else None

    def find_model(fn, models_):
        stem = Path(fn).stem
        return next(filter(lambda x: x.startswith(stem), models_), None)

    def format_time(fn, variance, time):
        return fn.replace('_time', ''), float(variance), float(time)

    def parse_times(raw_times):
        return [format_time(*rt.split(None, 2)) for rt in raw_times]

    def parse_model(raw_model):
        return {tup[0]: tup[1:] for tup in [
            (k.strip(), v.strip()) for (k, v) in
            [l.split(':', 1) for l in raw_model
             if ':' in l]]} if raw_model is not None else None

    def parse_(timing, model):
        return Timing(rf(timing, parse_times), **rf(model, parse_model))

    filenames = next(walk(result_dir), (None, None, []))[2]
    models = [f for f in filenames if 'model' in f]

    return ResultPresenter(
        [parse_(*pair) for pair in
         [(fn, find_model(fn, models)) for fn in  # time, model pairs
          [f for f in filenames if f not in models]]])  # times


class Timing:
    def __init__(self, times: list = None, **kwargs):
        unpack = lambda key: kwargs[key][0] if key in kwargs else None
        self.times = times or []
        self.programs = [t[0] for t in self.times]
        self.compiler = unpack('compiler')
        self.opt_level = unpack('opt level')
        self.data_size = unpack('data size')
        self.source = unpack('source')

    def find(self, opt, data_size, source):
        return self.source == source and \
               self.opt_level == opt and \
               self.data_size == data_size

    def get_time(self, program):
        if program in self.programs:
            return self.times[self.programs.index(program)][-1]


class ResultPresenter:

    def __init__(self, results: List[Timing]):
        self.__results = results
        self.opt_levels = sorted(
            list(set([r.opt_level for r in results])))
        self.data_sizes = sorted(
            list(set([r.data_size for r in results])),
            key=cmp_to_key(ResultPresenter.data_size_sort))
        self.sources = sorted(
            list(set([r.source for r in results])))
        self.programs = sorted(list(set(
            chain.from_iterable([r.programs for r in results]))))

    @staticmethod
    def data_size_sort(x, y):
        if x in SIZES and y in SIZES:
            xi, yi = SIZES.index(x), SIZES.index(y)
            return -1 if xi < yi else 0 if xi == yi else 1
        else:
            1

    def query(self, opt, size, source):
        return next(filter(
            lambda x: x.find(opt, size, source),
            self.__results), Timing())

    def __make_table(self):
        lp, ld = len(self.programs), len(self.data_sizes)
        lo, ls = len(self.opt_levels), len(self.sources)
        opts = [self.opt_levels[(c // ls) % lo] for c in range(lo * ls)]
        srcs = [self.sources[ci % ls] for ci in range(lo * ls)]
        table = [['Program', 'Data size'] + opts, ['', ''] + srcs]

        for ri in range(lp * ld):
            row = []
            p = self.programs[ri // ld]  # which program
            d = self.data_sizes[ri % ld]  # which data size
            for ci in range(-2, lo * ls):
                if ci == -2:
                    row.append(p if ri % ld == 0 else '')
                elif ci == -1:
                    row.append(d)
                else:
                    o = self.opt_levels[(ci // ls) % lo]
                    s = self.sources[ci % ls]
                    row.append(self.query(o, d, s).get_time(p))
            table.append(row)

        pad = len(max(self.programs + self.data_sizes, key=len))
        return table, pad

    def __speedup_table(self):
        lp, ld = len(self.programs), len(self.data_sizes)
        lo, ls = len(self.opt_levels), 1
        opts = [self.opt_levels[c % lo] for c in range(lo)]
        table = [['Program', 'Data size'] + opts]

        for ri in range(lp * ld):
            row = []
            p = self.programs[ri // ld]  # which program
            d = self.data_sizes[ri % ld]  # which data size
            for ci in range(-2, lo * ls):
                if ci == -2:
                    row.append(p if ri % ld == 0 else '')
                elif ci == -1:
                    row.append(d)
                else:
                    o = self.opt_levels[(ci // ls) % lo]
                    [s1, s2] = self.sources[0:2]
                    t1 = self.query(o, d, s1).get_time(p)
                    t2 = self.query(o, d, s2).get_time(p)
                    row.append(round(t1 / t2, 2))
            table.append(row)

        pad = len(max(self.programs + self.data_sizes, key=len))
        return table, pad

    def __to_markdown(self, table, pad):
        text = []
        table.insert(1, ['-' * pad] * len(table[0]))
        for r in table:
            row = f' | '.join([
                str(round(c, pad - 3)).ljust(pad, ' ')
                if isinstance(c, float)
                else str(c).ljust(pad, ' ') for c in r])
            text.append("| " + row + " |")
        text = "\n".join(text)
        self.save(text, 'md')

    def __to_tex(self, table, pad):
        text = []
        for r in table:
            row = f' & '.join([
                str(round(c, pad - 3)).ljust(pad, ' ')
                if isinstance(c, float)
                else str(c).ljust(pad, ' ') for c in r])
            text.append(row + "\\\\")
        text.insert(1, '\\hline')
        text = "\n".join(text)
        self.save(text, 'txt')

    def times_to_markdown(self):
        self.__to_markdown(*self.__make_table())

    def times_to_tex(self):
        self.__to_tex(*self.__make_table())

    def speedup_to_markdown(self):
        if len(self.sources) != 2:
            return print('speedup assumes two source directories')
        self.__to_markdown(*self.__speedup_table())

    def speedup_to_tex(self):
        if len(self.sources) != 2:
            return print('speedup assumes two source directories')
        self.__to_tex(*self.__speedup_table())

    @staticmethod
    def save(content, extension='txt'):
        file_name = path.join(getcwd(), f'result.{extension}')
        write_file(file_name, content)
        print(f'Wrote result to: {file_name}')


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument(
        "--speedup",
        action='store_true',
        help="skip writing result to file"
    )
    parser.add_argument(
        "--tex",
        action='store_true',
        help="skip writing result to file"
    )
    args = parser.parse_args()
    rp = parse_results(RESULTS_DIR)

    if args.speedup:
        if args.tex:
            rp.speedup_to_tex()
        else:
            rp.speedup_to_markdown()
    else:
        if args.tex:
            rp.times_to_tex()
        else:
            rp.times_to_markdown()
