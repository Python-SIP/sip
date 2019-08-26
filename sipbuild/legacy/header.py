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


from ..argument_parser import ArgumentParser
from ..exceptions import handle_exception

from ..module import copy_sip_h, resolve_abi_version


def main():
    """ Install the sip.h file for a private sip module. """

    # Parse the command line.
    parser = ArgumentParser("Install sip.h for a private sip module.")

    parser.add_argument(dest='sip_modules', nargs=1,
            help="the fully qualified name of the sip module", metavar="NAME")

    parser.add_argument('--abi-version',
            help="the ABI version", metavar="VERSION")

    parser.add_argument('--include-dir', default='.',
            help="the directory to install the sip.h file in", metavar="DIR")

    args = parser.parse_args()

    try:
        copy_sip_h(resolve_abi_version(args.abi_version), args.include_dir,
                args.sip_modules[0])
    except Exception as e:
        handle_exception(e)

    return 0
