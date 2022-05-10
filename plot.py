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

import warnings
from argparse import ArgumentParser
from functools import cmp_to_key
from itertools import chain
from os import walk, path, makedirs
from pathlib import Path
from re import match
from typing import List

from matplotlib import pyplot as plt
from matplotlib.lines import Line2D
from pytablewriter import MarkdownTableWriter, LatexTableWriter

warnings.filterwarnings("ignore")

# where to look for timing results
RESULTS_DIR = './results'

# custom sort order for data sizes smallest -> largest
SIZES = ["MINI", "SMALL", "MEDIUM", "LARGE", "EXTRALARGE", 'STANDARD']
COMPACT_SZ = ["XS", "S", "M", "L", "XL", "STD"]

# directory sorting in tables left -> right
SOURCES = ['original', "original_autopar", "fission_autopar",
           "fission_manual", "case_study-a", "case_study-b"]
COMPACT_SRC = ['og', "o.a", "f.a", "f.m", "a", "b"]
DIR_FILTER = ",".join(SOURCES[0:4])

# Configs for fixed plot/charts properties
BAR_COLORS = ["#005D80", '#009052', '#FEDB4D', '#E6793D', '#ff1744']
AXLINE = {'y': 1, 'color': '#777', 'lw': 1.5, 'ls': '-', 'zorder': 2}
BARS = {'edgecolor': "black", 'lw': 0.35, 'zorder': 3}
LEGEND = {'loc': 'upper left', 'handletextpad': -0.1,
          'bbox_to_anchor': (0, 1.0, .5, 0.08), 'frameon': False,
          'columnspacing': .25, 'borderpad': 0, 'ncol': 4}
