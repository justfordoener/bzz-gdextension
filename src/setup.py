from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        "bot_tester",
        [
            "binding.cpp",
            "move_calculation.cpp",
            "heuristic_bot.cpp",
            "bot_tester.cpp",
        ],
        include_dirs=[pybind11.get_include()],
        language="c++",
        extra_compile_args=["/O2"],
    ),
]

setup(
    name="bot_tester",
    version="0.1",
    ext_modules=ext_modules,
)
