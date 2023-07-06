from typing import Callable
from typing import List

import vedo


class ButtonGroup:
    class CallbackWrapper:
        def __init__(self, func: Callable, *args, **kwargs) -> None:
            self.func = func
            self.args = args
            self.kwargs = kwargs

        def __call__(self) -> None:
            self.func(*self.args, **self.kwargs)

    def __init__(
        self,
        plt: vedo.Plotter,
        at: int,
        button_titles: List[str],
        pos: List[float],
        size: int,
        exclusive: bool,
        init_on_indices: List[int],
        callback: Callable,
    ) -> None:
        x, y = pos
        self.dy = 0.026
        y += (len(button_titles) - 1) * self.dy
        plt.at(at)  # switch render
        self.buttons = [
            plt.add_button(
                self.CallbackWrapper(self._button_callback, i),
                states=[title, title],
                pos=[x, y - i * self.dy],
                size=size,
                font="courier",  # this font is of equal width
                bc=["dg", "gray"] if exclusive else ["db", "gray"],
            )
            for i, title in enumerate(button_titles)
        ]
        self.exclusive = exclusive
        self.callback = callback

        self.button_on_flags = [False] * len(button_titles)
        if self.exclusive:
            assert len(init_on_indices) == 1, "Require only one button that is ON initially when exclusive is True"
        for i in init_on_indices:
            self.button_on_flags[i] = True
        for i, flag in enumerate(self.button_on_flags):
            if not flag:
                self.buttons[i].switch()

    def _button_callback(self, index: int) -> None:
        if self.exclusive and self.button_on_flags[index]:
            return  # the button is already on, do not trig the callback again
        if self.exclusive:
            for i in range(len(self.buttons)):
                if i == index or self.button_on_flags[i]:
                    self.buttons[i].switch()
                    self.button_on_flags[i] = not self.button_on_flags[i]
        else:
            self.buttons[index].switch()
            self.button_on_flags[index] = not self.button_on_flags[index]
        self.callback(index)