LSYMBOL = {'marker': 's', 'lw': 0, 'markersize': 4}
SPLOT = {'dpi': 300}
YGRID = {'ls': 'dotted', 'zorder': 0, 'lw': .75}


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
        "--show",
        action='store_true',
        help="show generated plot or table"
    )
    parser.add_argument(
        "--dir_filter",
        action='store',
        help="Comma separated list of directories to consider "
        f'[default: {DIR_FILTER}]'
    )
    parser.add_argument(
        "--prog_filter",
        action='store',
        help="Comma separated list of programs to consider [default: *]"
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
                 time_millis, digits, pfilter, show):
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
        self.programs = [p for p in sorted(list(set(
            chain.from_iterable([r.programs for r in results]))))
                         if not pfilter or p in pfilter]

        self.millis = time_millis
        self.digits = digits
        self.ensure_out_dir(out_dir)
        self.out_dir = out_dir
        self.pfilter = pfilter
        self.show = show

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

    @property
    def prog_count(self):
        return len(self.programs)

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
    def to_float(value):
        return float(value) if match(r'^-?\d+(?:\.\d+)$', value) else 0

    @staticmethod
    def max_value(arr):
        mx_num = max([j for sub in [
            [ResultPresenter.to_float(e) for e in r]
            for r in arr[2:]] for j in sub])
        return max(1, int(-1 * mx_num // 1 * -1))

    @staticmethod
    def color(index):
        return BAR_COLORS[index % len(BAR_COLORS)]

    @staticmethod
    def write_table(data, fmt, fn, out_dir, show):
        writer, ext = MarkdownTableWriter, 'md'
        headers, values = data[0], data[1:]

        # remove line w/ dir names if all values are the same
        if len(list(set([v for v in values[0] if len(v) > 0]))) == 1:
            values = values[1:]

        if fmt == "tex":
            writer, ext = LatexTableWriterExt, 'tex'

        fn = fn if fn and len(fn) > 0 else 'result'
        f_path = path.join(out_dir, f'{fn}.{ext}')
        w = writer(headers=headers, value_matrix=values)
        w.dump(f_path)
        if show: w.write_table()
        print(f'Wrote result to: {f_path}')

    def generate_table(self, sources, value_func, compact=False):
        ops, prs, szs = self.opt_levels, self.programs, self.data_sizes
        lp, ld, lo, ls = len(prs), len(szs), len(ops), len(sources)

        # fill initial header rows
        opts = [ops[(c // ls) % lo] for c in range(lo * ls)]
        srcs = [sources[ci % ls] for ci in range(lo * ls)]
        if compact:
            srcs = [COMPACT_SRC[SOURCES.index(s)] for s in srcs]
        table = [['Program', ('Size' if compact else 'Data Size')]
                 + opts, ['', ''] + srcs]

        for ri in range(lp * ld):
            row = []
            p = prs[ri // ld]  # which program
            d = szs[ri % ld]  # which data size
            for ci in range(-2, lo * ls):
                if ci == -2:
                    row.append(p if ri % ld == 0 else '')
                elif ci == -1:
                    row.append(COMPACT_SZ[SIZES.index(d)]
                               if compact else d)
                else:
                    o = ops[(ci // ls) % lo]  # which opt
                    s = sources[ci % ls]  # which source
                    row.append(value_func(p, d, o, s))
            table.append(row)
        return table

    def times(self, fmt):
        vf = lambda p, d, o, s: self.time_str(
            self.query(o, d, s).get_time(p))
        table = self.generate_table(self.sources, vf)

        if fmt == "plot":
            fn = lambda x: "time_" + x
            label = f'clock time ({"ms" if self.millis else "s"})'
            self.plot(table, fn, self.sources, label, True)
        else:
            fn = "time_" + ("-".join(self.sources).lower())
            self.write_table(table, fmt, fn, self.out_dir, self.show)

    def speedup(self, fmt, baseline, target):
        src_len, r = len(self.sources), RESULTS_DIR
        src_error = f'speedup requires timing at least two groups of ' \
            f'programs, found {src_len} matching plot criteria'
        bl_error = f'timing results not found for {baseline} in {r} '

        if src_len < 2:
            return print(src_error)
        if baseline not in self.sources:
            return print(bl_error)

        bi = self.sources.index(baseline)
        s1 = self.sources[bi]
        sp = [n for i, n in enumerate(self.sources) if
              (i != bi and (not target or target == n))]

        def value_func(p, d, o, s2):
            ts = self.query(o, d, s1).get_time(p)  # sequential
            tp = self.query(o, d, s2).get_time(p)  # parallel
            speedup = ts / tp if (ts and tp and tp > 0) else 0
            return self.time_str(speedup, scale=False)

        table = self.generate_table(sp, value_func, compact=True)
        if fmt == "plot":
            fn = lambda x: f'speedup_{baseline}-{x}'
            self.plot(table, fn, sp, "speedup", False)
        else:
            fn = "speedup_" + ("-".join([baseline, target or 'all']))
            self.write_table(table, fmt, fn, self.out_dir, self.show)

    def plot(self, data, fn, prog_dir, ylabel, log):
        rows, cols = min(self.prog_count, 2), min(self.prog_count, 3)
        lbls = [COMPACT_SZ[SIZES.index(sz)] for sz in self.data_sizes]
        bars = [data[0].index(o) for o in self.opt_levels]
        plt.rc('font', **{'size': 18 if self.prog_count == 1 else 8})
        plt.rc('legend', fontsize=15 if self.prog_count == 1 else 6)
        ymin, ymax = 0, self.max_value(data)

        # draw a figure for each program directory
        for ci, target_name in enumerate(prog_dir):
            fig, axs = plt.subplots(**SPLOT, nrows=rows, ncols=cols)
            axs = axs.flatten() if self.prog_count > 1 else [axs]

            # draw a sub plot for each program
            for sub_plot, prog_name in zip(axs, self.programs):

                # draw the various bars
                ll, lb = len(lbls), len(bars)
                bw, x_off = 0.80 / lb, (lb / 2) - (1 / lb)
                y1, lines = [d[0] for d in data].index(prog_name), []
                for i, bi in enumerate([o + ci for o in bars]):
                    pos = [a + ((i - x_off) * bw) for a in range(ll)]
                    c, dr = self.color(i), data[y1:y1 + ll]
                    values = [self.to_float(v[bi]) for v in dr]
                    sub_plot.bar(pos, values, color=c, width=bw, **BARS)
                    lines.append(Line2D([0], [0], color=c, **LSYMBOL))
                sub_plot.legend(lines, self.opt_levels[:], **LEGEND)

                # format y axis scale
                if log:
                    sub_plot.set_yscale('log')
                else:
                    sub_plot.axhline(**AXLINE)
                    sub_plot.locator_params(axis='y', nbins=5)
                    sub_plot.set_ylim((ymin, ymax))

                # format rest of the plot
                sub_plot.yaxis.grid(True, **YGRID)
                sub_plot.set_ylabel(ylabel)
                sub_plot.set_xlabel(prog_name, labelpad=0)
                sub_plot.set_xticks(range(len(lbls)), lbls)
                sub_plot.tick_params(axis="y", direction="inout")
                sub_plot.tick_params(axis="x", length=0, pad=4)
                sub_plot.spines['right'].set_visible(False)
                sub_plot.spines['top'].set_visible(False)
                sub_plot.spines['bottom'].set_visible(False)
                sub_plot.margins(0.05)

            # if there are fewer programs, clear the overestimate
            for idx in range(self.prog_count, rows * cols):
                fig.delaxes(axs[idx])

            fig.tight_layout()
            fig.subplots_adjust(wspace=.4 if log else .25, hspace=.3)
            fig_name = f'{fn(target_name) or "plot"}.pdf'
            f_path = path.join(self.out_dir, fig_name)
            plt.savefig(f_path)
            print(f'Saved plot to to: {f_path}')
            if self.show: plt.show()


if __name__ == '__main__':

    args = setup_args().parse_args()

    dir_fil = [d.strip() for d in args.dir_filter.split(",")] \
        if args.dir_filter else DIR_FILTER
    prog_fil = [p.strip() for p in args.prog_filter.split(",")] \
        if args.prog_filter else None

    rp = ResultPresenter(
        results=parse_results(RESULTS_DIR, dir_fil),
        out_dir=args.out,
        time_millis=args.millis,
        digits=args.digits,
        pfilter=prog_fil,
        show=args.show)

    # only plot speedup for now
    if args.data == "speedup":
        rp.speedup(args.fmt, args.ss, args.st)
    else:
        rp.times(args.fmt)
