"""
Utility script for formatting results. (Python 3+).
It creates tables and plots of the results.

Usage:

```
python3 plot.py
```

List of options:

```
python3 plot.py --help
```
"""

from argparse import ArgumentParser
from functools import cmp_to_key
from itertools import chain
from math import ceil
from os import walk, path, makedirs
from pathlib import Path
from typing import List

import numpy as np
from matplotlib import pyplot as plt
from matplotlib.lines import Line2D
from pytablewriter import MarkdownTableWriter, LatexTableWriter

# where to look for timing results
RESULTS_DIR = './results'

# custom sort order for data sizes smallest -> largest
SIZES = ["MINI", "SMALL", "MEDIUM", "LARGE", "EXTRALARGE", 'STANDARD']
COMPACT_SZ = ["XS", "S", "M", "L", "XL", "STD"]

# directory sorting in tables left -> right
SOURCES = ['original', "original_autopar", "fission_autopar",
           "fission_manual", "case_norm", "case_alt"]
DIR_FILTER = ",".join(SOURCES[0:4])

# Configs for fixed plot/charts properties
BAR_COLORS = ["#005D80", '#009052', '#FEDB4D', '#E6793D', '#ff1744']
AXLINE = {'y': 1, 'color': '#777', 'lw': 1.5, 'ls': '-', 'zorder': 0}
plt.rc('font', **{'size': 8})
plt.rc('legend', fontsize=6)


def read_file(file_path):
    """Basic file read, by line"""
    with open(file_path, 'r', errors='replace') as fp:
        return fp.readlines()


def setup_args():
    parser = ArgumentParser()
    parser.add_argument(
        "-d", "--data",
        action='store',
        default='time',
        help="data choice: {time, speedup}"
    )
    parser.add_argument(
        "-o", "--out",
        action='store',
        default='plots',
        help="output directory"
    )
    parser.add_argument(
        "-f", "--fmt",
        action="store",
        default="md",
        help="output format: {tex, md, plot}"
    )
    parser.add_argument(
        "--ss",
        action="store",
        default="original",
        help="source directory for speedup [default: original]"
    )
    parser.add_argument(
        "--st",
        action="store",
        help="target directory for speedup [default: *]"
    )
    parser.add_argument(
        "--millis",
        action='store_true',
        help="display table of times in milliseconds, not seconds"
    )
    parser.add_argument(
        '--digits',
        type=int,
        choices=range(0, 15),
        metavar="[0-15]",
        help='number of digits for tabular values [default: 6]',
        default=6)
    parser.add_argument(
        "--dir_filter",
        action='store',
        help="Comma separated list of directories to consider "
        f'[default: {DIR_FILTER}]'
    )
    return parser


def parse_results(result_dir, dir_filter):
    """Make a data object from the captured results"""

    def rf(file_in, parser_func):
        return parser_func(read_file(path.join(result_dir, file_in))) \
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
        tm = Timing(rf(timing, parse_times), **rf(model, parse_model))
        return tm if tm.source in dir_filter else None

    # get a list of all files in results directory
    filenames = next(walk(result_dir), (None, None, []))[2]
    models = [f for f in filenames if f.endswith('model.txt')]

    # pair the results with their model; the finally remove nulls
    return [p for p in
            [parse_(*pair) for pair in
             [(fn, find_model(fn, models)) for fn in
              [f for f in filenames if f not in models]]] if p]


class LatexTableWriterExt(LatexTableWriter):
    """Overrides for LaTeX table writer behavior"""

    def _get_opening_row_items(self) -> List[str]:
        return ["".join([
            r"\begin{tabular}{",
            "{:s}".format(
                " | ".join(self._get_col_align_char_list())),
            r"} \hline", ])]

    def _get_closing_row_items(self) -> List[str]:
        return [r"\end{tabular}"]


