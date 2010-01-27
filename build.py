#!/usr/bin/env python

# Copyright (c) 2010 Riverbank Computing Limited <info@riverbankcomputing.com>
#
# This file is part of SIP.
#
# This copy of SIP is licensed for use under the terms of the SIP License
# Agreement.  See the file LICENSE for more details.
#
# This copy of SIP may also used under the terms of the GNU General Public
# License v2 or v3 as published by the Free Software Foundation which can be
# found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
#
# SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


"""This script prepares a repository copy of SIP for building and does all the
work of creating a packaged release.  It should be run from a Mercurial
repository or a Mercurial archive.  It is not part of a packaged release.
"""


import os
import shutil
import sys
import tarfile
import zipfile


# The files that need to be patched with the version number.
_PatchedFiles = (
    ('configure.py', ),
    ('sipgen', 'main.c'),
    ('siplib', 'sip.h'),
    ('sphinx', 'conf.py'), ('sphinx', 'introduction.rst'))

# Specific files that are auto-generated and need to be cleaned.
_GeneratedFiles = (
    ('Makefile', ), ('sipconfig.py', ),
    ('sipgen', 'Makefile'), ('sipgen', 'lexer.c'), ('sipgen', 'parser.c'),
    ('sipgen', 'parser.h'), ('sipgen', 'sip'),
    ('siplib', 'Makefile'))

# File types that are auto-generated and need to be cleaned.
_GeneratedFileTypes = ('.pyc', '.o', '.obj', '.so', '.pyd', '.exp', '.exe',
        '.gz', '.zip')

# Directories that are auto-generated and need to be cleaned.
_GeneratedDirs = (
    ('doc', ),
    ('sphinx', 'static'))

# Files in a release.
_ReleasedFiles = ('configure.py.in', 'LICENSE', 'LICENSE-GPL2', 'LICENSE-GPL3',
        'NEWS', 'README', 'sipdistutils.py', 'siputils.py')

# Directories in a release.
_ReleasedDirs = ('custom', 'sipgen', 'siplib', 'specs', 'sphinx')


def _get_release():
    """ Get the release of the package.

    :return:
        A tuple of the full version number and as a three part number.
    """

    # Get the name of the directory containing this file.  It should contain
    # dot files that tell us what sort of package we are.
    base_dir = os.path.dirname(os.path.abspath(__file__))

    numeric_version = '99.99.99'

    if os.path.exists(os.path.join(base_dir, '.hg')):
        # Handle a Mercurial repository.

        from mercurial import hg, ui

        # Get the repository.
        repo = hg.repository(ui.ui(), '.')

        # The changeset we want is the "parent" of the working directory.
        ctx = repo[None].parents()[0]

        for tag in ctx.tags():
            if tag != 'tip':
                version = numeric_version = tag
                break
        else:
            changeset = str(ctx)
            branch = ctx.branch()

            if branch != 'default':
                numeric_version = '%s.99' % branch

            version = 'snapshot-%s-%s' % (numeric_version, changeset)
    else:
        # Handle a Mercurial archive.

        name = os.path.basename(base_dir)
        changeset = "unknown"

        parts = name.split('-')
        if len(parts) > 1:
            name = parts[-1]

            if len(name) == 12:
                # This is the best we can do without access to the repository.
                changeset = name

        version = 'snapshot-%s-%s' % (numeric_version, changeset)

    return version, numeric_version


def _progress(message, quiet):
    """ Show a progress message to the user.

    :param message:
        The text of the message (without a newline).
    :param quiet:
        Set if progress messages should be suppressed.
    """

    if not quiet:
        sys.stdout.write(message)
        sys.stdout.write("\n")


def _rooted_name(root, *path):
    """ Convert a sequence of path components to a name below a root directory.

    :param root:
        The name of the root directory.
    :param \*path:
        The sequence of path components.
    :return:
        The name.
    """

    name = os.path.join(*path)
    name = os.path.join(root, name)

    return name


def _remove_file(name, quiet):
    """ Remove a file, ignoring any errors.

    :param name:
        The name of the file.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    _progress("Removing %s" % name, quiet)

    try:
        os.remove(name)
    except:
        pass


def _create_directory(name, quiet):
    """ Create a directory.

    :param name:
        The name of the directory.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    _progress("Creating directory %s" % name, quiet)

    try:
        os.mkdir(name)
    except:
        pass


def _remove_directory(name, quiet):
    """ Remove a directory, ignoring any errors.

    :param name:
        The name of the directory.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    _progress("Removing directory %s" % name, quiet)
    shutil.rmtree(name, ignore_errors=True)


def _patch_files(root, quiet, clean_patches):
    """ Patch the required files to contain the correct version information.

    :param root:
        The name of the root directory.
    :param quiet:
        Set if progress messages should be suppressed.
    :param clean_patches:
        Set if the original files should be removed after creating the patched
        version.
    """

    release, numeric_version = _get_release()

    hex_version = ''
    for part in numeric_version.split('.'):
        hex_version = '%s%02x' % (hex_version, int(part))

    for f in _PatchedFiles:
        dst_fn = _rooted_name(root, *f)
        src_fn = dst_fn + '.in'

        _progress("Creating %s from %s" % (dst_fn, src_fn), quiet)

        dst = open(dst_fn, 'w')
        src = open(src_fn)

        for line in src:
            line = line.replace('@RM_RELEASE@', release)
            line = line.replace('@RM_VERSION@', numeric_version)
            line = line.replace('@RM_HEXVERSION@', hex_version)

            dst.write(line)

        dst.close()
        src.close()

        if clean_patches:
            _remove_file(src_fn, quiet)


def _misc_prepare(root, quiet):
    """ Perform any additional location dependent preparation.

    :param root:
        The name of the root directory.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    # Sphinx will warn if there is no 'static' directory event though it is
    # unused and empty.
    _create_directory(_rooted_name(root, 'sphinx', 'static'), quiet)


