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

# The root directory, i.e. the one containing this script.
_RootDir = os.path.dirname(os.path.abspath(__file__))


def _release_tag(ctx):
    """ Get the release tag (i.e. a tag of the form x.y[.z]) converted to a
    3-tuple of integers if there is one.

    :param ctx:
        The Mercurial change context containing the tags.
    :return:
        The 3-tuple of integers or None if there was no release tag.
    """

    for tag in ctx.tags():
        if tag != 'tip':
            parts = tag.split('.')

            if len(parts) == 2:
                parts.append('0')

            if len(parts) == 3:
                major, minor, micro = parts

                try:
                    return (int(major), int(minor), int(micro))
                except ValueError:
                    pass

    return None


def _format_changelog(ctx):
    """ Format the log message for a changeset.

    :param ctx:
        The Mercurial change context containing the tags.
    :return:
        The formatted change log.
    """

    from mercurial.util import datestr

    date = datestr(ctx.date(), '%a, %d %b %Y %H:%M:%S %1%2')

    log = "Changeset %s at %s\n%s" % (str(ctx), date, ctx.description())

    return log


def _get_release():
    """ Get the release of the package.

    :return:
        A tuple of the full release name, the version number, the hexadecimal
        version number and a list of changelog entries (all as strings).
    """

    # The root directory should contain dot files that tell us what sort of
    # package we are.

    release_prefix = 'snapshot-'
    release_suffix = ''

    if os.path.exists(os.path.join(_RootDir, '.hg')):
        # Handle a Mercurial repository.

        from mercurial import hg, ui

        # Get the repository.
        repo = hg.repository(ui.ui(), _RootDir)

        # The changeset we want is the "parent" of the working directory.
        ctx = repo[None].parents()[0]

        version = _release_tag(ctx)

        if version is not None:
            release_prefix = ''
        else:
            release_suffix = '-' + str(ctx)

        changelog = [_format_changelog(ctx)]

        # Go back through the line of the first parent to find the last
        # release.
        parent_version = None

        parents = ctx.parents()
        while len(parents) != 0:
            parent_ctx = parents[0]

            changelog.append(_format_changelog(parent_ctx))

            parent_version = _release_tag(parent_ctx)
            if parent_version is not None:
                break

            parents = parent_ctx.parents()

        if version is None and parent_version is not None:
            # This is a snapshot so work out what the next version will be
            # based on the previous version.
            major, minor, micro = parent_version

            if ctx.branch() == 'default':
                minor += 1

                # This should be 0 anyway.
                micro = 0
            else:
                micro += 1

            version = (major, minor, micro)
    else:
        # Handle a Mercurial archive.

        changelog = None
        name = os.path.basename(_RootDir)

        release_suffix = "-unknown"
        version = None

        parts = name.split('-')
        if len(parts) > 1:
            name = parts[-1]

            if len(name) == 12:
                # This is the best we can do without access to the repository.
                release_suffix = '-' + name

    # Format the results.
    if version is None:
        version = (99, 99, 99)

    major, minor, micro = version

    if micro == 0:
        version = '%d.%d' % (major, minor)
    else:
        version = '%d.%d.%d' % (major, minor, micro)

    release = '%s%s%s' % (release_prefix, version, release_suffix)
    hex_version = '%02x%02x%02x' % (major, minor, micro)

    return release, version, hex_version, changelog


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


def _rooted_name(package, *path):
    """ Convert a sequence of path components to a name below the root
    directory.

    :param package:
        The name of the optional package directory.
    :param \*path:
        The sequence of path components.
    :return:
        The name.
    """

    name = os.path.join(*path)

    if package is not None:
        name = os.path.join(package, name)

    name = os.path.join(_RootDir, name)

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


def _patch_files(package, quiet, clean_patches):
    """ Patch the required files to contain the correct version information.

    :param package:
        The name of the optional package directory.
    :param quiet:
        Set if progress messages should be suppressed.
    :param clean_patches:
        Set if the original files should be removed after creating the patched
        version.
    """

    release, version, hex_version, _ = _get_release()

    for f in _PatchedFiles:
        dst_fn = _rooted_name(package, *f)
        src_fn = dst_fn + '.in'

        _progress("Patching %s" % dst_fn, quiet)

        dst = open(dst_fn, 'w')
        src = open(src_fn)

        for line in src:
            line = line.replace('@RM_RELEASE@', release)
            line = line.replace('@RM_VERSION@', version)
            line = line.replace('@RM_HEXVERSION@', hex_version)

            dst.write(line)

        dst.close()
        src.close()

        if clean_patches:
            _remove_file(src_fn, quiet)


def _misc_prepare(package, quiet):
    """ Perform any additional location dependent preparation.

    :param package:
        The name of the optional package directory.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    # Sphinx will warn if there is no 'static' directory event though it is
    # unused and empty.
    _create_directory(_rooted_name(package, 'sphinx', 'static'), quiet)


def _run_tools(package, quiet):
    """ Run flex and bison.  This should really be done from make but the SIP
    build system doesn't support it - and it will be gone in SIP v5 anyway.

    :param package:
        The name of the optional package directory.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    sipgen = _rooted_name(package, 'sipgen')

    lexer = os.path.join(sipgen, 'lexer')
    _progress("Running flex to create %s.c" % lexer, quiet)
    os.system('flex -o%s.c %s.l' % (lexer, lexer))

    parser = os.path.join(sipgen, 'parser')
    _progress("Running bison to create %s.c" % parser, quiet)
    os.system('bison -y -d -o %s.c %s.y' % (parser, parser))


def _run_sphinx(package=None, quiet=True, clean=False):
    """ Run Sphinx to create the HTML documentation.

    :param package:
        The name of the optional package directory.
    :param quiet:
        Set if progress messages should be suppressed.
    :param clean:
        Set if the .buildinfo file and .doctrees directory should be removed.
    """

    sphinx = _rooted_name(package, 'sphinx')
    doc = _rooted_name(package, 'doc')

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


def _prepare_root(root=None, quiet=True, clean_patches=False):
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


def _clean_root(package=None, quiet=True):
    """ Clean up the directory.

    :param package:
        The name of the optional package directory.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    for f in _PatchedFiles:
        _remove_file(_rooted_name(package, *f), quiet)

    for f in _GeneratedFiles:
        _remove_file(_rooted_name(package, *f), quiet)

    root = _RootDir
    if package is not None:
        root = os.path.join(root, package)

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
        _remove_directory(_rooted_name(package, *d), quiet)


def changelog(quiet=True, out_file=None):
    """get the relevant changelog entries"""

    _, _, _, changelog = _get_release()

    if changelog is None:
        sys.stderr.write("Unable to produce a changelog without a repository\n")
        sys.exit(2)

    if out_file is None:
        out_file = sys.stdout

    out_file.write("\n\n".join(changelog) + "\n")


def clean(quiet=True):
    """remove all files not stored in the repository"""

    _clean_root(quiet=quiet)

    release, _, _, _ = _get_release()
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

    release, _, _, _ = _get_release()

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

    release, _, _, _ = _get_release()

    sys.stdout.write(release + "\n")


if __name__ == '__main__':

    import optparse

    actions = (changelog, clean, doc, prepare, release, version)

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

    release, _, _, _ = _get_release()

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
