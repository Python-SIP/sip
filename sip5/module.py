# Copyright (c) 2019, Riverbank Computing Limited
# All rights reserved.
#
# This copy of SIP is licensed for use under the terms of the SIP License
# Agreement.  See the file LICENSE for more details.
#
# This copy of SIP may also used under the terms of the GNU General Public
# License v2 or v3 as published by the Free Software Foundation which can be
# found in the files LICENSE-GPL2 and LICENSE-GPL3 included in this package.
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


def module(sip_module, module_dir=None, setup_cfg=None):
    """ Create the code to build a sip module. """

    # sip_module: eg. 'PyQt5.sip'
    # module_dir: the directory to create everything in, default to cwd, create
    #    if doesn't exist, leave any existing contents in place.
    # setup_cfg: the name of an optional setup.cfg file.

    # By default this will create an sdist .tar.gz which can be uploaded to
    # PyPI.  Run 'pip wheel sdist.tar.gz' to create the wheel.  There should be
    # an option to leave it as a directory and to not create the .tar.gz.

    # There will be a default setup.cfg file with generic details (including a
    # long_description link to a generic README).  The following macros will be
    # expanded (for both default and user supplied files):

    #    @SIP5_PACKAGE@ the top-level package name: sip_module.split('.')[0]
    #    @SIP5_NAME@ the PyPI name of the package: sip_module.replace('.', '_')
    #    @SIP5_VERSION@ the version number of the package

    # setup.cfg requires setuptools v30.3.0 to there needs to be a
    # pyproject.toml file to specify that as a minimum requirement.

    # Need an option to install the sip.h file in a specified directory.  Do we
    # define the name of the module in sip.h (DRY) and to be picked up by the
    # generated code? Yes.

    # The setup.cfg file determines if the sip.pyi is included.  The default
    # setup.cfg does not but should include a commented out section that does.

    # Add an option to provide an optional README file so that the default
    # setup.cfg can be used in most cases.

    # The version number is major.minor.maint with major.minor corresponding to
    # the ABI major.minor.  The maint is incremented when the implementation of
    # the current ABI version changes.
