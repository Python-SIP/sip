.. module:: sipbuild
    :synopsis: The SIP build system.


:mod:`sipbuild` Module Reference
================================

TODO


.. class:: AbstractBuilder(project, \*\*kwargs)

    TODO


.. class:: AbstractProject

    TODO


.. class:: Bindings(project, name, \*\*kwargs)

    TODO


.. class:: Buildable(project, name)

    TODO


.. class:: BuildableBindings(bindings, fq_name, uses_limited_api=False)

    TODO


.. class:: BuildableExecutable(project, name, target, uses_limited_api=False)

    TODO


.. class:: BuildableModule(project, name, fq_name, uses_limited_api=False)

    TODO


.. class:: Builder(project, \*\*kwargs)

    TODO


.. class:: DistutilsBuilder(project, \*\*kwargs)

    TODO


.. function:: handle_exception(e)

    TODO


.. class:: Installable(name, target_subdir=None)

    TODO


.. class:: Option(name, \*, option_type=str, choices=None, default=None, help='', metavar=None, inverted=False, tools=None)

    TODO


.. exception:: PyProjectOptionException(name, text, \*, section_name=None, detail='')

    TODO


.. exception:: PyProjectUndefinedOptionException(name, \*, section_name=None)

    TODO


.. class:: Project(\*\*kwargs)

    TODO


.. exception:: UserException(text, \*, detail='')

    TODO
