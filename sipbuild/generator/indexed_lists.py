# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2025 David Koňařík <dvdkon@konarici.cz>


from collections import defaultdict
from typing import TypeVar, TYPE_CHECKING

from .scoped_name import ScopedName

if TYPE_CHECKING:
    from .specification import (
        CachedName,
        MappedType,
        WrappedClass,
        WrappedEnum,
        WrappedTypedef,
    )


_T = TypeVar("_T")


class IndexedList(list[_T]):
    """
    A list of objects with methods overridden to maintain some indices of those
    objects. By itself, this class is just a list; subclasses can reimplement
    the underscored methods to actually create the indices.
    """

    # Abstract methods, to be implemented by subclass

    def _index_add(self, item):
        """Called when an item is added to the list."""
        ...

    def _index_remove(self, item):
        """Called when an item is removed from the list."""
        ...

    def _index_clear(self):
        """Called at initialisation and then whenever the list is cleared."""
        ...

    # Redefinitions of list methods to keep index up-to-date

    def __init__(self):
        """Create empty list."""
        super().__init__(self)
        self._index_clear()

    def append(self, v):
        """Add v to the end of list, updating indices in the process."""
        super().append(v)
        self._index_add(v)

    def insert(self, i, v):
        """Add v to the list at index i, updating indices in the process."""
        super().insert(i, v)
        self._index_add(v)

    def extend(self, vs):
        """
        Add elements of vs to the end of the list, updating indices in the
        process.
        """
        for v in vs:
            self.append(v)

    def remove(self, v):
        """
        Remove first occurrence of v from the list, updating indices in the
        process.
        """
        super().remove(v)
        self._index_remove(v)

    def clear(self):
        """
        Delete all elements from the list, updating indices in the process.
        """
        super().clear()
        self._index_clear()

    def __setitem__(self, i, v):
        """Set element at index i to v, updating indices in the process."""
        self._index_remove(self[i])
        super().__setitem__(i, v)
        self._index_add(v)

    def __delitem__(self, i):
        """Delete element at index i, updating indices in the process."""
        self._index_remove(self[i])
        super().__delitem__(i)


class IndexedClassList(IndexedList['WrappedClass']):
    """
    A list of WrappedClasss keeping the following indices:
    - classes by fq_cpp_name
    """

    def _index_clear(self):
        """Set up indices. See IndexedList._index_clear()."""

        self._by_cppname = defaultdict(list)

    def _index_add(self, klass):
        """Add element to indices. See IndexedList._index_add()."""

        self._by_cppname[klass.iface_file.fq_cpp_name].append(klass)

    def _index_remove(self, klass):
        """Remove element from indices. See IndexedList._index_add()."""

        self._by_cppname[klass.iface_file.fq_cpp_name].remove(klass)

    def by_fq_cpp_name(self, name):
        """Find WrappedClasses by their .iface_file.fq_cpp_name."""

        return self._by_cppname[name]


class IndexedEnumList(IndexedList['WrappedEnum']):
    """
    A list of WrappedEnums keeping the following indices:
    - enums by fq_cpp_name
    """

    def _index_clear(self):
        """ Initialise the indices. """

        self._by_cppname = {}

    def _index_add(self, enum):
        """ Add a WrappedEnum to the indices. """

        if enum.fq_cpp_name:
            self._by_cppname[enum.fq_cpp_name] = enum

    def _index_remove(self, enum):
        """ Remove a WrappedEnum from the indices. """

        if enum.fq_cpp_name:
            del self._by_cppname[enum.fq_cpp_name]

    def by_fq_cpp_name(self, name):
        """ Find a WrappedEnum by its .fq_cpp_name. """

        return self._by_cppname.get(name)


class IndexedCachedNameList(IndexedList['CachedName']):
    """
    A list of CachedNames keeping an index of the names by string values.
    """

    def _index_clear(self):
        """Set up indices. See IndexedList._index_clear()."""
        self._by_name = {}

    def _index_add(self, name):
        """Add CachedName to indices. See IndexedList._index_add()."""
        assert name.name not in self._by_name
        self._by_name[name.name] = name

    def _index_remove(self, name):
        """Remove CachedName from indices. See IndexedList._index_add()."""
        del self._by_name[name.name]

    def by_name(self, name: str):
        """Find CachedName by its .name."""
        return self._by_name.get(name)


class IndexedMappedTypeList(IndexedList['MappedType']):
    """
    A list of MappedTypes keeping an index of the names by readable base names.
    """

    def _index_clear(self):
        """Set up indices. See IndexedList._index_clear()."""
        self._by_readable_base_name = defaultdict(list)

    def _index_add(self, mapped_type):
        """Add MappedType to indices. See IndexedList._index_add()."""
        self._by_readable_base_name[
            mapped_type.iface_file.fq_cpp_name.readable_base_name
        ].append(mapped_type)

    def _index_remove(self, mapped_type):
        """Remove MappedType from indices. See IndexedList._index_add()."""
        self._by_readable_base_name[
            mapped_type.iface_file.fq_cpp_name.readable_base_name
        ].remove(mapped_type)

    def by_readable_base_name(self, name: str):
        """Find MappedTypes by their .iface_file.fq_cpp_name.readable_base_name."""
        return self._by_readable_base_name[name]


class IndexedTypedefList(IndexedList['WrappedTypedef']):
    """
    A list of WrappedTypedefs keeping an index of the typedefs by fq_cpp_name.
    """

    def _index_clear(self):
        """Set up indices. See IndexedList._index_clear()."""
        self._by_fq_cpp_name = {}

    def _index_add(self, typedef):
        """Add WrappedTypedef to indices. See IndexedList._index_add()."""
        name = tuple(typedef.fq_cpp_name._name)
        assert name not in self._by_fq_cpp_name
        self._by_fq_cpp_name[name] = typedef

    def _index_remove(self, typedef):
        """Remove WrappedTypedef from indices. See IndexedList._index_add()."""
        del self._by_fq_cpp_name[tuple(typedef.fq_cpp_name._name)]

    def by_fq_cpp_name(self, name: ScopedName):
        """Find WrappedTypedef by its .fq_cpp_name."""
        return self._by_fq_cpp_name.get(tuple(name._name))