class Timing:
    """Model for a single timing result."""

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
    """Represents a collection of results, and offers some formatting
    options """

    def __init__(self, results: List[Timing], out_dir,
                 time_millis, digits):
        self.__results = results

        # list of all (unique) recorded optimization levels
        self.opt_levels = sorted(
            list(set([r.opt_level for r in results])))

        # list of all (unique) recorded data sizes
        self.data_sizes = sorted(
            list(set([r.data_size for r in results])),
            key=cmp_to_key(ResultPresenter.data_size_sort))

        # list of source directories
        self.sources = sorted(
            list(set([r.source for r in results])),
            key=cmp_to_key(ResultPresenter.sources_sort))

        # list of names of benchmarked programs
        self.programs = sorted(list(set(
            chain.from_iterable([r.programs for r in results]))))

        self.millis = time_millis
        self.digits = digits
        self.ensure_out_dir(out_dir)
        self.out_dir = out_dir

    def time_str(self, t, scale=True):
        if not t:
            return '-'
        ms, d = self.millis, self.digits

        if scale:
            t = t if not ms else t * 1000
        cap_len = len(str(int(t)))
        dig_len = d + (1 if d > 0 else 0)

        return format(t, f'.{d}f')[:cap_len + dig_len]

    def query(self, opt, size, source):
        """Find timing result by given parameters."""
        return next(filter(lambda x: x.find(opt, size, source),
                           self.__results), Timing())

    @staticmethod
    def ensure_out_dir(dir_path):
        return path.exists(dir_path) or makedirs(dir_path)

    @staticmethod
    def custom_sort(x, y, src_arr):
        """Determine custom sort order of x and y."""
        xi = src_arr.index(x) if x in src_arr else 1
        yi = src_arr.index(y) if y in src_arr else 1
        return -1 if xi < yi else 0 if xi == yi else 1

    @staticmethod
    def data_size_sort(x, y):
        return ResultPresenter.custom_sort(x, y, SIZES)

    @staticmethod
    def sources_sort(x, y):
        return ResultPresenter.custom_sort(x, y, SOURCES)

    @staticmethod
    def write_table(data, fmt, fn, out_dir):
        writer, ext = MarkdownTableWriter, 'md'
        headers, values = data[0], data[1:]
        # remove line w/ dir names if all values are the same
        if len(list(set([v for v in values[0] if len(v) > 0]))) == 1:
            values = values[1:]
        if fmt == "tex":
            writer, ext = LatexTableWriterExt, 'tex'
        fn = fn if fn and len(fn) > 0 else 'result'
        f_path = path.join(out_dir, f'{fn}.{ext}')
        writer(headers=headers, value_matrix=values).dump(f_path)
        print(f'Wrote result to: {f_path}')

    def time_table(self):
        """Generates a 2d array of recorded times."""
        lp, ld = len(self.programs), len(self.data_sizes)
        lo, ls = len(self.opt_levels), len(self.sources)

        # fill initial header rows
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
                    t = self.time_str(self.query(o, d, s).get_time(p))
                    row.append(t)
            table.append(row)
        return table

    def speedup_table(self, baseline, target=None):
        # fix original vs. parallel source for computing speedup
        base_i = self.sources.index(baseline)
        s1 = self.sources[base_i]
        sp = [n for i, n in enumerate(self.sources) if
              (i != base_i and (not target or target == n))]

        lp, ld = len(self.programs), len(self.data_sizes)
        lo, ls = len(self.opt_levels), len(sp)
        opts = [self.opt_levels[(c // ls) % lo] for c in range(lo * ls)]
        srcs = [sp[ci % ls] for ci in range(lo * ls)]

        # initial headers row
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
                    s2 = srcs[ci % ls]
                    ts = self.query(o, d, s1).get_time(p)  # sequential
                    tp = self.query(o, d, s2).get_time(p)  # parallel
                    speedup = ts / tp if (ts and tp and tp > 0) else 0
                    row.append(self.time_str(speedup, scale=False))
            table.append(row)
        return table

    def times(self, fmt):
        fn = "-".join(self.sources).lower()
        self.write_table(self.time_table(), fmt, fn, self.out_dir)

    def speedup(self, fmt, baseline, target):
        src_len, r = len(self.sources), RESULTS_DIR
        src_error = f'speedup requires timing at least two groups of ' \
            f'programs, found {src_len} matching plot criteria'
        bl_error = f'timing results not found for {baseline} in {r} '

        if src_len < 2:
            return print(src_error)
        if baseline not in self.sources:
            return print(bl_error)

        fn = "-".join([baseline, target or 'all'])
        data = self.speedup_table(baseline, target)

        if fmt == "plot":
            self.plot(data, baseline)
        else:
            self.write_table(data, fmt, fn, self.out_dir)

    def plot(self, data, fn):
        p_count = len(self.programs)
        rows, cols = min(p_count, 2), min(p_count, 3)
        colors = BAR_COLORS
        subplots = min(p_count, rows * cols)
        labels = [COMPACT_SZ[SIZES.index(sz)] for sz in self.data_sizes]
        src_len = (len(data[0]) - 2) // len(self.opt_levels)
        y_label = "Speedup"

        bars = [(data[0].index(o), o) for o in self.opt_levels]
        lx, w = np.arange(len(labels)), 0.80 / len(bars)
        x_adjust = (len(bars) / 2) - (1 / len(bars))

        y_min, y_max = 0, max(1, ceil(np.amax(
            [[(float(c) if c and c != '-' else 1)
              for c in r[2:]] for r in data[2:]])))
        bar_x = [lx + ((i - x_adjust) * w) for i in range(0, len(bars))]
        bar_props = {'edgecolor': "black", 'lw': 0.35, 'width': w}
        sub_props = {'nrows': rows, 'ncols': cols, 'dpi': 300}

        for src_i in range(src_len):
            data_cols = [o + src_i for (o, _) in bars]
            col_name = data[1][data_cols[0]]
            fig, axs = plt.subplots(**sub_props)

            if p_count > 1:
                axs = axs.flatten()
            for p in range(subplots):
                subplt = axs[p] if p_count > 1 else axs
                p_name = self.programs[p]
                subplt.axhline(**AXLINE)

                ri = [d[0] for d in data].index(p_name)
                ry = ri + len(labels)

                for i, bi in enumerate(data_cols):
                    series = [float(v[bi]) if v[bi] != '-'
                              else 0 for v in data[ri:ry]]
                    subplt.bar(bar_x[i], series,
                               color=colors[i % len(colors)],
                               **bar_props)

                subplt.set_ylim((y_min, y_max))
                subplt.set_ylabel(y_label)
                subplt.set_xticks(lx, labels)
                subplt.set_xlabel(p_name)
                subplt.tick_params(axis="y", direction="inout")
                subplt.tick_params(axis="x", length=0, pad=4)
                subplt.spines['right'].set_visible(False)
                subplt.spines['top'].set_visible(False)
                subplt.spines['bottom'].set_visible(False)
                subplt.legend([
                    Line2D([0], [0], marker='s', lw=0,
                           color=colors[i % len(colors)],
                           markersize=4) for (i, _) in
                    enumerate(self.opt_levels)],
                    self.opt_levels[:],
                    loc='upper left',
                    handletextpad=-0.1,
                    bbox_to_anchor=(0, 1.05, .5, 0.08),
                    frameon=False,
                    columnspacing=.25,
                    borderpad=0,
                    ncol=4)

                subplt.margins(0.05)

            # if there are fewer programs, clear the overestimate
            for idx in range(len(self.programs), rows * cols):
                fig.delaxes(axs[idx])

            plt.tight_layout()
            plt_path = path.join(self.out_dir, f'{fn}-{col_name}.pdf')
            plt.savefig(plt_path)
            plt.show()


if __name__ == '__main__':

    args = setup_args().parse_args()

    dir_fil = [d.strip() for d in args.dir_filter.split(",")] \
        if args.dir_filter else DIR_FILTER

    rp = ResultPresenter(
        results=parse_results(RESULTS_DIR, dir_fil),
        out_dir=args.out,
        time_millis=args.millis,
        digits=args.digits)

    # only plot speedup for now
    if args.fmt == "plot" or args.data == "speedup":
        rp.speedup(args.fmt, args.ss, args.st)
    else:
        rp.times(args.fmt)
