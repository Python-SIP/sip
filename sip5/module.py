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

    # There will be a default setup.cfg file with generic details (including a
    # long_description link to a generic README).  The following macros will be
    # expanded (for both default and user supplied files):

    #    @SIP5_PACKAGE@ the top-level package name: sip_module.split('.')[0]
    #    @SIP5_NAME@ the PyPI name of the package: sip_module.replace('.', '-')
    #    @SIP5_VERSION@ the version number of the package (ie. the module ABI
    #        version or something derived from it)

    # setup.cfg requires setuptools v30.3.0 to there needs to be a
    # pyproject.toml file to specify that as a minimum requirement.

    # The sip.h file will not be installed anywhere but the eventual build
    # system will automatically add its location to the compiler invocation as
    # required. (Might reconsider later.)

    # Do we always install the .pyi file or do we add it as an option?  Do we
    # leave that to the setup.cfg file?
