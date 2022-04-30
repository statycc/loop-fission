from os import walk, path
from itertools import chain
from pathlib import Path
from typing import List

RESULTS_DIR = './results'


def read_file(file_path):
    with open(file_path, 'r') as fp:
        return fp.readlines()


class Timing:
    def __init__(self, times: list, **kwargs):
        unpack = lambda key: kwargs[key][0] if key in kwargs else None
        self.times = times
        self.programs = [t[0] for t in times]
        self.compiler = unpack('compiler')
        self.opt_level = unpack('opt level')
        self.data_size = unpack('data size')
        self.source = unpack('source')

    def __str__(self):
        return f'{self.compiler} {self.opt_level} ' \
            f'{self.data_size} {self.source}\n{self.times}'


class ResultPresenter:

    def __init__(self, results: List[Timing]):
        self.__results = results
        self.opt_levels = list(set([r.opt_level for r in results]))
        self.data_sizes = list(set([r.data_size for r in results]))
        self.sources = list(set([r.source for r in results]))
        self.programs = list(set(chain.from_iterable(
            [r.programs for r in results])))

        print('opt levels: ', self.opt_levels)
        print('data sizes: ', self.data_sizes)
        print('sources: ', self.sources)
        print('programs: ', self.programs)


def parse_results(result_dir):
    def rf(file_in, parser):
        return parser(read_file(path.join(result_dir, file_in))) \
            if file_in else None

    def find_model(fn, all_models):
        return next(filter(
            lambda x: x.startswith(Path(fn).stem), all_models), None)

    def format_time(fn, variance, time):
        return fn.replace('_time', ''), float(variance), float(time)

    def parse_times(raw_times):
        return [format_time(*rt.split(None, 2)) for rt in raw_times]

    def parse_model(raw_model):
        return {tup[0]: tup[1:] for tup in [
            (k.strip(), v.strip()) for (k, v) in
            [l.strip().split(':', 1) for l in raw_model
             if ':' in l]]} if raw_model is not None else None

    def parse_(timing, model):
        return Timing(rf(timing, parse_times), **rf(model, parse_model))

    filenames = next(walk(result_dir), (None, None, []))[2]
    models = [f for f in filenames if 'model' in f]

    return ResultPresenter(
        [parse_(*pair) for pair in
         [(fn, find_model(fn, models)) for fn in  # time, model pairs
          [f for f in filenames if f not in models]]])  # times


if __name__ == '__main__':
    rp = parse_results(RESULTS_DIR)
