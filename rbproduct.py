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

from rbtools import (BuildableProduct, copy_into_directory, patch_file,
        Product, progress, remove_file, TestableProduct, WheelProduct)


class SipProduct(Product, BuildableProduct, TestableProduct, WheelProduct):
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

    # The files and directories to exclude from the working source directory.
    _EXCLUDE = ('METADATA.in', 'Roadmap.rst', 'build.py', 'rbproduct.py',
            '__pycache__')

    # The files that need patching with the release information.
    _PATCH = (
        ('configure.py', ),
        ('sipgen', 'sip.h'),
        ('siplib', 'sip.h.in'),
        ('sphinx', 'conf.py'), ('sphinx', 'introduction.rst'))

    def build(self, platform, build_type=None, debug=False):
        """ Build the product in the current directory using the given
        platform.
        """

        # Configure the build.
        configure_py = ['configure.py']

        if debug:
            configure_py.append('--debug')

        platform.run_script(*configure_py)

        # Do the build.
        platform.run_make()

    def prepare(self, platform, working_src_dir, release_map):
        """ Prepare a new working source directory (prior to building or making
        a release) for a version of the product.
        """

        # Populate the working source directory.
        progress("Populating '{}'".format(working_src_dir))

        for src in os.listdir():
            if src not in self._EXCLUDE:
                copy_into_directory(src, working_src_dir)

        # Patch the files that need it.
        progress("Patching files")

        for f in self._PATCH:
            dst_file = os.path.join(working_src_dir, os.path.join(*f))
            src_file = dst_file + '.in'

            patch_file(src_file, dst_file, release_map)
            remove_file(src_file)

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

        # TODO: run sphinx - should we have a separate release method and do it
        # in that?  Should there be a separate rb-docs for when developing
        # docs? (and therefore a separate build_docs() method called after
        # prepare() by rb-release?)

    ### TestableProduct #######################################################

    def get_imports(self, platform):
        """ Get the sequence of modules to test that they can be imported. """

        return ('sip', )

    ### WheelProduct ##########################################################

    def add_wheel_contents(self, wheel, platform):
        """ Add the product's files to a wheel. """

        wheel.add_contents(
                (os.path.join('siplib', platform.impl_from_module('sip')), ))
