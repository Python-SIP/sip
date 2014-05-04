#!/usr/bin/python

# Copyright (c) 2014 Riverbank Computing Limited <info@riverbankcomputing.com>
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


# MacHg is not on sys.path, so put it there if we find it.
MacHgPath = '/Applications/MacHg.app/Contents/Plugins/LocalMercurial'
if os.path.isdir(MacHgPath):
    sys.path.append(MacHgPath)


# The files that need to be patched with the version number.
_PatchedFiles = (
    ('configure.py', ),
    ('sipgen', 'sip.h'),
    ('siplib', 'sip.h.in'),
    ('sphinx', 'conf.py'), ('sphinx', 'introduction.rst'))

# Specific files that are auto-generated and need to be cleaned.
_GeneratedFiles = (
    ('Makefile', ), ('sipconfig.py', ),
    ('sipgen', 'Makefile'), ('sipgen', 'lexer.c'), ('sipgen', 'parser.c'),
    ('sipgen', 'parser.h'), ('sipgen', 'sip'),
    ('siplib', 'Makefile'), ('siplib', 'sip.h'), ('siplib', 'siplib.c'),
    ('siplib', 'siplib.sbf'))

# File types that are auto-generated and need to be cleaned.
_GeneratedFileTypes = ('.pyc', '.o', '.obj', '.so', '.pyd', '.exp', '.exe',
        '.gz', '.zip')

# Directories that are auto-generated and need to be cleaned.
_GeneratedDirs = (
    ('__pycache__', ),
    ('doc', ))

# Files in a release.
_ReleasedFiles = ('configure.py.in', 'LICENSE', 'LICENSE-GPL2', 'LICENSE-GPL3',
        'NEWS', 'README', 'sipdistutils.py', 'siputils.py')

# Directories in a release.
_ReleasedDirs = ('sipgen', 'siplib', 'specs', 'sphinx')

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

    log = "changeset:   %s\ndate:        %s\n%s" % (str(ctx), datestr(ctx.date()), ctx.description())

    return log


def _get_release():
    """ Get the release of the package.

    :return:
        A tuple of the full release name, the version number, the hexadecimal
        version number and a list of changelog entries (all as strings).
    """

    # The root directory should contain dot files that tell us what sort of
    # package we are.

    release_suffix = ''

    if os.path.exists(os.path.join(_RootDir, '.hg')):
        # Handle a Mercurial repository.

        from mercurial import hg, ui

        # Get the repository.
        repo = hg.repository(ui.ui(), _RootDir)

        # The last changeset is the "parent" of the working directory.
        ctx = repo[None].parents()[0]

        # If the one before the last changeset has a release tag then the last
        # changeset refers to the tagging and not a genuine change.
        before = ctx.parents()[0]

        version = _release_tag(before)

        if version is not None:
            ctx = before
        else:
            release_suffix = '-snapshot-' + str(ctx)

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
        version = (0, 1, 0)

    major, minor, micro = version

    if micro == 0:
        version = '%d.%d' % (major, minor)
    else:
        version = '%d.%d.%d' % (major, minor, micro)

    release = '%s%s' % (version, release_suffix)
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


def _run_tools(package, quiet):
    """ Run flex and bison.  This should really be done from make but the SIP
    build system doesn't support it - and it will be gone in SIP v5 anyway.

    :param package:
        The name of the optional package directory.
    :param quiet:
        Set if progress messages should be suppressed.
    """

    sipgen = _rooted_name(package, 'sipgen')
    metasrc = os.path.join(sipgen, 'metasrc')

    lexer_l = os.path.join(metasrc, 'lexer.l')
    lexer_c = os.path.join(sipgen, 'lexer.c')
    _progress("Running flex to create %s" % lexer_c, quiet)
    os.system('flex -o%s %s' % (lexer_c, lexer_l))

    parser_y = os.path.join(metasrc, 'parser.y')
    parser_c = os.path.join(sipgen, 'parser.c')
    _progress("Running bison to create %s" % parser_c, quiet)
    os.system('bison -y -d -o %s %s' % (parser_c, parser_y))


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


