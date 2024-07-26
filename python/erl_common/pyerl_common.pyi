from . import storage
from . import vedo_ext
from . import dict_ext
from . import yaml
from enum import IntEnum

__all__ = [
    "storage",
    "vedo_ext",
    "dict_ext",
    "yaml",
    "manually_set_seed",
]

def manually_set_seed(seed: int) -> None: ...

class Logging:
    class Level(IntEnum):
        INFO = 0
        DEBUG = 1
        WARN = 2
        ERROR = 3
        SILENT = 4

    level: Level

    @staticmethod
    def get_date_str() -> str: ...
    @staticmethod
    def get_time_str() -> str: ...
    @staticmethod
    def get_date_time_str() -> str: ...
    @staticmethod
    def get_time_stamp() -> str: ...
