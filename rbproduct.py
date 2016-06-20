# Copyright (c) 2016, Riverbank Computing Limited
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


import os

from rbtools import (BuildableProduct, copy_into_directory, get_configuration,
        patch_file, progress, remove_directory, remove_file, TestableProduct,
        WheelProduct)


class SipProduct(BuildableProduct, TestableProduct, WheelProduct):
    """ The encapsulation of the sip product. """

    ### Product ###############################################################

    # The name of the product.
    name = 'sip'

    def installation(self, name):
        """ Convert a file or directory name relative to the installation
        directory of the product to one that is relative to site-packages.
        """

        return name

    ### BuildableProduct ######################################################

    # The supported build types.
    build_types = {
        'develop-build':    "a debug build of the code generator and module",
        'docs-build':       "a build of the documentation",
        'minimal-build':    "a build containing the code generator",
        'full-build':       "a full build",
        'posix':            "a source release for any POSIX target",
        'win':              "a source release for any Windows target",
        'wheel':            "a binary release for the native target",
    }
    default_build_type = 'develop-build'

    # The files and directories to copy to the working source directory.
    _INCLUDE = ('LICENSE', 'LICENSE-GPL2', 'LICENSE-GPL3', 'NEWS', 'README',
            'configure.py.in', 'sip.pyi', 'sipdistutils.py', 'sipgen',
            'siplib', 'siputils.py', 'specs', 'sphinx')

    # The files that need patching with the release information.
    _PATCH = (
        ('configure.py', ),
        ('sipgen', 'sip.h'),
        ('siplib', 'sip.h.in'),
        ('sphinx', 'conf.py'), ('sphinx', 'introduction.rst'))

    def build(self, platform):
        """ Build the product in the current directory using the given
        platform.
        """

        # Configure the build.
        configure_py = ['configure.py']

        if self.build_type == 'develop-build':
            configure_py.append('--debug')

        if self.build_type == 'minimal-build':
            configuration = get_configuration()
            minimal_bin_dir = configuration.get('sip', 'minimal_bin_dir',
                    platform, ispath=True)

            if minimal_bin_dir:
                configure_py.extend(['--bindir', minimal_bin_dir])

        platform.run_script(*configure_py)

        # Do the build.
        platform.run_make()

    def install(self, platform):
        """ Install the product from the current directory using the given
        platform.
        """

        if self.build_type == 'minimal-build':
            platform.run_make(target='install', chdir='sipgen')
        elif self.build_type != 'docs-build':
            platform.run_make()

    def prepare(self, platform, working_src_dir, macros):
        """ Prepare a new working source directory (prior to building or making
        a release) for a version of the product.
        """

        # As far as preparation goes, any release corresponds to a full build
        # (although a wheel doesn't need the code generator).
        prep_type = self.build_type if self.is_build() else 'full-build'

        # Populate the working source directory.
        progress("Populating the '{}' directory".format(working_src_dir))

        for src in self._INCLUDE:
            copy_into_directory(src, working_src_dir)

        # Patch the files that need it.
        for f in self._PATCH:
            patched = [working_src_dir]
            patched += f
            patch_file(os.path.join(*patched), macros, prototype='.in')

        if prep_type in ('develop-build', 'minimal-build', 'full-build'):
            # Run bison and flex.
            sipgen = os.path.join(working_src_dir, 'sipgen')
            metasrc = os.path.join(sipgen, 'metasrc')

            lexer_l = os.path.join(metasrc, 'lexer.l')
            lexer_c = os.path.join(sipgen, 'lexer.c')
            progress("Running flex to create {}".format(lexer_c))
            platform.run('flex', '-o', lexer_c, lexer_l)

            parser_y = os.path.join(metasrc, 'parser.y')
            parser_c = os.path.join(sipgen, 'parser.c')
            progress("Running bison to create {}".format(parser_c))
            platform.run('bison', '-y', '-d', '-o', parser_c, parser_y)

        if prep_type in ('docs-build', 'full-build'):
            # Run sphinx.
            sphinx = os.path.join(working_src_dir, 'sphinx')
            doc = os.path.join(working_src_dir, 'doc')
            html = os.path.join(doc, 'html')

            progress("Creating HTML documentation in {}".format(html))
            platform.run('sphinx-build', '-b', 'html', sphinx, html)

            remove_file(os.path.join(html, '.buildinfo'))
            remove_directory(os.path.join(html, '.doctrees'))

    ### TestableProduct #######################################################

    def get_imports(self, platform):
        """ Get the sequence of modules to test that they can be imported. """

        return ('sip', )

    ### WheelProduct ##########################################################

    def add_wheel_contents(self, wheel, platform):
        """ Add the product's files to a wheel. """

        wheel.add_contents(
                (os.path.join('siplib', platform.impl_from_module('sip')), ))
