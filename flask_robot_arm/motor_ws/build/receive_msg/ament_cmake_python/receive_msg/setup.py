from setuptools import find_packages
from setuptools import setup

setup(
    name='receive_msg',
    version='0.0.0',
    packages=find_packages(
        include=('receive_msg', 'receive_msg.*')),
)
