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


# Define the current ABI version number.  SIP must handle modules with the
# same major number and with the same or earlier minor number.  Whenever
# members are added to non-embedded data structures they must be appended, the
# minor number incremented and the maintenance number set to 0.  Whenever data
# structure members are removed or their offset changed then the major number
# must be incremented and the minor and maintenance numbers set to 0.  Other
# changes to the module source require the maintenance number to be
# incremented.

ABI_MAJOR = 12
ABI_MINOR = 6
ABI_MAINTENANCE = 0


# ABI version history:
#
# 12.6  Added sip_api_long_as_size_t() to the public API.
#       Added the '=' format character to sip_api_build_result().
#       Added the '=' format character to sip_api_parse_result_ex().
#
# 12.5  Replaced the sipConvertFromSliceObject() macro with
#       sip_api_convert_from_slice_object() in the public API.
#
# 12.4  Added sip_api_instance_destroyed_ex() to the private API.
#
# 12.3  Added SIP_TYPE_SCOPED_ENUM to the sipTypeDef flags.
#       Added sip_api_convert_to_enum() to the public API.
#       Added sip_api_convert_to_bool() to the public API.
#       Added sip_api_long_as_char(), sip_api_long_as_signed_char(),
#       sip_api_long_as_unsigned_char(), sip_api_long_as_short(),
#       sip_api_long_as_unsigned_short(), sip_api_long_as_int(),
#       sip_api_long_as_unsigned_int(), sip_api_long_as_long(),
#       sip_api_long_as_unsigned_long(), sip_api_long_as_long_long(),
#       sip_api_long_as_unsigned_long_long() to the public API.
#       Deprecated sip_api_can_convert_to_enum().
#
# 12.2  Added sip_api_print_object() to the public API.
#       Renamed sip_api_common_dtor() to sip_api_instance_destroyed() and added
#       it to the public API.
#       Added sipEventType and sip_api_register_event_handler() to the public
#       API.
#
# 12.1  Added sip_api_enable_gc() to the public API.
#
# 12.0  Added SIP_TYPE_LIMITED_API to the sipTypeDef flags.
#       Added sip_api_py_type_dict() and sip_api_py_type_name() to the public
#       API.
#       Added sip_api_set_new_user_type_handler() to the public API.
#       Added sip_api_is_user_type() to the public API.
#       Added sip_api_set_type_user_data() and sip_api_get_type_user_data() to
#       the public API.
#       Added sip_api_set_user_object() and sip_api_get_user_object() to the
#       public API.
#       Added sip_api_get_method() and sip_api_from_method() to the public API.
#       Added sip_api_get_c_function() to the public API.
#       Added sip_api_get_date() and sip_api_from_date() to the public API.
#       Added sip_api_get_datetime() and sip_api_from_datetime() to the public
#       API.
#       Added sip_api_get_time() and sip_api_from_time() to the public API.
#       Added sip_api_get_frame() to the public API.
#       Added sip_api_check_plugin_for_type() to the public API.
#       Added sip_api_unicode_new(), sip_api_unicode_write() and
#       sip_api_unicode_data() to the public API.
#       Added sip_api_get_buffer_info() and sip_api_relese_buffer_info() to the
#       public API.
#       Added sip_api_call_procedure_method() to the public API.
#       Added sip_api_is_owned_by_python() to the private API.
#       Added sip_api_is_derived_class() to the private API.
#       Removed the im_version member from sipImportedModuleDef.
#       Removed the im_module member from sipImportedModuleDef.
#       Removed the em_version member from sipExportedModuleDef.
#       Removed the em_virthandlers member from sipExportedModuleDef.
#       Re-ordered the API functions.
#
# 11.3  Added sip_api_get_interpreter() to the public API.
#
# 11.2  Added sip_api_get_reference() to the private API.
#
# 11.1  Added sip_api_invoke_slot_ex().
#
# 11.0  Added the pyqt5QtSignal and pyqt5ClassTypeDef structures.
#       Removed qt_interface from pyqt4ClassTypeDef.
#       Added hack to pyqt4QtSignal.
#
# 10.1  Added ctd_final to sipClassTypeDef.
#       Added ctd_init_mixin to sipClassTypeDef.
#       Added sip_api_get_mixin_address() to the public API.
#       Added sip_api_convert_from_new_pytype() to the public API.
#       Added sip_api_convert_to_array() to the public API.
#       Added sip_api_convert_to_typed_array() to the public API.
#       Added sip_api_register_proxy_resolver() to the public API.
#       Added sip_api_init_mixin() to the private API.
#       Added qt_interface to pyqt4ClassTypeDef.
#
# 10.0  Added sip_api_set_destroy_on_exit().
#       Added sip_api_enable_autoconversion().
#       Removed sip_api_call_error_handler_old().
#       Removed sip_api_start_thread().
#
# 9.2   Added sip_gilstate_t and SIP_RELEASE_GIL to the public API.
#       Renamed sip_api_call_error_handler() to
#       sip_api_call_error_handler_old().
#       Added the new sip_api_call_error_handler() to the private API.
#
# 9.1   Added the capsule type.
#       Added the 'z' format character to sip_api_build_result().
#       Added the 'z', '!' and '$' format characters to
#       sip_api_parse_result_ex().
#
# 9.0   Changed the sipVariableGetterFunc signature.
#       Added sip_api_parse_result_ex() to the private API.
#       Added sip_api_call_error_handler() to the private API.
#       Added em_virterrorhandlers to sipExportedModuleDef.
#       Re-ordered the API functions.
#
# 8.1   Revised the sipVariableDef structure.
#       sip_api_get_address() is now part of the public API.
#
# 8.0   Changed the size of the sipSimpleWrapper structure.
#       Added sip_api_get_address().
#
# 7.1   Added the 'H' format character to sip_api_parse_result().
#       Deprecated the 'D' format character of sip_api_parse_result().
#
# 7.0   Added sip_api_parse_kwd_args().
#       Added sipErrorState, sip_api_add_exception().
#       The type initialisation function is now passed a dictionary of keyword
#       arguments.
#       All argument parsers now update a set of error messages rather than an
#       argument count.
#       The signatures of sip_api_no_function() and sip_api_no_method() have
#       changed.
#       Added ctd_docstring to sipClassTypeDef.
#       Added vf_docstring to sipVersionedFunctionDef.
#
# 6.0   Added the sipContainerDef structure to define the contents of a class
#       or mapped type.  Restructured sipClassDef and sipMappedTypeDef
#       accordingly.
#       Added the 'r' format character to sip_api_parse_args().
#       Added the 'r' format character to sip_api_call_method() and
#       sip_api_build_result().
#       Added the assignment, array and copy allocation helpers.
#
# 5.0   Added sip_api_is_api_enabled().
#       Renamed the td_version_nr member of sipTypeDef to be int and where -1
#       indicates it is not versioned.
#       Added the em_versions member to sipExportedModuleDef.
#       Added the em_versioned_functions member to sipExportedModuleDef.
#
# 4.0   Much refactoring.
#
# 3.8   Added sip_api_register_qt_metatype() and sip_api_deprecated().
#       Added qt_register_meta_type() to the Qt support API.
#       The C/C++ names of enums and types are now always defined in the
#       relevant structures and don't default to the Python name.
#       Added the 'XE' format characters to sip_api_parse_args().
#
# 3.7   Added sip_api_convert_from_const_void_ptr(),
#       sip_api_convert_from_void_ptr_and_size() and
#       sip_api_convert_from_const_void_ptr_and_size().
#       Added the 'g' and 'G' format characters (to replace the now deprecated
#       'a' and 'A' format characters) to sip_api_build_result(),
#       sip_api_call_method() and sip_api_parse_result().
#       Added the 'k' and 'K' format characters (to replace the now deprecated
#       'a' and 'A' format characters) to sip_api_parse_args().
#       Added sip_api_invoke_slot().
#       Added sip_api_parse_type().
#       Added sip_api_is_exact_wrapped_type().
#       Added the td_assign and td_qt fields to the sipTypeDef structure.
#       Added the mt_assign field to the sipMappedType structure.
#
# 3.6   Added the 'g' format character to sip_api_parse_args().
#
# 3.5   Added the td_pickle field to the sipTypeDef structure.
#       Added sip_api_transfer_break().
#
# 3.4   Added qt_find_connection() to the Qt support API.
#       Added sip_api_string_as_char(), sip_api_unicode_as_wchar(),
#       sip_api_unicode_as_wstring(), sip_api_find_class(),
#       sip_api_find_named_enum() and sip_api_parse_signature().
#       Added the 'A', 'w' and 'x' format characters to sip_api_parse_args(),
#       sip_api_parse_result(), sip_api_build_result() and
#       sip_api_call_method().
#
# 3.3   Added sip_api_register_int_types().
#
# 3.2   Added sip_api_export_symbol() and sip_api_import_symbol().
#
# 3.1   Added sip_api_add_mapped_type_instance().
#
# 3.0   Moved the Qt support out of the sip module and into PyQt.  This is
#       such a dramatic change that there is no point in attempting to maintain
#       backwards compatibility.
#
# 2.0   Added the td_flags field to the sipTypeDef structure.
#       Added the first_child, sibling_next, sibling_prev and parent fields to
#       the sipWrapper structure.
#       Added the td_traverse and td_clear fields to the sipTypeDef structure.
#       Added the em_api_minor field to the sipExportedModuleDef structure.
#       Added sip_api_bad_operator_arg().
#       Added sip_api_wrapper_check().
#
# 1.1   Added support for __pos__ and __abs__.
#
# 1.0   Removed all deprecated parts of the API.
#       Removed the td_proxy field from the sipTypeDef structure.
#       Removed the create proxy function from the 'q' and 'y' format
#       characters to sip_api_parse_args().
#       Removed sip_api_emit_to_slot().
#       Reworked the enum related structures.
#
# 0.2   Added the 'H' format character to sip_api_parse_args().
#
# 0.1   Added sip_api_add_class_instance().
#       Added the 't' format character to sip_api_parse_args().
#       Deprecated the 'J' and 'K' format characters to sip_api_parse_result().
#
# 0.0   The original version.
