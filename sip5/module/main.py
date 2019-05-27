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


from argparse import ArgumentParser

from ..exceptions import handle_exception
from ..version import SIP_VERSION_STR

from .module import module


def main():
    """ Create the code to build a sip module. """

    # Parse the command line.
    parser = ArgumentParser(
            description="Generate the code for a sip extension module.")

    parser.add_argument('-V', '--version', action='version',
            version=SIP_VERSION_STR)

    parser.add_argument(dest='sip_modules', nargs=1,
            help="the fully qualified name of the sip module",
            metavar="NAME")

    parser.add_argument('--documentation-dir', dest='documentation_dir',
            help="the name of the directory where sip.rst will be installed",
            metavar="DIR")

    parser.add_argument('--include-dir', dest='include_dir',
            help="the name of the directory where sip.h will be installed",
            metavar="DIR")

    parser.add_argument('--module-dir', dest='module_dir',
            help="the name of the directory where the code will be generated",
            metavar="DIR")

    parser.add_argument('--no-sdist', dest='no_sdist', action='store_true',
            default=False,
            help="do not generate the .tar.gz sdist file")

    parser.add_argument('--setup-cfg', dest='setup_cfg',
            help="the name of the setup.cfg file to use",
            metavar="FILE")

    args = parser.parse_args()

    try:
        module(sip_module=args.sip_modules[0],
                documentation_dir=args.documentation_dir,
                include_dir=args.include_dir, module_dir=args.module_dir,
                no_sdist=args.no_sdist, setup_cfg=args.setup_cfg)
    except Exception as e:
        handle_exception(e)

    return 0
