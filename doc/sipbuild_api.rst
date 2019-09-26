.. module:: sipbuild
    :synopsis: The SIP build system.


:mod:`sipbuild` Module Reference
================================

TODO


Classes
-------

.. class:: AbstractBuilder(project, \*\*kwargs)

    TODO


.. class:: AbstractProject

    TODO


.. class:: Bindings(project, name, \*\*kwargs)

    TODO


.. class:: Buildable(project, name)

    TODO


.. class:: BuildableBindings(bindings, fq_name, \*, uses_limited_api=False)

    TODO


.. class:: BuildableExecutable(project, name, target, \*, uses_limited_api=False)

    TODO


.. class:: BuildableModule(project, name, fq_name, \*, uses_limited_api=False)

    TODO


.. class:: Builder(project, \*\*kwargs)

    TODO


.. class:: DistutilsBuilder(project, \*\*kwargs)

    TODO


.. class:: Installable(name, \*, target_subdir=None)

    TODO


.. class:: Option(name, \*, option_type=str, choices=None, default=None, help=None, metavar=None, inverted=False, tools=None)

    TODO


.. class:: Project(\*\*kwargs)

    TODO


Exceptions
----------

.. exception:: PyProjectOptionException(name, text, \*, section_name=None, detail=None)

    TODO


.. exception:: PyProjectUndefinedOptionException(name, \*, section_name=None)

    TODO


.. exception:: UserException(text, \*, detail=None)

    TODO


Functions
---------

.. function:: handle_exception(e)

    TODO
