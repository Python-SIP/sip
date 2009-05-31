def setup(app):
    """ This is the entry point for this Sphinx extension. """

    app.add_description_unit('argument-annotation', 'aanno',
            indextemplate='single: %s (argument annotation)')

    app.add_description_unit('class-annotation', 'canno',
            indextemplate='single: %s (class annotation)')

    app.add_description_unit('enum-annotation', 'eanno',
            indextemplate='single: %s (enum annotation)')

    app.add_description_unit('exception-annotation', 'xanno',
            indextemplate='single: %s (exception annotation)')

    app.add_description_unit('function-annotation', 'fanno',
            indextemplate='single: %s (function annotation)')

    app.add_description_unit('license-annotation', 'lanno',
            indextemplate='single: %s (license annotation)')

    app.add_description_unit('mapped-type-annotation', 'manno',
            indextemplate='single: %s (mapped type annotation)')

    app.add_description_unit('typedef-annotation', 'tanno',
            indextemplate='single: %s (typedef annotation)')

    app.add_description_unit('variable-annotation', 'vanno',
            indextemplate='single: %s (variable annotation)')

    app.add_description_unit('directive', 'directive',
            indextemplate='single: %s (directive)')
