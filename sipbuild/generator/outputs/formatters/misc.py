# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


def fmt_copying(copying, comment):
    """ Return a formatted %Copying text. """

    s = ''

    for block in copying:
        for line in block.text.rstrip().split('\n'):
            line = line.rstrip()
            if line:
                s = f'{s}{comment} {line}\n'
            else:
                s = f'{s}{comment}\n'

    return s


def fmt_scoped_py_name(scope, py_name):
    """ Return a formatted scoped Python name. """

    if scope is None or scope.is_hidden_namespace:
        scope_s = ''
    else:
        scope_s = fmt_scoped_py_name(scope.scope, None) + scope.py_name.name + '.'

    if py_name is None:
        py_name_s = ''
    else:
        py_name_s = py_name

    return scope_s + py_name_s
