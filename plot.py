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
from os import walk, path, getcwd
from pathlib import Path
from typing import List

import numpy as np
from matplotlib import pyplot as plt
from matplotlib.lines import Line2D

# where to look for timing results
RESULTS_DIR = './results'

# name of original examples directory, for computing speedup
SEQ_TIME = 'original'

# custom sort order for data sizes smallest -> largest
SIZES = ["MINI", "SMALL", "MEDIUM", "LARGE", "EXTRALARGE", 'STANDARD']
COMPACT_SZ = ["XS", "S", "M", "L", "XL", "STD"]

# directory sorting in tables left -> right
SOURCES = [SEQ_TIME, "original_autopar", "fission_autopar",
           "fission_manual"]

# Configs for plot/charts
BAR_COLORS = ["#005D80", '#009052', '#FEDB4D', '#E6793D', '#073b4c']
plt.rc('axes', labelsize=8)
plt.rc('xtick', labelsize=8)
plt.rc('ytick', labelsize=8)
plt.rc('legend', fontsize=6)


def read_file(file_path):
    """Basic file read, by line"""
    with open(file_path, 'r') as fp:
        return fp.readlines()


def write_file(file_path, lines):
    """Basic file write, lines of text"""
    with open(file_path, 'w') as fp:
        fp.write(lines)


def parse_results(result_dir):
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
        return Timing(rf(timing, parse_times), **rf(model, parse_model))

    filenames = next(walk(result_dir), (None, None, []))[2]
    models = [f for f in filenames if 'model' in f]

    return ResultPresenter(
        [parse_(*pair) for pair in
         [(fn, find_model(fn, models)) for fn in  # time, model pairs
          [f for f in filenames if f not in models]]])  # times


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

    def __init__(self, results: List[Timing]):
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

    def query(self, opt, size, source):
        """Find timing result by given parameters."""
        return next(filter(
            lambda x: x.find(opt, size, source),
            self.__results), Timing())

    @staticmethod
    def get_pad(matrix, col_index):
        col_vector = [str(row[col_index]) for row in matrix]
        return len(max(col_vector, key=len))

    @staticmethod
    def save(content, extension='txt'):
        file_name = path.join(getcwd(), f'result.{extension}')
        write_file(file_name, content)
        print(f'Wrote result to: {file_name}')

    def __times_table(self):
        """Generates a datatable of recorded times."""
        lp, ld = len(self.programs), len(self.data_sizes)
        lo, ls = len(self.opt_levels), len(self.sources)
        opts = [self.opt_levels[(c // ls) % lo] for c in range(lo * ls)]
        srcs = [self.sources[ci % ls] for ci in range(lo * ls)]

        # fill initial header rows
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
        return table

    def __speedup_table(self):
        lp, ld = len(self.programs), len(self.data_sizes)
        lo, ls = len(self.opt_levels), 1
        opts = [self.opt_levels[c % lo] for c in range(lo)]

        # initial headers row
        table = [['Program', 'Data size'] + opts]

        # fix original vs. parallel source for computing speedup
        og_index = self.sources.index(SEQ_TIME)
        s1 = self.sources[og_index]
        s2 = self.sources[0:2][(og_index + 1) % 2]

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
                    ts = self.query(o, d, s1).get_time(p)  # sequential
                    tp = self.query(o, d, s2).get_time(p)  # parallel
                    speedup = ts / tp if (ts and tp) else 0
                    row.append(str(round(speedup, 2)).ljust(4, '0'))
            table.append(row)
        return table

    def __out_formatted(self, data, fmt):
        if fmt == "plot":
            self.plot()
        elif fmt == "tex":
            self.to_tex(data)
        else:
            self.to_markdown(data)

    def to_markdown(self, table):
        text = []
        pads = [self.get_pad(table, i) for i in range(len(table[0]))]
        for r in table:
            row = f' | '.join([
                str(round(c, pads[i] - 3)).ljust(pads[i], ' ')
                if isinstance(c, float)
                else str(c).ljust(pads[i], ' ')
                for i, c in enumerate(r)])
            text.append("| " + row + " |")

        text.insert(1, "| " + (" | ".join(
            ['-' * pads[i] for i in range(len(table[0]))])) + " |")
        text = "\n".join(text)
        self.save(text, 'md')

    def to_tex(self, table):
        text = []
        pads = [self.get_pad(table, i) for i in range(len(table[0]))]

        for r in table:
            row = f' & '.join([
                str(round(c, pads[i] - 3)).ljust(pads[i], ' ')
                if isinstance(c, float)
                else str(c).ljust(pads[i], ' ')
                for i, c in enumerate(r)])
            text.append(row + "\\\\")

        text.insert(1, '\\hline')
        text = "\n".join(text)
        self.save(text, 'txt')

    def times(self, fmt):
        self.__out_formatted(self.__times_table(), fmt)

    def speedup(self, fmt):
        if len(self.sources) != 2:
            return print('speedup assumes two source directories')
        self.__out_formatted(self.__speedup_table(), fmt)

    def plot(self):
        p_count = len(self.programs)
        rows, cols = min(p_count, 2), min(p_count, 3)
        colors = BAR_COLORS
        fig_count = max(1, p_count // (rows * cols))
        subplots = min(p_count, rows * cols)
        labels = [COMPACT_SZ[SIZES.index(sz)] for sz in self.data_sizes]
        y_label = "Speedup"

        data = self.__speedup_table()
        bars = [(data[0].index(o), o) for o in self.opt_levels]
        lx, w = np.arange(len(labels)), 0.80 / len(bars)
        x_adjust = (len(bars) / 2) - (1 / len(bars))

        y_min, y_max = 0, max(4, ceil(np.amax(
            [[(float(c) if c else 1)
              for c in r[2:]] for r in data[1:]])))
        bar_x = [lx + ((i - x_adjust) * w) for i in range(0, len(bars))]
        bar_props = {'edgecolor': "black", 'lw': 0.35,
                     'width': w}

        for f in range(fig_count):
            fig, axs = plt.subplots(
                figsize=[7, 5], nrows=rows, ncols=cols, dpi=300)
            if p_count > 1:
                axs = axs.flatten()
            for p in range(subplots):
                subplt = axs[p] if p_count > 1 else axs

                p_name = self.programs[f * (rows * cols) + p]
                ri = [d[0] for d in data].index(p_name)
                ry = ri + len(labels)

                for i, (bi, label) in enumerate(bars):
                    subplt.bar(
                        bar_x[i], [float(v[bi]) for v in data[ri:ry]],
                        label=label, color=colors[i % len(colors)],
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
            plt.savefig(f'result_{f + 1}.pdf')
            plt.show()


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument(
        "-d", "--data",
        action='store',
        default='times',
        help="data choice: {time, speedup}"
    )
    parser.add_argument(
        "-f", "--fmt",
        action="store",
        default="markdown",
        help="output format: {tex, markdown, plot}"
    )

    args = parser.parse_args()
    rp = parse_results(RESULTS_DIR)

    if args.data == "speedup":
        rp.speedup(args.fmt)
    else:
        rp.times(args.fmt)
