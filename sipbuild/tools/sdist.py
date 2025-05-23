# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 Phil Thompson <phil@riverbankcomputing.com>


from ..abstract_project import AbstractProject
from ..exceptions import handle_exception


def main():
    """ Build an sdist for the project from the command line. """

    try:
        project = AbstractProject.bootstrap('sdist',
                "Build an sdist for the project.")
        sdist_file = project.build_sdist('.')
        project.progress(f"{sdist_file} has been built.")
    except Exception as e:
        handle_exception(e)

    return 0


if __name__ == '__main__':
    import sys

    sys.exit(main())
