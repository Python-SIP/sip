# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2026 Phil Thompson <phil@riverbankcomputing.com>


def test_visit_wrappers(module):
    klasses = [module.Klass(i) for i in range(10)]
    visited = []

    def visitor(obj):
        if isinstance(obj, module.Klass):
            visited.append(obj.id())

    module.visit_wrappers(visitor)

    assert sorted(visited) == list(range(10))


def test_visit_wrappers_changing_object_map(module):
    klasses = [module.Klass(i) for i in range(100)]
    created = []
    visited = []

    def visitor(obj):
        if not isinstance(obj, module.Klass):
            return

        visited.append(obj.id())

        if len(visited) == 1:
            # Create enough wrappers to make the object map grow, then destroy
            # the wrappers that have not been visited.
            created.extend(module.Klass(-1) for _ in range(1000))
            klasses.clear()

    module.visit_wrappers(visitor)

    # Only wrappers that exist when they would be visited are visited.
    assert visited[0] >= 0
    assert all(i == -1 for i in visited[1:])
