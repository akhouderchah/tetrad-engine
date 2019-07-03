from setuptools import setup

setup(name='tetrad_build_tool',
      version='0.1',
      description='Tool to lex C++ projects and provide reflection functionality.',
      author='Alex Khouderchah',
      author_email='alex.khouderchah@gmail.com',
      license='Apache-2.0',
      packages=['build_tool'],
      install_requires=[
          'protobuf'
      ])