def _run_tools(root, quiet):
    """ Run flex and bison.  This should really be done from make but the SIP
    build system doesn't support it - and it will be gone in SIP v5 anyway.

    :param root:
        The name of the root directory.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    sipgen = _rooted_name(root, 'sipgen')

    lexer = os.path.join(sipgen, 'lexer')
    _progress("Running flex to create %s.c" % lexer, quiet)
    os.system('flex -o%s.c %s.l' % (lexer, lexer))

    parser = os.path.join(sipgen, 'parser')
    _progress("Running bison to create %s.c" % parser, quiet)
    os.system('bison -y -d -o %s.c %s.y' % (parser, parser))


def _run_sphinx(root='.', quiet=True, clean=False):
    """ Run Sphinx to create the HTML documentation.

    :param root:
        The name of the root directory.
    :param quiet:
        Set if progress messages should be suppressed.
    :param clean:
        Set if the .buildinfo file and .doctrees directory should be removed.
    """

    sphinx = _rooted_name(root, 'sphinx')
    doc = _rooted_name(root, 'doc')

    html = os.path.join(doc, 'html')

    if quiet:
        qflag = ' -q'
    else:
        qflag = ''

    _progress("Creating HTML documentation in %s" % html, quiet)
    os.system('sphinx-build%s -b html %s %s' % (qflag, sphinx, html))

    if clean:
        _remove_file(os.path.join(html, '.buildinfo'), quiet)
        _remove_directory(os.path.join(html, '.doctrees'), quiet)


def _prepare_root(root='.', quiet=True, clean_patches=False):
    """ Prepare a root directory.

    :param root:
        The name of the root directory.
    :param quiet:
        Set if progress messages should be suppressed.
    :param clean_patches:
        Set if the original files should be removed after creating the patched
        version.
    """

    _patch_files(root, quiet, clean_patches)
    _run_tools(root, quiet)
    _misc_prepare(root, quiet)


def _clean_root(root='.', quiet=True):
    """ Clean up a root directory.

    :param root:
        The name of the root directory.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    for f in _PatchedFiles:
        _remove_file(_rooted_name(root, *f), quiet)

    for f in _GeneratedFiles:
        _remove_file(_rooted_name(root, *f), quiet)

    for dirpath, dirnames, filenames in os.walk(root):
        try:
            dirnames.remove('.hg')
        except ValueError:
            pass

        for f in filenames:
            for ext in _GeneratedFileTypes:
                if f.endswith(ext):
                    name = os.path.join(dirpath, f)
                    _remove_file(name, quiet)

    for d in _GeneratedDirs:
        _remove_directory(_rooted_name(root, *d), quiet)


def clean(quiet=True):
    """remove all files not stored in the repository"""

    _clean_root(quiet=quiet)

    release, _ = _get_release()
    package = 'sip-' + release
    _remove_directory(package, quiet)


def doc(quiet=True):
    """create the documentation"""

    _run_sphinx(quiet=quiet)


def prepare(quiet=True):
    """prepare for configuration and building"""

    _prepare_root(quiet=quiet)


def release(quiet=True):
    """generate a release package"""

    release, _ = _get_release()

    package = 'sip-' + release
    _remove_directory(package, quiet)
    _create_directory(package, quiet)

    for f in _ReleasedFiles:
        _progress("Adding file %s to release" % f, quiet)
        shutil.copy2(f, package)

    for d in _ReleasedDirs:
        _progress("Adding directory %s to release" % d, quiet)
        shutil.copytree(d, os.path.join(package, d))

    _clean_root(root=package, quiet=quiet)
    _prepare_root(root=package, quiet=quiet, clean_patches=True)
    _run_sphinx(root=package, quiet=quiet, clean=True)

    tar_package = package + '.tar.gz'
    _progress("Creating package %s" % tar_package, quiet)
    tf = tarfile.open(tar_package, 'w:gz')
    tf.add(package)
    tf.close()

    zip_package = package + '.zip'
    _progress("Creating package %s" % zip_package, quiet)
    zf = zipfile.ZipFile(zip_package, 'w', zipfile.ZIP_DEFLATED)

    for dirpath, dirnames, filenames in os.walk(package):
        for f in filenames:
            zf.write(os.path.join(dirpath, f))

    zf.close()


def version(quiet=True):
    """query the version of the package"""

    release, _ = _get_release()

    sys.stdout.write(release + "\n")


if __name__ == '__main__':

    import optparse

    actions = (clean, doc, prepare, release, version)

    class MyParser(optparse.OptionParser):

        def get_usage(self):
            """ Reimplemented to add the description of the actions.  We don't
            use the description because the default formatter strips newlines.
            """

            usage = optparse.OptionParser.get_usage(self)

            usage += "\n" + __doc__ + "\nActions:\n"

            for action in actions:
                usage += "  %-7s  %s\n" % (action.func_name, action.func_doc)

            return usage

    action_names = [action.func_name for action in actions]

    release, _ = _get_release()

    parser = MyParser(
            usage="%%prog [options] %s" % '|'.join(action_names),
            version=release)

    parser.add_option("-q", "--quiet", action='store_true', default=False,
            dest='quiet', help="suppress progress messages")

    options, args = parser.parse_args()

    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    for action in actions:
        if action.func_name == args[0]:
            action(quiet=options.quiet)
            break
    else:
        parser.print_help()
        sys.exit(1)

    sys.exit()
