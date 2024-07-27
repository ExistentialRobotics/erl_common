from . import storage
from . import vedo_ext
from . import dict_ext
from . import yaml
from enum import IntEnum

__all__ = [
    "set_global_random_seed",
    "storage",
    "vedo_ext",
    "dict_ext",
    "yaml",
    "Logging",
]

def set_global_random_seed(seed: int) -> None: ...

class Logging:
    class Level(IntEnum):
        kInfo = 0
        kDebug = 1
        kWarn = 2
        kError = 3
        kSilent = 4

    level: Level

    @staticmethod
    def get_date_str() -> str: ...
    @staticmethod
    def get_time_str() -> str: ...
    @staticmethod
    def get_date_time_str() -> str: ...
    @staticmethod
    def get_time_stamp() -> str: ...
