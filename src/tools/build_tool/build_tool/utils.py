import os

class FileNameError(Exception):
    pass


def verify_dir(dirname: str) -> str:
    """
    Ensures that a provided directory name is valid

    @throw FileNameError if the provided name does not exist, or if it is not a
           directory
    @return the real path of the provided directory name
    """
    dirname = os.path.realpath(dirname)
    if not os.path.exists(dirname):
        raise Exception('%s does not exist' % dirname)
    if not os.path.isdir(dirname):
        raise Exception('%s is not a directory' % dirname)
    return dirname
