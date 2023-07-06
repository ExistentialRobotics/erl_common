from tqdm import tqdm
from typing import Any
from typing import Optional

__all__ = ["dict_to_obj_attrs"]


def _set_object_attributes(
    obj: Any,
    attributes: dict,
    special_setters: Optional[dict],
    level: int,
    verbose: bool,
) -> None:
    if special_setters is None:
        special_setters = dict()

    for key, value in attributes.items():

        if isinstance(value, dict):
            if verbose:
                tqdm.write("    " * level + f"{key}:")
            _set_object_attributes(
                getattr(obj, key),
                value,
                special_setters.get(key, None),
                level + 1,
                verbose,
            )
            continue

        if key in special_setters:
            value = special_setters[key](value)
        setattr(obj, key, value)
        if verbose:
            tqdm.write("    " * level + f"Set {key} to {value}")


def dict_to_obj_attrs(
    obj: Any,
    attributes: dict,
    special_setters: Optional[dict] = None,
    verbose: bool = False,
) -> None:
    if special_setters is None:
        special_setters = dict()

    _set_object_attributes(obj, attributes, special_setters, 0, verbose)