def _prepare_root(package=None, quiet=True, clean_patches=False):
    """ Prepare a directory.

    :param package:
        The name of the optional package directory.
    :param quiet:
        Set if progress messages should be suppressed.
    :param clean_patches:
        Set if the original files should be removed after creating the patched
        version.
    """

    _patch_files(package, quiet, clean_patches)
    _run_tools(package, quiet)


def _clean_root(package=None, quiet=True):
    """ Clean up a directory.

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


def changelog(output, quiet=True):
    """ The description of each change set going back to the last release are
    written to a file object.

    :param output:
        The file object that the log is written to.
    :param quiet:
        Set if progress messages should be suppressed.
    :return:
        True if the log was written or False if the information wasn't
        available (because this is a Mercurial archive).
    """

    _, _, _, changelog = _get_release()

    if changelog is None:
        return False

    output.write("\n\n".join(changelog) + "\n")

    return True


def clean(quiet=True):
    """ Clean by removing all files and directories not stored in the
    repository.

    :param quiet:
        Set if progress messages should be suppressed.
    """

    _clean_root(quiet=quiet)

    release, _, _, _ = _get_release()
    package = 'sip-' + release
    _remove_directory(package, quiet)


def doc(quiet=True):
    """ Create the documentation.

    :param quiet:
        Set if progress messages should be suppressed.
    """

    _run_sphinx(quiet=quiet)


def prepare(quiet=True):
    """ Prepare for configuration and building by creating all the required
    additional files.

    :param quiet:
        Set if progress messages should be suppressed.
    """

    _prepare_root(quiet=quiet)


def release(quiet=True):
    """ Generate a set of release packages.

    :param quiet:
        Set if progress messages should be suppressed.
    """

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

    _clean_root(package=package, quiet=quiet)
    _prepare_root(package=package, quiet=quiet, clean_patches=True)
    _run_sphinx(package=package, quiet=quiet, clean=True)

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
    """ Get the full version name of the package.  If it is a release then it
    will be of the form x.y[.z].  If it is a snapshot then it will be of the
    form snapshot-x.y[.z]-changeset where x.y[.z] is the version number of the
    next release (not the previous one).  If this is a Mercurial archive
    (rather than a repository) then it does the best it can (based on the name
    of the directory) with the limited information available.

    :param quiet:
        Set if progress messages should be suppressed.
    :return:
        The full version name.
    """

    release, _, _, _ = _get_release()

    return release


if __name__ == '__main__':

    def _changelog(options):
        """get the changelog entries since the last release"""

        if not changelog(sys.stdout, quiet=options.quiet):
            sys.stderr.write("Unable to produce a changelog without a repository\n")
            sys.exit(2)


    def _clean(options):
        """remove all files not stored in the repository"""

        clean(quiet=options.quiet)


    def _doc(options):
        """create the documentation"""

        doc(quiet=options.quiet)


    def _prepare(options):
        """prepare for configuration and building"""

        prepare(quiet=options.quiet)


    def _release(options):
        """generate release packages"""

        release(quiet=options.quiet)


    def _version(options):
        """query the version of the package"""

        sys.stdout.write(version(quiet=options.quiet) + "\n")


    actions = (_changelog, _clean, _doc, _prepare, _release, _version)

    import optparse

    class MyParser(optparse.OptionParser):

        def get_usage(self):
            """ Reimplemented to add the description of the actions.  We don't
            use the description because the default formatter strips newlines.
            """

            usage = optparse.OptionParser.get_usage(self)

            usage += "\n" + __doc__ + "\nActions:\n"

            for action in actions:
                usage += "  %-9s  %s\n" % (action.func_name[1:], action.func_doc)

            return usage


    action_names = [action.func_name[1:] for action in actions]

    rel, _, _, _ = _get_release()

    parser = MyParser(
            usage="%%prog [options] %s" % '|'.join(action_names), version=rel)

    parser.add_option("-q", "--quiet", action='store_true', default=False,
            dest='quiet', help="suppress progress messages")

    options, args = parser.parse_args()

    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    for action in actions:
        if action.func_name[1:] == args[0]:
            action(options)
            break
    else:
        parser.print_help()
        sys.exit(1)

    sys.exit()
